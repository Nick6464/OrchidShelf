# Orchid Shelf - Automated Flood Irrigation System

A Home Assistant-controlled flood irrigation system designed for orchids and other plants that benefit from flood-and-drain watering cycles.

## What It Does

This system automatically waters your plants using a flood irrigation method. It fills each tray with water, lets the plants soak, then drains everything back to a central reservoir. Water is reused and cycles run on programmed schedules.

## How It Works

The system consists of 4 independent zones each with its own pump and flood tray, all connected to a central water reservoir. An ESP32 controller running ESPHome manages everything through Home Assistant. The pumps are reversible - they run forward to fill trays and reverse to drain them back to the reservoir.

Each zone cycles through four phases: Fill (pump pushes water to tray), Soak (water sits while plants absorb it), Drain (pump reverses to pull water back), and Wait (dry period until next cycle). Since all zones operate independently, you can run different schedules for different plant types simultaneously.

## Why Use Flood Irrigation?

Flood irrigation mimics the natural wet/dry cycles that orchids and many other plants experience in their native environments. Unlike traditional watering methods that can leave chunky growing media like bark and perlite only partially saturated, flooding ensures every piece gets thoroughly soaked. Once configured, the system operates autonomously for extended periods. Water returns to the central reservoir for reuse, though the reservoir requires periodic manual topping up as water evaporates.

## Configuration

Each pump zone has its own independent settings: Fill time (1-180 minutes), Soak time (5 minutes to 3 days), Drain time (1-180 minutes), and Cycle interval (6 hours to 31 days). These parameters can be adjusted to match different plant requirements.

Example schedules:

| Plant Type | Fill | Soak | Drain | Wait |
|------------|------|------|-------|------|
| Orchids* | 15 min | 2 hours | 10 min | 5 days |
| Seedlings | 5 min | 30 min | 5 min | 2 days |
| Large plants | 30 min | 6 hours | 20 min | 7 days |
| Arid succulents | 10 min | 45 min | 8 min | 14 days |

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
- Time until next cycle
- Manual cycle triggers
- Quick preset adjustments
- Enable/disable controls per zone

## Hardware

The system is built around an ESP32 development board that controls 4 peristaltic pumps through L298N breakout driver boards (which enable the critical pump reversal feature). Each zone needs its own flood tray, and everything connects to a central water reservoir with standard tubing and fittings.

## Future Plans

- Dual-reservoir system with water refiltering to manage salt concentration as water evaporates
- Ultrasonic level sensors to stop filling when trays are full
- Weather integration to adjust watering based on season or humidity
- Water quality monitoring (TDS/pH)
- Automated Fertigation by bin/zone
- More flexible cron-based scheduling for watering cycles
- Proper ESP32 schematics and PCB design to replace the current perf board prototype
