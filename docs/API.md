# API Reference

## Thread-Safe Sensor Data Access

### Individual Sensor Getters

```cpp
bool getSensorTemperature(int* temp);
bool getSensorHumidity(float* humidity);
bool getSensorLux(int* lux);
bool getSensorDistance(float* distance);
```

**Parameters:**
- Pointer to variable to store sensor value

**Returns:**
- `true` if data retrieved successfully
- `false` if mutex timeout or invalid parameter

**Example:**
```cpp
int temperature;
if (getSensorTemperature(&temperature)) {
    Serial.print("Temperature: ");
    Serial.println(temperature);
}
```

### Bulk Data Access

```cpp
bool getAllSensorData(int* temp, float* humidity, int* lux, float* distance);
```

**Description:** Atomically retrieves all sensor values in a single mutex operation.

**Parameters:**
- `temp` - Pointer to temperature variable (°C)
- `humidity` - Pointer to humidity variable (%)
- `lux` - Pointer to light level variable
- `distance` - Pointer to distance variable (inches)

**Example:**
```cpp
int temp, lux;
float humidity, distance;

if (getAllSensorData(&temp, &humidity, &lux, &distance)) {
    // All sensor data is from the same moment in time
    Serial.printf("T:%d H:%.1f L:%d D:%.2f\n", temp, humidity, lux, distance);
}
```

### Sensor Data Updates

```cpp
bool setSensorDistance(float distance);
```

**Description:** Thread-safe update of distance sensor value (typically from ESP-NOW).

**Parameters:**
- `distance` - New distance value in inches

**Returns:**
- `true` if update successful
- `false` if mutex timeout

### Utility Functions

```cpp
bool printSensorDataSafe();
bool copySensorDataSafe(SensorData* dest);
```

**printSensorDataSafe():** Safely prints all sensor values to Serial.

**copySensorDataSafe():** Creates a complete copy of sensor data structure.

## Configuration Management

### EEPROM Functions

```cpp
void saveMacToEEPROM(uint8_t* mac);
bool loadMacFromEEPROM(uint8_t* mac);
bool parseMacAddress(const char* macStr, uint8_t* mac);
void printMacAddress(uint8_t* mac);
```

### Interactive Configuration

```cpp
void configureMacAddress();
```

**Description:** Interactive MAC address configuration with timeout protection.

**Features:**
- Input validation
- Show current MAC
- Clear stored MAC
- 30-second timeout

## Communication Protocols

### LoRa Functions

```cpp
bool initializeLoRa();
void createHub(const char* hubName, const char* sensorNames, const char* types);
void pushAllData(const char* hubName);
```

**initializeLoRa():** Initialize LoRa radio with error handling.

**createHub():** Send hub creation packet with sensor definitions.

**pushAllData():** Transmit all sensor data atomically.

### ESP-NOW Functions

```cpp
bool initializeNowSerial(uint8_t* mac);
void initializeNowFromEEPROM();
bool parseDistance(const char* message, float* distance);
void handleNowMessages();
```

## Serial Commands

### Available Commands

| Command | Description | Example |
|---------|-------------|---------|
| `config` | Configure ESP-NOW peer MAC | `config` |
| `status` | Show system status | `status` |
| `sensors` | Read sensors now | `sensors` |
| `send` | Send LoRa packet | `send Greenhouse` |
| `lora` | Retry LoRa init | `lora` |
| `reset` | Restart device | `reset` |

### Command Processing

```cpp
bool readSerialLine(char *buffer, size_t bufferSize);
void parseCommand(char *line, char *cmd, char *args);
void dispatchCommand(const char *cmd, const char *args);
```

## FreeRTOS Tasks

### Task Creation

```cpp
void createTasks();
```

**Description:** Creates all system tasks with appropriate priorities and stack sizes.

### Task Functions

```cpp
void sensorTask(void* parameter);    // Priority 2, 1Hz sampling
void commsTask(void* parameter);     // Priority 1, communication handling
void commandTask(void* parameter);   // Priority 1, serial commands
```

## Global Context Access

```cpp
GlobalContext& getGlobalContext();
void initializeGlobalContext();
```

**Description:** Access to centralized system state. Direct access should be avoided in favor of thread-safe accessor functions.

## Hardware Abstraction

### Pin Definitions (pins.h)

```cpp
// I2C pins
#define PIN_I2C_SDA    21
#define PIN_I2C_SCL    22

// LoRa pins  
#define PIN_LORA_CS    12
#define PIN_LORA_RST   13
#define PIN_LORA_DIO0  11
```

### Board Selection

```cpp
#define BOARD_ESP32_DEV     // Standard ESP32
// #define BOARD_ESP32_S3   // ESP32-S3 variant
// #define BOARD_CUSTOM     // Custom configuration
```

## Error Handling

### Return Values
- Functions return `bool` for success/failure indication
- Timeout-based mutex operations prevent deadlocks
- Input validation on all public functions

### Error Recovery
- Graceful degradation when components fail
- User-initiated retry mechanisms
- Automatic recovery where possible