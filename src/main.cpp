/**
 * ESP32 Multi-Sensor Hub with ESP-NOW + LoRa
 * Version: 2.0.0
 * 
 * Copyright (C) 2025 Michael Garcia, M&E Design
 * Based on original .ino by Geoff Mcintyre of Mr.Industries (https://mr.industries/)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * 
 * A modular, thread-safe sensor data collection and transmission system
 * using FreeRTOS tasks for concurrent operations.
 */

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "GlobalContext.h"
#include "Sensors.h"
#include "LoRaLink.h"
#include "NowLink.h"
#include "Config.h"
#include "Commands.h"
#include "Tasks.h"
#include "EventQueue.h"
#include "Logger.h"

/**
 * System initialization and task creation
 * 
 * Initializes all subsystems in proper order:
 * 1. Global state management
 * 2. Hardware interfaces (sensors, LoRa, ESP-NOW)
 * 3. Configuration management
 * 4. FreeRTOS task creation
 */
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize logging system for structured output and telemetry
  initLogger(LOG_INFO, SINK_SERIAL);
  
  logInfo("ESP32 Multi-Sensor Hub with ESP-NOW + LoRa");
  logInfo("Sensors: Temperature, Humidity, Light, Ultrasonic Distance");
  logSystemEvent("SYSTEM_STARTUP", "Initializing subsystems");

  // Initialize centralized system state management
  initializeGlobalContext();
  
  // Initialize FreeRTOS event queue for inter-task communication
  if (!initEventQueue()) {
    logCritical("Failed to create event queue - system halted");
    return;
  }
  logSystemEvent("EVENT_QUEUE_INIT", "Inter-task communication ready");
  
  // Initialize I2C environmental sensors (TSL2561 light, HTU21D-F temp/humidity)
  initializeSensors();
  
  // Initialize EEPROM for persistent MAC address storage
  initializeConfig();
  
  // Initialize LoRa radio module for wireless data transmission
  if (!initializeLoRa()) {
    logWarn("LoRa initialization failed - sensors will read but no transmission");
    logInfo("Type 'lora' command to retry LoRa initialization");
  }

  // Initialize ESP-NOW peer-to-peer communication from stored configuration
  initializeNowFromEEPROM();
  
  // Display system status and available serial commands
  printStartupInfo();
  
  // Perform initial environmental sensor reading
  readEnvironmentalSensors();
  
  // Create FreeRTOS tasks for concurrent sensor sampling and communication
  createTasks();
  
  logSystemEvent("TASKS_CREATED", "FreeRTOS multitasking system operational");
}

/**
 * Main loop - minimal implementation for FreeRTOS
 * 
 * All system operations are handled by dedicated FreeRTOS tasks.
 * This loop only provides watchdog functionality.
 */
void loop() {
  // Yield to FreeRTOS scheduler - all work done in tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}