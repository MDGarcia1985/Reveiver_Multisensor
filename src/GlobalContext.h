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