#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Task priorities
#define SENSOR_TASK_PRIORITY 2
#define COMMS_TASK_PRIORITY 1
#define COMMAND_TASK_PRIORITY 1

// Task stack sizes
#define SENSOR_TASK_STACK 2048
#define COMMS_TASK_STACK 4096
#define COMMAND_TASK_STACK 2048

void createTasks();
void sensorTask(void* parameter);
void commsTask(void* parameter);
void commandTask(void* parameter);

extern SemaphoreHandle_t sensorDataMutex;