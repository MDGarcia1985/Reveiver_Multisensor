/**
 * SensorDataAccess.h - Thread-safe sensor data access interface
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
#include "GlobalContext.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Safe sensor data access functions
bool getSensorTemperature(int* temp);
bool getSensorHumidity(float* humidity);
bool getSensorLux(int* lux);
bool getSensorDistance(float* distance);
bool getAllSensorData(int* temp, float* humidity, int* lux, float* distance);

bool setSensorDistance(float distance);
bool updateSensorTimestamp(unsigned long* lastUpdate);

// Bulk operations
bool copySensorDataSafe(SensorData* dest);
bool printSensorDataSafe();