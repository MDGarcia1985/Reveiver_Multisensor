/**
 * GlobalContext.h - Centralized system state management
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
#include "ESP32_NOW_Serial.h"

struct SensorData {
  int temperature;
  float humidity;
  int lux;
  float distance;
  unsigned long lastEnvironmentalUpdate;
  unsigned long lastDistanceUpdate;
  unsigned long lastLoRaTransmit;
};

struct GlobalContext {
  // Sensor data
  SensorData sensors;
  
  // LoRa state
  bool loraActive;
  
  // ESP-NOW state
  ESP_NOW_Serial_Class* nowSerial;
  bool nowSerialActive;
  uint8_t peerMacAddress[6];
  bool macAddressSet;
  char receivedMessageBuffer[256];
  int receivedMessageLen;
  
  // Timing
  unsigned long currentTime;
};

GlobalContext& getGlobalContext();
void initializeGlobalContext();