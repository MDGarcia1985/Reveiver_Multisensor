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