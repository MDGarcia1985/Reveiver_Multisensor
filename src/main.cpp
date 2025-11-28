/**
 * ESP32 Multi-Sensor Hub with ESP-NOW + LoRa
 * 
 * A modular, thread-safe sensor data collection and transmission system
 * using FreeRTOS tasks for concurrent operations.
 * 
 * Features:
 * - Environmental sensors: Temperature, Humidity, Light
 * - Distance measurement via ESP-NOW communication
 * - LoRa wireless data transmission
 * - Thread-safe sensor data access with mutex protection
 * - Serial command interface for configuration and control
 * 
 * License: Open Source - see LICENSE file
 * Author: [Your Name]
 * Version: 1.0
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

  Serial.println("\n=== ESP32 Multi-Sensor Hub with ESP-NOW + LoRa ===");
  Serial.println("Sensors: Temperature, Humidity, Light, Ultrasonic Distance\n");

  // Initialize centralized state management
  initializeGlobalContext();
  
  // Initialize I2C sensors (TSL2561, HTU21D-F)
  initializeSensors();
  
  // Initialize EEPROM for persistent configuration
  initializeConfig();
  
  // Initialize LoRa radio for data transmission
  if (!initializeLoRa()) {
    Serial.println("⚠️  WARNING: LoRa initialization failed!");
    Serial.println("⚠️  Sensors will read but no LoRa transmission!");
    Serial.println("⚠️  Type 'lora' to retry.\n");
  }

  // Initialize ESP-NOW for peer-to-peer communication
  initializeNowFromEEPROM();
  
  // Display available commands to user
  printStartupInfo();
  
  // Perform initial sensor reading
  readEnvironmentalSensors();
  
  // Create concurrent FreeRTOS tasks for system operation
  createTasks();
  
  Serial.println("FreeRTOS tasks created - system running\n");
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