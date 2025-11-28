/**
 * Tasks.cpp - FreeRTOS task implementations
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
 */

#include "Tasks.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include "Sensors.h"
#include "LoRaLink.h"
#include "NowLink.h"
#include "Commands.h"
#include "EventQueue.h"

SemaphoreHandle_t sensorDataMutex = NULL;

void createTasks() {
  // Create mutex for sensor data protection
  sensorDataMutex = xSemaphoreCreateMutex();
  
  // Create sensor sampling task
  xTaskCreate(
    sensorTask,
    "SensorTask",
    SENSOR_TASK_STACK,
    NULL,
    SENSOR_TASK_PRIORITY,
    NULL
  );
  
  // Create communications task
  xTaskCreate(
    commsTask,
    "CommsTask", 
    COMMS_TASK_STACK,
    NULL,
    COMMS_TASK_PRIORITY,
    NULL
  );
  
  // Create command handling task
  xTaskCreate(
    commandTask,
    "CommandTask",
    COMMAND_TASK_STACK,
    NULL,
    COMMAND_TASK_PRIORITY,
    NULL
  );
}

void sensorTask(void* parameter) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(ENVIRONMENTAL_SENSOR_INTERVAL);
  
  while (true) {
    // Acquire mutex for thread-safe sensor data access
    if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY)) {
      readEnvironmentalSensors();
      xSemaphoreGive(sensorDataMutex);
      
      // Broadcast sensor data ready event to other tasks
      sendEvent(EVENT_SENSOR_DATA_READY);
    }
    
    // Maintain precise 1Hz sampling rate using absolute timing
    vTaskDelayUntil(&lastWakeTime, frequency);
  }
}

void commsTask(void* parameter) {
  TickType_t lastLoRaTransmit = 0;
  const TickType_t loraInterval = pdMS_TO_TICKS(LORA_TRANSMIT_INTERVAL);
  EventMessage event;
  
  while (true) {
    // Update global timestamp for system timing
    getGlobalContext().currentTime = millis();
    
    // Process incoming ESP-NOW peer messages
    handleNowMessages();
    
    // Process inter-task events with short timeout for responsiveness
    if (receiveEvent(&event, pdMS_TO_TICKS(10))) {
      switch (event.type) {
        case EVENT_SENSOR_DATA_READY:
          // Environmental sensor data updated - available for transmission
          break;
        case EVENT_DISTANCE_UPDATED:
          // Distance measurement received via ESP-NOW communication
          break;
        case EVENT_LORA_SEND_REQUEST:
          // Manual LoRa transmission requested via serial command
          if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10))) {
            pushAllData("Greenhouse");
            sendEvent(EVENT_LORA_SEND_COMPLETE);
            xSemaphoreGive(sensorDataMutex);
          }
          break;
        default:
          break;
      }
    }
    
    // Perform periodic LoRa data transmission
    TickType_t currentTick = xTaskGetTickCount();
    if (getGlobalContext().loraActive && (currentTick - lastLoRaTransmit) >= loraInterval) {
      if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10))) {
        pushAllData("Greenhouse");
        lastLoRaTransmit = currentTick;
        xSemaphoreGive(sensorDataMutex);
      }
    }
    
    // Short delay to prevent task starvation
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void commandTask(void* parameter) {
  while (true) {
    handleSerialCommands();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}