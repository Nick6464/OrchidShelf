# VL6180X Water Level Control

This guide explains how to set up the VL6180X Time-of-Flight sensors for automatic water level control in your flood irrigation system.

## Overview

The system uses VL6180X Time-of-Flight sensors to measure actual water depth in each tray. The pumps automatically stop when the target water level is reached. This provides:

- **Consistent water levels** across cycles
- **Compensation for pump variations** and tubing differences  
- **Automatic adaptation** to reservoir levels
- **Safety timeout** if target depth isn't reached

## Hardware Requirements

### VL6180X Sensors

Each bin needs one VL6180X sensor mounted above the tray pointing downward. The VL6180X measures 5-200mm range with 1mm accuracy.

**Important:** VL6180X sensors have a fixed I2C address (0x29). For multiple sensors, you need either:

1. **I2C Multiplexer (Recommended)** - Use a TCA9548A or PCA9548A multiplexer to connect up to 8 sensors
2. **XSHUT Address Remapping** - Use XSHUT pins to disable sensors during initialization and change addresses

### Mounting

Mount sensors 150-250mm above the empty tray bottom:
- Sensor pointing straight down
- Clear line of sight to water surface
- Away from tray edges to avoid false readings
- Protected from water spray during filling

## I2C Multiplexer Setup (Recommended)

### TCA9548A Wiring

```
TCA9548A → ESP32
VCC      → 3.3V
GND      → GND
SDA      → GPIO21
SCL      → GPIO22

VL6180X Sensor 1 → TCA9548A Channel 0
VL6180X Sensor 2 → TCA9548A Channel 1
VL6180X Sensor 3 → TCA9548A Channel 2
VL6180X Sensor 4 → TCA9548A Channel 3

Each VL6180X:
VCC → 3.3V
GND → GND
SDA → Channel SDA
SCL → Channel SCL
```

### Configuration with Multiplexer

Update `floodshelf_depth.yaml` to add multiplexer support:

```yaml
# I2C Multiplexer
i2c:
  - id: bus_a
    sda: GPIO21
    scl: GPIO22
    scan: true
    frequency: 400kHz

# Add multiplexer component
external_components:
  - source: github://Nick6464/vl6180x-esphome
    components: [ vl6180x ]

sensor:
  # Bin 1 - Multiplexer Channel 0
  - platform: vl6180x
    name: "Bin 1 Water Distance"
    id: bin_1_distance
    address: 0x29
    i2c_id: bus_a
    multiplexer:
      address: 0x70
      channel: 0
    update_interval: 500ms
    filters:
      - median:
          window_size: 5
          send_every: 1

  # Bin 2 - Multiplexer Channel 1
  - platform: vl6180x
    name: "Bin 2 Water Distance"
    id: bin_2_distance
    address: 0x29
    i2c_id: bus_a
    multiplexer:
      address: 0x70
      channel: 1
    update_interval: 500ms
    filters:
      - median:
          window_size: 5
          send_every: 1

  # Repeat for bins 3 and 4...
```

## Calibration

### 1. Measure Empty Distance

For each bin, measure the distance from the sensor to the empty tray bottom in millimeters.

In Home Assistant, set the "Empty Distance" values:
- Bin 1 Empty Distance: e.g., 200mm
- Bin 2 Empty Distance: e.g., 195mm
- Bin 3 Empty Distance: e.g., 205mm
- Bin 4 Empty Distance: e.g., 198mm

### 2. Set Target Depth

Set your desired water depth for each bin:
- Bin 1 Target Depth: e.g., 50mm (for orchids)
- Bin 2 Target Depth: e.g., 30mm (for seedlings)
- Bin 3 Target Depth: e.g., 70mm (for larger plants)
- Bin 4 Target Depth: e.g., 25mm (for succulents)

### 3. Set Safety Timeout

Set "Max Fill Time" for each bin as a safety backup:
- Should be longer than normal fill time
- Prevents overflow if sensor fails
- Default: 15 minutes

## How It Works

### Fill Phase
1. Pump starts in reverse (filling)
2. ToF sensor continuously measures distance to water surface
3. System calculates: `water_depth = empty_distance - current_distance`
4. When `water_depth >= target_depth`, pump stops
5. If timeout is reached before target, pump stops (safety)

### Drain Phase
1. Pump runs forward (draining)
2. Sensor monitors water level
3. When distance approaches empty_distance (within 5mm), pump stops
4. Ensures complete drainage

### Soak Phase
- Time-based (unchanged from original system)
- Configurable 1-480 minutes per bin

## Configuration Settings

### Per-Bin Settings

Each bin has independent configuration:

| Setting | Range | Default | Description |
|---------|-------|---------|-------------|
| Target Depth | 5-150mm | 50mm | Desired water depth |
| Empty Distance | 50-300mm | 200mm | Sensor-to-tray distance |
| Max Fill Time | 1-60 min | 15min | Safety timeout |
| Soak Duration | 1-480 min | 60min | Soak time |
| Cycle Interval | 1-30 days | 5 days | Days between cycles |

### Scheduling (Unchanged)

Two modes available:
1. **Interval Days** - Run every N days at specific hour
2. **Daily Times** - Run at specific hours (comma-separated)

## Troubleshooting

### Sensor Not Detected

Check I2C connection:
```yaml
i2c:
  scan: true  # Shows detected devices in logs
```

Look for address 0x29 (or 0x70 for multiplexer) in the logs.

### Inconsistent Readings

Add filtering:
```yaml
filters:
  - median:
      window_size: 7
      send_every: 1
  - sliding_window_moving_average:
      window_size: 3
```

### Fill Timeout

If fills always timeout:
1. Check pump is running correctly
2. Verify target depth is achievable
3. Increase max fill time
4. Check sensor mounting (should see water surface)

### Overfilling

If water exceeds target:
1. Reduce target depth slightly
2. Check empty distance calibration
3. Verify sensor has clear view (no spray obstruction)

### Pump Doesn't Stop

Emergency stop available: Turn off bin enable switch or press "Clear All Pending Cycles"

## Pin Usage

The system uses these pins for pumps and sensors:

**Pumps:** GPIO23, 22, 21 (Pump 1), GPIO19, 18, 17 (Pump 2), GPIO25, 26, 27 (Pump 3), GPIO32, 33, 16 (Pump 4)

**I2C (ToF Sensors):** GPIO21 (SDA), GPIO22 (SCL)

**Note:** GPIO21 is shared between Pump 1 Drain and I2C SDA. This works because:
- I2C uses open-drain with pull-ups
- Pump drain only activates during drain phase
- ToF sensors primarily used during fill phase

**Available Pins:** GPIO2, GPIO4, GPIO5, GPIO12-15, GPIO34 are available for future expansion.

## Performance Tips

1. **Update Interval** - 500ms provides good responsiveness without sensor wear
2. **Median Filtering** - Removes spurious readings from water surface ripples
3. **Safety Margin** - Set empty distance 5-10mm more than actual to account for sensor angle
4. **Calibration** - Verify empty distance annually or if trays are moved

## Advanced: Multiple I2C Buses

ESP32 supports two I2C buses. For 4 sensors without multiplexer:

```yaml
i2c:
  - id: bus_a
    sda: GPIO21
    scl: GPIO22
    scan: true
  - id: bus_b
    sda: GPIO32
    scl: GPIO33
    scan: true

sensor:
  - platform: vl6180x
    id: bin_1_distance
    i2c_id: bus_a
    address: 0x29
  - platform: vl6180x
    id: bin_2_distance
    i2c_id: bus_b
    address: 0x29
  # Can only have 2 sensors this way without multiplexer
```

**Note:** This conflicts with Pump 4 pins. Choose multiplexer approach instead.

