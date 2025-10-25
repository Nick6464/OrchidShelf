# Quick Start: Water Depth Control

Fast setup guide for VL6180X sensor-based flood irrigation control.

**How It Works:** The VL6180X ToF sensors measure the distance to the water surface. When filling, the system monitors this distance and stops the pump when the target depth is reached. No time-based durations needed - the sensors control everything automatically.

## What You Need

**Hardware:**
- 4x VL6180X Time-of-Flight sensors ([example link](https://www.adafruit.com/product/3316))
- 1x TCA9548A I2C Multiplexer ([example link](https://www.adafruit.com/product/2717))
- Jumper wires
- Mounting brackets (3D printed or DIY)

**Time Required:** 1-2 hours for complete setup

## Step 1: Hardware Installation (30 min)

### Wire the I2C Multiplexer

```
TCA9548A → ESP32
VCC      → 3.3V
GND      → GND  
SDA      → GPIO21
SCL      → GPIO22
```

### Connect VL6180X Sensors

Each sensor connects to one multiplexer channel:

```
Bin 1 Sensor → Channel 0
Bin 2 Sensor → Channel 1
Bin 3 Sensor → Channel 2
Bin 4 Sensor → Channel 3

Each sensor:
VCC → 3.3V
GND → GND
SDA → Channel SDA
SCL → Channel SCL
```

### Mount Sensors

Position each sensor:
- 150-250mm above empty tray bottom
- Pointing straight down
- Clear view of water surface
- Secure so it won't move

## Step 2: Update Configuration (10 min)

### Update floodshelf.yaml

Add multiplexer configuration after the bin_1_distance sensor definition:

```yaml
sensor:
  # Bin 1 - Multiplexer Channel 0
  - platform: vl6180x
    name: "Bin 1 Water Distance"
    id: bin_1_distance
    address: 0x29
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
    multiplexer:
      address: 0x70
      channel: 1
    update_interval: 500ms
    filters:
      - median:
          window_size: 5
          send_every: 1

  # Bin 3 - Multiplexer Channel 2
  - platform: vl6180x
    name: "Bin 3 Water Distance"
    id: bin_3_distance
    address: 0x29
    multiplexer:
      address: 0x70
      channel: 2
    update_interval: 500ms
    filters:
      - median:
          window_size: 5
          send_every: 1

  # Bin 4 - Multiplexer Channel 3
  - platform: vl6180x
    name: "Bin 4 Water Distance"
    id: bin_4_distance
    address: 0x29
    multiplexer:
      address: 0x70
      channel: 3
    update_interval: 500ms
    filters:
      - median:
          window_size: 5
          send_every: 1
```

Also update the flood cycle scripts to use the correct sensor IDs. Replace the placeholder references in pump_2_flood_cycle, pump_3_flood_cycle, and pump_4_flood_cycle:

```yaml
# In pump_2_flood_cycle, change:
float current = calculate_water_depth(2, id(bin_1_distance).state);
# To:
float current = calculate_water_depth(2, id(bin_2_distance).state);

# In pump_3_flood_cycle, change:
float current = calculate_water_depth(3, id(bin_1_distance).state);
# To:
float current = calculate_water_depth(3, id(bin_3_distance).state);

# In pump_4_flood_cycle, change:
float current = calculate_water_depth(4, id(bin_1_distance).state);
# To:
float current = calculate_water_depth(4, id(bin_4_distance).state);

# Also update drain sections similarly
```

## Step 3: Flash Firmware (5 min)

```bash
esphome run floodshelf.yaml
```

Select your ESP32 device and wait for upload to complete.

## Step 4: Verify Sensors (5 min)

In Home Assistant, check that these sensors appear:
- Bin 1 Water Distance
- Bin 2 Water Distance
- Bin 3 Water Distance
- Bin 4 Water Distance

Each should show distance readings in millimeters. Wave your hand under a sensor to verify it responds.

## Step 5: Calibrate Empty Distance (10 min)

For each bin:

1. **Ensure tray is completely empty and dry**
2. **Check sensor reading in Home Assistant**
3. **Note the distance value** (e.g., 203mm)
4. **Set "Bin X Empty Distance"** to this value
5. **Add 5mm safety margin** (e.g., set to 208mm)

Repeat for all 4 bins.

Example:
```
Bin 1: Reading 203mm → Set Empty Distance: 208mm
Bin 2: Reading 198mm → Set Empty Distance: 203mm
Bin 3: Reading 205mm → Set Empty Distance: 210mm
Bin 4: Reading 201mm → Set Empty Distance: 206mm
```

## Step 6: Set Target Depths (5 min)

Choose conservative starting depths:

```
Bin 1 Target Depth: 30mm
Bin 2 Target Depth: 30mm
Bin 3 Target Depth: 30mm
Bin 4 Target Depth: 30mm
```

You can increase these later based on your plants' needs.

## Step 7: Set Safety Timeouts (5 min)

Set reasonable max fill times as safety backup:

```
Bin 1 Max Fill Time: 15 minutes
Bin 2 Max Fill Time: 15 minutes
Bin 3 Max Fill Time: 15 minutes
Bin 4 Max Fill Time: 15 minutes
```

## Step 8: Configure Soak Times (5 min)

Set how long water sits in each tray:

```
Bin 1 Soak Duration: 60 minutes
Bin 2 Soak Duration: 60 minutes
Bin 3 Soak Duration: 60 minutes
Bin 4 Soak Duration: 60 minutes
```

## Step 9: Test First Cycle (20 min)

Run a manual test cycle:

1. **Press "Start Bin 1 Cycle"** in Home Assistant
2. **Watch the sensor reading decrease** as water fills
3. **Verify pump stops** when target depth reached
4. **Wait through soak period**
5. **Watch sensor reading increase** during drain
6. **Verify complete drainage**

Monitor the "Bin 1 Status" sensor to see:
- "Filling" → "Soaking" → "Draining" → "Idle"

If anything seems wrong, press the bin enable switch to stop.

## Step 10: Fine-Tune Depths (10 min)

After test cycle completes:

1. **Check if water depth was appropriate** for your plants
2. **Adjust target depths** up or down as needed
3. **Test another cycle** if you made significant changes

Typical depths:
- Orchids: 40-60mm
- Seedlings: 20-40mm
- Large plants: 60-80mm
- Succulents: 20-30mm

## Step 11: Set Schedule (5 min)

Configure when each bin should water:

**Option 1: Interval Days Mode**
```
Bin 1: Every 5 days at 9:00 AM
Bin 2: Every 3 days at 10:00 AM
Bin 3: Every 7 days at 11:00 AM
Bin 4: Every 10 days at 12:00 PM
```

**Option 2: Daily Times Mode**
```
Bin 1: Daily at 9,18 (9 AM and 6 PM)
Bin 2: Daily at 10
Bin 3: Daily at 8,20
Bin 4: Daily at 12
```

## Troubleshooting

### Sensors Not Detected

Check I2C connection:
```yaml
i2c:
  scan: true
```

Look in ESPHome logs for:
```
Found i2c device at address 0x70 (TCA9548A)
Found i2c device at address 0x29 (VL6180X)
```

### Fill Timeout Every Cycle

- Check pump is running (reverse switch on)
- Verify target depth is realistic (start at 30mm)
- Increase max fill time
- Check sensor can see water surface

### Readings Jump Around

Add more filtering:
```yaml
filters:
  - median:
      window_size: 7
      send_every: 1
  - sliding_window_moving_average:
      window_size: 5
```

### Pump Won't Stop Filling

- Emergency: Turn off bin enable switch
- Check sensor wiring
- Verify sensor ID matches in script
- Check empty distance calibration

### Water Depth Inconsistent

- Remeasure empty distance
- Ensure sensor is mounted firmly
- Check for water spray hitting sensor
- Verify sensor pointing straight down

## Monitoring

Create a card in Home Assistant to monitor:
- Current water distance for each bin
- Bin status (Idle/Filling/Soaking/Draining)
- Target depth settings
- Next cycle countdown

## Next Steps

Once system is running smoothly:

1. **Optimize depths** for each plant type
2. **Adjust soak times** based on media drainage
3. **Fine-tune schedules** for your climate
4. **Monitor plant response** and adjust as needed

## Support

See full documentation:
- `docs/DEPTH_CONTROL_GUIDE.md` - Complete setup details  
- `docs/TOF_WIRING_GUIDE.md` - Sensor wiring information
- `README.md` - Main project documentation

## Quick Reference Card

**Typical Settings:**

| Parameter | Orchids | Seedlings | Large Plants | Succulents |
|-----------|---------|-----------|--------------|------------|
| Target Depth | 50mm | 30mm | 70mm | 25mm |
| Soak Time | 2 hours | 30 min | 6 hours | 45 min |
| Interval | 5 days | 2 days | 7 days | 14 days |

**Safety Limits:**
- Min depth: 5mm
- Max depth: 150mm (practical limit ~100mm)
- Min soak: 1 minute
- Max soak: 8 hours (480 minutes)
- Max fill timeout: 60 minutes

**Calibration Check:**
Empty distance = Sensor reading when tray is empty + 5mm safety margin

That's it! Your depth-based flood irrigation system is now ready to provide precise, consistent watering for your plants.

