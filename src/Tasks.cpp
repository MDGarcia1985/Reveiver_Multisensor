#include "Tasks.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include "Sensors.h"
#include "LoRaLink.h"
#include "NowLink.h"
#include "Commands.h"

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
    // Take mutex before accessing sensor data
    if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY)) {
      readEnvironmentalSensors();
      xSemaphoreGive(sensorDataMutex);
    }
    
    vTaskDelayUntil(&lastWakeTime, frequency);
  }
}

void commsTask(void* parameter) {
  TickType_t lastLoRaTransmit = 0;
  const TickType_t loraInterval = pdMS_TO_TICKS(LORA_TRANSMIT_INTERVAL);
  
  while (true) {
    getGlobalContext().currentTime = millis();
    
    // Handle ESP-NOW messages
    handleNowMessages();
    
    // Send LoRa data periodically
    TickType_t currentTick = xTaskGetTickCount();
    if (getGlobalContext().loraActive && (currentTick - lastLoRaTransmit) >= loraInterval) {
      if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10))) {
        pushAllData("Greenhouse");
        lastLoRaTransmit = currentTick;
        xSemaphoreGive(sensorDataMutex);
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void commandTask(void* parameter) {
  while (true) {
    handleSerialCommands();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}