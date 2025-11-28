/**
 * Sensors.cpp - Environmental sensor interface implementation
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

#include "Sensors.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include "Tasks.h"
#include "Logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cmath>

static Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
static Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void initializeSensors() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.print("I2C initialized - SDA: GPIO");
  Serial.print(PIN_I2C_SDA);
  Serial.print(", SCL: GPIO");
  Serial.println(PIN_I2C_SCL);
  
  logInfo("Initializing I2C environmental sensors");
  
  if (!tsl.begin()) {
    logWarn("TSL2561 light sensor not detected - readings will be zero");
  } else {
    configureTSL2561();
    logInfo("TSL2561 light sensor initialized successfully");
  }

  if (!htu.begin()) {
    logWarn("HTU21D-F temp/humidity sensor not detected - readings will be zero");
  } else {
    logInfo("HTU21D-F temperature/humidity sensor initialized successfully");
  }
}

void configureTSL2561() {
  tsl.setGain(TSL2561_GAIN_16X);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}

void readEnvironmentalSensors() {
  float temp_c = htu.readTemperature();
  float humidity = htu.readHumidity();
  
  GlobalContext& ctx = getGlobalContext();
  
  if (!isnan(temp_c) && temp_c >= -40 && temp_c <= 85) {
    ctx.sensors.temperature = (int)temp_c;
  }
  
  if (!isnan(humidity) && humidity >= 0 && humidity <= 100) {
    ctx.sensors.humidity = humidity;
  }
  
  sensors_event_t event;
  if (tsl.getEvent(&event)) {
    if (event.light > 0 && event.light < 100000) {
      ctx.sensors.lux = (int)event.light;
    }
  }
  
  ctx.sensors.lastEnvironmentalUpdate = millis();
}



void printCurrentSensorValues() {
  printSensorDataSafe();
}