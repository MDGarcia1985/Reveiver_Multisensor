#include "SensorDataAccess.h"
#include "Tasks.h"

#define MUTEX_TIMEOUT_MS 100

bool getSensorTemperature(int* temp) {
  if (!temp) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *temp = getGlobalContext().sensors.temperature;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool getSensorHumidity(float* humidity) {
  if (!humidity) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *humidity = getGlobalContext().sensors.humidity;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool getSensorLux(int* lux) {
  if (!lux) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *lux = getGlobalContext().sensors.lux;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool getSensorDistance(float* distance) {
  if (!distance) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *distance = getGlobalContext().sensors.distance;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool getAllSensorData(int* temp, float* humidity, int* lux, float* distance) {
  if (!temp || !humidity || !lux || !distance) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    GlobalContext& ctx = getGlobalContext();
    *temp = ctx.sensors.temperature;
    *humidity = ctx.sensors.humidity;
    *lux = ctx.sensors.lux;
    *distance = ctx.sensors.distance;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool setSensorDistance(float distance) {
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    getGlobalContext().sensors.distance = distance;
    getGlobalContext().sensors.lastDistanceUpdate = millis();
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool updateSensorTimestamp(unsigned long* lastUpdate) {
  if (!lastUpdate) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *lastUpdate = millis();
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool copySensorDataSafe(SensorData* dest) {
  if (!dest) return false;
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    *dest = getGlobalContext().sensors;
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  return false;
}

bool printSensorDataSafe() {
  if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
    GlobalContext& ctx = getGlobalContext();
    Serial.println("=== Current Sensor Values ===");
    Serial.print("Temperature: ");
    Serial.print(ctx.sensors.temperature);
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(ctx.sensors.humidity, 1);
    Serial.println("%");
    Serial.print("Lux: ");
    Serial.println(ctx.sensors.lux);
    Serial.print("Distance: ");
    Serial.print(ctx.sensors.distance, 2);
    Serial.println(" in");
    Serial.println("============================\n");
    xSemaphoreGive(sensorDataMutex);
    return true;
  }
  Serial.println("Failed to access sensor data");
  return false;
}