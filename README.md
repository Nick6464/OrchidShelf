# Orchid Shelf - Automated Flood Irrigation System

A Home Assistant-controlled flood irrigation system designed for orchids and other plants that benefit from flood-and-drain watering cycles.

## What It Does

This system automatically waters your plants using a flood irrigation method. It fills each tray with water, lets the plants soak, then drains everything back to a central reservoir. Water is reused and cycles run on cron-based schedules with intelligent queue management to prevent conflicts.

## How It Works

The system consists of 4 independent zones each with its own pump and flood tray, all connected to a central water reservoir. An ESP32 controller running ESPHome manages everything through Home Assistant. The pumps are reversible - they run forward to fill trays and reverse to drain them back to the reservoir.

Each zone cycles through four phases: Fill (pump pushes water to tray), Soak (water sits while plants absorb it), Drain (pump reverses to pull water back), and Wait (dry period until next cycle). A queue system ensures only one zone operates at a time, queuing any conflicting schedules and processing them in order once the active cycle completes.

## Why Use Flood Irrigation?

Flood irrigation mimics the natural wet/dry cycles that orchids and many other plants experience in their native environments. Unlike traditional watering methods that can leave chunky growing media like bark and perlite only partially saturated, flooding ensures every piece gets thoroughly soaked. Once configured, the system operates autonomously for extended periods. Water returns to the central reservoir for reuse, though the reservoir requires periodic manual topping up as water evaporates.

## Configuration

Each pump zone has independent cron-style scheduling and cycle parameters. Set a daily time (like 9:00 AM) and interval (like every 5 days), then configure the cycle phases: Fill time (1-60 minutes), Soak time (1-120 minutes), and Drain time (1-60 minutes). The system automatically calculates when each zone's next watering is due and displays human-readable countdowns like "4d 21h 15m".

Example schedules:

| Plant Type | Schedule | Interval | Fill | Soak | Drain |
|------------|----------|----------|------|------|-------|
| Orchids* | 9:00 AM | 5 days | 15 min | 2 hours | 10 min |
| Seedlings | 10:00 AM | 2 days | 5 min | 30 min | 5 min |
| Large plants | 11:00 AM | 7 days | 30 min | 6 hours | 20 min |
| Arid succulents | 12:00 PM | 14 days | 10 min | 45 min | 8 min |

*\* Yes, I know your Cymbidiums need different water than your Promenaea and your Sarcochilus. Just pretend they don't for a minute, and please send me pics of your orchid setups.*

## Project Files

The system consists of several configuration files that work together:

- **`floodshelf.yaml`** - Main ESPHome configuration for the ESP32 controller. Contains all pump logic, timing controls, and sensor definitions. Flash this to your ESP32 device.

- **`configuration.yaml`** - Home Assistant main config that includes all the other components. Add the contents to your existing HA configuration or use as-is for a dedicated setup.

- **`dashboard.yaml`** - Complete Home Assistant dashboard with controls for all 4 zones, scheduling, and monitoring. Import this as a new dashboard in HA. Note: Entity IDs are sprinkled throughout and will likely need updating to match your actual device names.

- **`input_boolean.yaml`** - Scheduler enable/disable switches for each pump zone. Required for the automated scheduling to work.

- **`template.yaml`** - Optional template sensors that provide enhanced system status summaries in the dashboard.

- **`secrets.yaml`** - WiFi credentials for the ESP32. Update with your network details before flashing.

## Home Assistant Integration

The system shows up in Home Assistant with:
- Real-time status for each zone (Idle/Filling/Soaking/Draining)
- Human-readable countdown timers showing exactly when next watering is due ("4d 21h 15m")  
- Cron-style scheduling controls (set daily time and cycle intervals)
- Live queue viewer showing which zones are pending and waiting
- Manual cycle triggers and emergency queue clearing
- Individual zone enable/disable controls
- Separate fill and drain pump speed settings per zone

## Hardware

The system is built around an ESP32 development board that controls 4 peristaltic pumps through L298N breakout driver boards (which enable the critical pump reversal feature). Each zone needs its own flood tray, and everything connects to a central water reservoir with standard tubing and fittings.

### ESP32 Pin Layout

```
        EN - Not used |            | GPIO23 - Pump 1 Speed
        VP - Not used |            | GPIO22 - Pump 1 Fill
        VN - Not used |            | TX0 - Not used
    GPIO34 - Not used |            | RX0 - Not used
GPIO32 - Pump 4 Speed |            | GPIO21 - Pump 1 Drain
 GPIO33 - Pump 4 Fill |            | GPIO19 - Pump 2 Speed
GPIO25 - Pump 3 Speed |            | GPIO18 - Pump 2 Fill
 GPIO26 - Pump 3 Fill |            | GPIO5 - Not used
GPIO27 - Pump 3 Drain |            | TX2 - Not used
    GPIO14 - Not used |            | RX2 - Not used
    GPIO12 - Not used |            | GPIO4 - Not used
    GPIO13 - Not used |            | GPIO2 - Not used
       GND - Not used |            | GPIO15 - Not used
          VIN - Power |            | GND - Not used
```

**Pin Assignment Summary:**
- **Pump 1 (Motor A)**: Speed=GPIO23, Fill=GPIO22, Drain=GPIO21
- **Pump 2 (Motor B)**: Speed=GPIO19, Fill=GPIO18, Drain=GPIO17  
- **Pump 3 (Motor C)**: Speed=GPIO25, Fill=GPIO26, Drain=GPIO27
- **Pump 4 (Motor D)**: Speed=GPIO32, Fill=GPIO33, Drain=GPIO16

The configuration uses two HW-095 motor driver boards, with each board controlling two pumps (motors A & B on board 1, motors C & D on board 2).

## Future Plans

- Dual-reservoir system with water refiltering to manage salt concentration as water evaporates
- Ultrasonic level sensors to stop filling when trays are full
- Weather integration to adjust watering based on season or humidity
- Water quality monitoring (TDS/pH)
- Automated fertigation by bin/zone
- Advanced queue priority and override systems
- Proper ESP32 schematics and PCB design to replace the current perf board prototype
