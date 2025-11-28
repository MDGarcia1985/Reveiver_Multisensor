#pragma once
#include <Arduino.h>
#include "ESP32_NOW_Serial.h"
#include "MacAddress.h"
#include "WiFi.h"
#include "esp_wifi.h"

#define ESPNOW_WIFI_MODE_STATION 1
#define ESPNOW_WIFI_CHANNEL 1

#if ESPNOW_WIFI_MODE_STATION
#define ESPNOW_WIFI_MODE WIFI_STA
#define ESPNOW_WIFI_IF   WIFI_IF_STA
#else
#define ESPNOW_WIFI_MODE WIFI_AP
#define ESPNOW_WIFI_IF   WIFI_IF_AP
#endif

bool initializeNowSerial(uint8_t* mac);
void initializeNowFromEEPROM();
bool parseDistance(const char* message, float* distance);
void handleNowMessages();