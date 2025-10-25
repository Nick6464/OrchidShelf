# Time-of-Flight Laser Sensor Wiring Guide

## Sensor Model Identification

Most ToF laser sensors use I2C communication. Common models:
- **VL53L0X** - Up to 2m range, very popular
- **VL53L1X** - Up to 4m range, newer version
- **VL53L4CD** - Up to 1.3m range, lower cost

Check your sensor board for markings to identify which model you have.

## Wiring Connections

### Standard I2C ToF Sensor Pinout:
```
VL53L0X/VL53L1X Sensor
┌─────────────────┐
│  VIN (3.3-5V)   │ ──→ ESP32 3.3V
│  GND            │ ──→ ESP32 GND
│  SCL (Clock)    │ ──→ ESP32 GPIO22
│  SDA (Data)     │ ──→ ESP32 GPIO21
│  GPIO1 (XSHUT)  │ ──→ Optional: GPIO pin for shutdown control (leave unconnected for basic setup)
└─────────────────┘
```

### Recommended Pin Assignment for Your ESP32:

**Option 1: Using Default I2C Pins (RECOMMENDED)**
- **VCC** → ESP32 3.3V pin
- **GND** → ESP32 GND pin
- **SDA** → **GPIO21** (Default I2C Data)
- **SCL** → **GPIO22** (Default I2C Clock)

⚠️ **Note**: GPIO21 and GPIO22 are currently used for Pump 1 control in your main system. For standalone testing, use these pins. For integration into your main system, see Option 2 below.

**Option 2: Alternative I2C Pins (for integration with pump system)**
If you want to add the ToF sensor to your existing system without removing pumps:
- **VCC** → ESP32 3.3V pin
- **GND** → ESP32 GND pin
- **SDA** → **GPIO13** (currently used for Bin 3 Trigger - can be relocated)
- **SCL** → **GPIO15** (currently used for Bin 4 Trigger - can be relocated)

**Option 3: Software I2C (if all hardware I2C pins are taken)**
- **SDA** → Any available GPIO (e.g., GPIO35, but note it's input-only)
- **SCL** → Any available GPIO (e.g., GPIO0, but be careful during boot)

## Physical Setup for Testing

1. **Power Connections**
   - Connect VCC to 3.3V (NOT 5V if your board is 3.3V only - check the sensor specs)
   - Connect GND to GND
   - Some boards have a 5V regulator and can accept 5V input

2. **I2C Bus Connections**
   - Connect SDA to GPIO21
   - Connect SCL to GPIO22
   - Make sure connections are secure

3. **Optional Pull-up Resistors**
   - Most ToF sensor breakout boards have built-in pull-up resistors (typically 4.7kΩ)
   - Check your board - if it doesn't have resistors, you'll need to add 4.7kΩ resistors between SDA→3.3V and SCL→3.3V

## Testing Procedure

1. **Flash the test configuration:**
   ```bash
   esphome run tof_test.yaml
   ```

2. **Check the I2C scan results in the logs:**
   - You should see "Found i2c device at address 0x29"
   - If not found, check your wiring

3. **Monitor distance readings:**
   - The sensor will output distance measurements every second
   - Watch for stable readings when pointing at objects
   - Valid range depends on your sensor model:
     - VL53L0X: 30mm to 2000mm
     - VL53L1X: 40mm to 4000mm

4. **Troubleshooting:**
   - **"No device found"**: Check wiring, try different I2C pins
   - **Readings stuck at 0 or 8190**: Check sensor orientation, ensure clear line of sight
   - **Erratic readings**: Add/check pull-up resistors, reduce update_interval

## Integration with Main System

Once testing confirms the sensor works, you have two options:

### Option A: Add to existing floodshelfheight.yaml
Add the I2C and sensor configuration to your existing file. This requires moving some pump control pins.

### Option B: Keep as separate ESP32
Use a second ESP32 dedicated to the ToF sensor. This is simpler and keeps systems independent.

## Pin Conflict Resolution

Your current system uses:
- GPIO21: Pump 1 Drain
- GPIO22: Pump 1 Fill

To add ToF sensor to the main system, you could:
1. Move Pump 1 controls to other available pins
2. Use software I2C on different GPIO pins
3. Use a second ESP32 for the ToF sensor

## Available GPIO Pins on Your Current System

Based on your README, these pins appear unused:
- GPIO0 (boot sensitive, use with caution)
- GPIO35 (input only)

You could use GPIO35 (SDA) and GPIO0 (SCL) with software I2C, but this is less reliable than hardware I2C.

## What is the ToF Sensor For?

Given your flood irrigation system, you likely want to use the ToF sensor to:
- Replace ultrasonic sensors with more accurate distance measurement
- Measure water level in bins with better precision
- Add to reservoir to monitor water supply level

The ToF laser sensor is more accurate than ultrasonic in wet/humid environments and works well with transparent water surfaces.

