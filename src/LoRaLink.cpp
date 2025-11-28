/**
 * LoRaLink.cpp - LoRa radio communication implementation
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

#include "LoRaLink.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include "Logger.h"
#include <cmath>

bool initializeLoRa() {
  Serial.println("\n========== Initializing LoRa ==========");
  Serial.println("Pin Configuration:");
  Serial.print("  SCK:  GPIO "); Serial.println(PIN_LORA_SCK);
  Serial.print("  MISO: GPIO "); Serial.println(PIN_LORA_MISO);
  Serial.print("  MOSI: GPIO "); Serial.println(PIN_LORA_MOSI);
  Serial.print("  CS:   GPIO "); Serial.println(PIN_LORA_CS);
  Serial.print("  RST:  GPIO "); Serial.println(PIN_LORA_RST);
  Serial.print("  DIO0: GPIO "); Serial.println(PIN_LORA_DIO0);
  Serial.println();
  
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  
  Serial.println("Attempting LoRa.begin(915E6)...");
  if (!LoRa.begin(915E6)) {
    logError("LoRa initialization failed at 915MHz");
    logInfo("Check: wiring, pin definitions, 3.3V power, antenna connection");
    return false;
  }
  
  logNetworkEvent("LoRa", "INITIALIZED", "915MHz ready for transmission");
  getGlobalContext().loraActive = true;
  
  logInfo("Creating LoRa sensor hub configuration");
  delay(100);
  
  createHub("Greenhouse", "Temperature,Humidity,Lux,Distance", "1,2,1,2");
  return true;
}

void createHub(const char* hubName, const char* sensorNames, const char* types) {
  if (!getGlobalContext().loraActive) {
    Serial.println("ERROR: Cannot create hub - LoRa not active!");
    return;
  }
  
  if (!hubName || !sensorNames || !types) {
    Serial.println("ERROR: Invalid parameters for hub creation");
    return;
  }
  
  if (!LoRa.beginPacket()) {
    Serial.println("ERROR: Failed to begin LoRa packet");
    return;
  }
  
  LoRa.print("    ");
  LoRa.print("CH");
  LoRa.print(">");
  LoRa.print(hubName);
  LoRa.print(":");
  LoRa.print(sensorNames);
  LoRa.print(":");
  LoRa.print(types);
  
  if (!LoRa.endPacket()) {
    Serial.println("ERROR: Failed to send LoRa packet");
    return;
  }
  
  Serial.print("LoRa: Hub created - ");
  Serial.println(hubName);
  Serial.print("  Sensors: ");
  Serial.println(sensorNames);
  Serial.print("  Types: ");
  Serial.println(types);
}

void pushAllData(const char* hubName) {
  if (!getGlobalContext().loraActive) {
    Serial.println("ERROR: Cannot send data - LoRa not active!");
    return;
  }
  
  if (!hubName) {
    Serial.println("ERROR: Invalid hub name");
    return;
  }
  
  int temp;
  float humidity, distance;
  int lux;
  
  if (!getAllSensorData(&temp, &humidity, &lux, &distance)) {
    Serial.println("ERROR: Failed to read sensor data");
    return;
  }
  
  // Log structured sensor telemetry data
  logSensorData(temp, humidity, lux, distance);
  
  if (!LoRa.beginPacket()) {
    Serial.println("ERROR: Failed to begin LoRa packet");
    return;
  }
  
  LoRa.print("    ");
  LoRa.print("PD");
  LoRa.print(">");
  LoRa.print(hubName);
  LoRa.print(":");
  LoRa.print(temp);
  LoRa.print(",");
  LoRa.print(humidity, 1);
  LoRa.print(",");
  LoRa.print(lux);
  LoRa.print(",");
  LoRa.print(distance, 2);
  LoRa.print(",");
  
  if (!LoRa.endPacket()) {
    logError("LoRa packet transmission failed");
    return;
  }
  
  logNetworkEvent("LoRa", "DATA_TX", "Sensor data transmitted successfully");
  
  getGlobalContext().sensors.lastLoRaTransmit = millis();
}

