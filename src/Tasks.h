/**
 * Tasks.h - FreeRTOS task definitions
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