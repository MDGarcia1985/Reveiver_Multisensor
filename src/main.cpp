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

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== ESP32 Multi-Sensor Hub with ESP-NOW + LoRa ===");
  Serial.println("Sensors: Temperature, Humidity, Light, Ultrasonic Distance\n");

  // Initialize global context
  initializeGlobalContext();
  
  // Initialize sensors
  initializeSensors();
  
  // Initialize configuration
  initializeConfig();
  
  // Initialize LoRa
  if (!initializeLoRa()) {
    Serial.println("⚠️  WARNING: LoRa initialization failed!");
    Serial.println("⚠️  Sensors will read but no LoRa transmission!");
    Serial.println("⚠️  Type 'lora' to retry.\n");
  }

  // Initialize ESP-NOW
  initializeNowFromEEPROM();
  
  // Print help
  printStartupInfo();
  
  // Read initial sensor values
  readEnvironmentalSensors();
  
  // Create FreeRTOS tasks
  createTasks();
  
  Serial.println("FreeRTOS tasks created - system running\n");
}

void loop() {
  // FreeRTOS tasks handle all operations
  // Keep loop minimal for watchdog
  vTaskDelay(pdMS_TO_TICKS(1000));
}