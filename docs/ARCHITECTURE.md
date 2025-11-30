# System Architecture

## Overview

The ESP32 Multi-Sensor Hub uses a modular, thread-safe architecture built on FreeRTOS for concurrent operations and reliable sensor data management.

## Design Principles

### 1. Separation of Concerns
Each module has a single, well-defined responsibility:
- **Sensors**: Hardware interface and data acquisition
- **Communications**: Data transmission (LoRa, ESP-NOW)
- **Configuration**: Persistent settings management
- **Commands**: User interaction interface

### 2. Thread Safety
All shared data access is protected by FreeRTOS mutexes to prevent race conditions in the multi-tasking environment.

### 3. Hardware Abstraction
Pin definitions are centralized in `pins.h` for easy portability across different ESP32 variants.

## Module Dependencies

```
main.cpp
├── GlobalContext (centralized state)
├── Tasks (FreeRTOS task management)
├── EventQueue (inter-task communication)
├── Logger (structured logging and telemetry)
├── Sensors ──┬── pins.h
│             └── SensorDataAccess (thread-safe access)
├── LoRaLink ──┬── pins.h
│              ├── SensorDataAccess
│              └── Logger
├── NowLink ───┬── Config
│              ├── SensorDataAccess
│              ├── EventQueue
│              └── Logger
├── Config ────── NowLink
└── Commands ──┬── All modules (for status/control)
               ├── SensorDataAccess
               └── Logger
```

## Data Flow

### Sensor Data Pipeline
1. **Sensor Task** reads I2C sensors every 1 second
2. Data stored in `GlobalContext` with mutex protection
3. **Communications Task** reads data atomically for transmission
4. **Command Task** provides user access to current readings

### ESP-NOW Distance Updates
1. **Communications Task** receives ESP-NOW messages
2. Distance data parsed and validated
3. Sensor data updated using thread-safe functions
4. Other tasks access updated distance immediately

### LoRa Transmission
1. **Communications Task** triggers periodic transmission
2. Atomic snapshot of all sensor data taken
3. Data formatted and transmitted via LoRa radio
4. Transmission timestamp updated

### Event-Driven Communication
1. **EventQueue** provides FreeRTOS-based inter-task messaging
2. Tasks send events for state changes (sensor updates, distance received)
3. **Communications Task** processes events from queue
4. Events carry optional data payloads for efficient communication

### Structured Logging
1. **Logger** provides multi-level filtering (DEBUG to CRITICAL)
2. Structured telemetry functions for sensor data and system events
3. Multiple output sinks (Serial console, future network/storage)
4. Thread-safe operation across all FreeRTOS tasks

## Task Architecture

### Task Priorities
- **Sensor Task**: Priority 2 (highest) - ensures consistent sampling
- **Communications Task**: Priority 1 - handles real-time communication
- **Command Task**: Priority 1 - user interaction

### Task Synchronization
- **Mutex Protection**: All sensor data access uses `sensorDataMutex`
- **Timeout Handling**: 100ms timeout prevents deadlocks
- **Atomic Operations**: Bulk data operations are atomic
- **Event Queue**: FreeRTOS queue for inter-task communication
- **Logging Coordination**: Thread-safe logging across all tasks

## Memory Management

### Static Allocation
- Global context uses static allocation for predictable memory usage
- Task stacks sized appropriately for each task's requirements
- No dynamic memory allocation in critical paths

### Buffer Management
- Fixed-size buffers for ESP-NOW message reception
- Overflow protection with bounds checking
- Circular buffer behavior for message parsing

## Error Handling Strategy

### Graceful Degradation
- System continues operation if individual components fail
- LoRa failure doesn't affect sensor sampling
- Sensor failures don't affect communication

### Recovery Mechanisms
- Automatic retry for transient failures
- User-initiated retry commands for persistent issues
- Watchdog protection via minimal main loop

## Extensibility

### Adding New Sensors
1. Extend `SensorData` structure
2. Add initialization in `Sensors.cpp`
3. Create thread-safe access functions
4. Update display and transmission functions

### Adding New Communication Protocols
1. Create new module following existing patterns
2. Add task in `Tasks.cpp` if needed
3. Use existing thread-safe sensor access
4. Update command interface for control

## Performance Characteristics

### Timing Requirements
- **Sensor Sampling**: 1Hz (1000ms intervals)
- **LoRa Transmission**: 1Hz (configurable)
- **ESP-NOW Processing**: Real-time (10ms task cycle)
- **Command Processing**: 50ms response time

### Resource Usage
- **CPU**: Efficient task scheduling with appropriate delays
- **Memory**: Static allocation, ~8KB total usage
- **Power**: Sleep modes between operations where possible