# ESP32 Multi-Sensor Hub

A modular, thread-safe sensor data collection and transmission system using FreeRTOS tasks for concurrent operations.

## Features

- **Environmental Monitoring**: Temperature, humidity, and light level sensing
- **Distance Measurement**: Ultrasonic distance data via ESP-NOW peer communication
- **Wireless Transmission**: LoRa radio for long-range data transmission
- **Thread-Safe Design**: FreeRTOS tasks with mutex-protected sensor data access
- **Interactive Control**: Serial command interface for configuration and monitoring
- **Hardware Portability**: Clean pin abstraction for different ESP32 variants

## Hardware Requirements

### Core Components
- ESP32 development board (ESP32 DevKit or ESP32-S3)
- LoRa module (SX1276/SX1278 compatible)
- TSL2561 light sensor
- HTU21D-F temperature/humidity sensor

### Pin Connections

#### ESP32 DevKit (Default)
```
I2C Sensors:
- SDA: GPIO 21
- SCL: GPIO 22

LoRa Module:
- SCK:  GPIO 18
- MISO: GPIO 19  
- MOSI: GPIO 23
- CS:   GPIO 12
- RST:  GPIO 13
- DIO0: GPIO 11
```

See `src/pins.h` for other board configurations.

## Software Architecture

### Modular Design
```
main.cpp           - System initialization and FreeRTOS task creation
├── GlobalContext  - Centralized state management
├── Sensors        - Environmental sensor interface
├── LoRaLink       - LoRa radio communication
├── NowLink        - ESP-NOW peer communication  
├── Config         - EEPROM configuration management
├── Commands       - Serial command interface
├── Tasks          - FreeRTOS task definitions
└── pins.h         - Hardware pin abstraction
```

### FreeRTOS Tasks
- **Sensor Task** (Priority 2): 1Hz environmental sensor sampling
- **Communications Task** (Priority 1): ESP-NOW message handling and LoRa transmission
- **Command Task** (Priority 1): Serial command processing

## Installation

### Prerequisites
- PlatformIO IDE or Arduino IDE with ESP32 support
- Required libraries (see `platformio.ini`)

### Library Dependencies
```ini
lib_deps = 
    adafruit/Adafruit Unified Sensor
    adafruit/Adafruit TSL2561
    adafruit/Adafruit HTU21DF Library
    sandeepmistry/LoRa
    ESP32_NOW_Serial  ; ESP-NOW communication library
```

### Build and Upload
```bash
# Using PlatformIO
pio run --target upload

# Using Arduino IDE
# Open main.cpp and upload to ESP32 board
```

## Configuration

### Board Selection
Edit `src/pins.h` to select your board:
```cpp
#define BOARD_ESP32_DEV     // Standard ESP32 DevKit
// #define BOARD_ESP32_S3   // ESP32-S3 variant
// #define BOARD_CUSTOM     // Custom pin configuration
```

### ESP-NOW Peer Setup
1. Connect to serial monitor (115200 baud)
2. Type `config` to enter MAC address configuration
3. Enter peer device MAC address in format: `AA:BB:CC:DD:EE:FF`
4. Configuration is saved to EEPROM

## Usage

### Serial Commands
- `config` - Configure ESP-NOW peer MAC address
- `status` - Show system status and sensor readings
- `sensors` - Read sensors immediately
- `send [hubName]` - Send LoRa packet (default hub: "Greenhouse")
- `lora` - Retry LoRa initialization
- `reset` - Restart device

### Data Format

#### LoRa Hub Creation
```
CH>Greenhouse:Temperature,Humidity,Lux,Distance:1,2,1,2
```

#### LoRa Data Transmission
```
PD>Greenhouse:25,65.5,1200,45.67,
```

#### ESP-NOW Distance Messages
```
DIST:45.67
```

## Thread Safety

All sensor data access is protected by FreeRTOS mutexes:

```cpp
// Safe sensor data access
int temp;
float humidity, distance;
int lux;

if (getAllSensorData(&temp, &humidity, &lux, &distance)) {
    // Use sensor data safely
}
```

## Extending the System

### Adding New Sensors
1. Add sensor initialization to `Sensors.cpp`
2. Update `SensorData` struct in `GlobalContext.h`
3. Add thread-safe access functions in `SensorDataAccess.cpp`

### Adding New Communication Protocols
1. Create new module (e.g., `WiFiLink.cpp`)
2. Add task in `Tasks.cpp`
3. Update `main.cpp` initialization

## Troubleshooting

### Common Issues
- **LoRa initialization fails**: Check wiring and pin definitions
- **Sensor readings are zero**: Verify I2C connections and sensor power
- **ESP-NOW not connecting**: Ensure correct peer MAC address configuration

### Debug Output
Enable verbose output by connecting to serial monitor at 115200 baud.

## License

This project is open source. See LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes following the existing code style
4. Add appropriate documentation
5. Submit a pull request

## Version History

- **v1.0** - Initial release with FreeRTOS tasks and thread-safe sensor access