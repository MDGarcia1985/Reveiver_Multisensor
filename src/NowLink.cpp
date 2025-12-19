/**
 * NowLink.cpp - ESP-NOW peer communication implementation
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

#include "NowLink.h"
#include "GlobalContext.h"
#include "Config.h"
#include "SensorDataAccess.h"
#include "Tasks.h"
#include "EventQueue.h"
#include "Logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstring>

bool initializeNowSerial(uint8_t* mac) {
  GlobalContext& ctx = getGlobalContext();
  
  Serial.println("\n--- Initializing ESP-NOW ---");
  Serial.print("My WiFi Mode: ");
  Serial.println("Station");
  Serial.print("Attempting to connect to peer: ");
  printMacAddress(mac);

  if (esp_now_init() != ESP_OK) {
    logNetworkEvent("ESP-NOW", "INIT_FAILED", "ESP-NOW initialization failed");
    return false;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    logNetworkEvent("ESP-NOW", "PEER_ADD_FAILED", "Failed to add peer");
    return false;
  }

  logNetworkEvent("ESP-NOW", "CONNECTED", "Peer communication established");
  setMacAddress(mac);
  ctx.nowSerialActive = true;
  return true;
}

void initializeNowFromEEPROM() {
  Serial.print("\nWiFi Mode: ");
  Serial.println("Station");
  WiFi.mode(ESPNOW_WIFI_MODE);

  Serial.print("Channel: ");
  Serial.println(ESPNOW_WIFI_CHANNEL);
  esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  while (WiFi.status() != WL_CONNECTED && WiFi.getMode() == WIFI_STA) {
    delay(100);
  }

  Serial.println("\n=== MAC ADDRESS INFO ===");
  Serial.print("Station MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("AP MAC:      ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("ACTIVE MODE: ");
  Serial.println("Station");
  Serial.print("USE THIS MAC FOR PEER CONFIG: ");
  Serial.println(WiFi.macAddress());
  Serial.println("========================\n");

  uint8_t storedMac[6];
  if (loadMacFromEEPROM(storedMac)) {
    Serial.print("Loaded peer MAC from EEPROM: ");
    printMacAddress(storedMac);
    initializeNowSerial(storedMac);
  } else {
    Serial.println("No MAC address found in EEPROM");
  }
}

bool parseDistance(const char* message, float* distance) {
  if (!message || !distance) return false;
  
  if (strncmp(message, "DIST:", 5) == 0) {
    char* endPtr;
    float val = strtof(message + 5, &endPtr);
    if (*endPtr != '\0' && *endPtr != '\r' && *endPtr != '\n') return false;
    if (val < 0 || val > 1000) return false;
    *distance = val;
    return true;
  }
  return false;
}

namespace {
  constexpr int MAX_BYTES_PER_LOOP = 32;
  constexpr char MIN_PRINTABLE = 32;
  constexpr char MAX_PRINTABLE = 126;

  inline bool isPrintableNowChar(char c) {
    return (c != '\r') && (c >= MIN_PRINTABLE) && (c <= MAX_PRINTABLE);
  }

  void handleCompleteNowLine(GlobalContext &ctx) {
    if (ctx.receivedMessageLen == 0 ||
        ctx.receivedMessageLen >= (int)sizeof(ctx.receivedMessageBuffer)) {
      ctx.receivedMessageLen = 0;
      return;
    }

    ctx.receivedMessageBuffer[ctx.receivedMessageLen] = '\0';

    float distance = 0.0f;
    if (!parseDistance(ctx.receivedMessageBuffer, &distance)) {
      ctx.receivedMessageLen = 0;
      return;
    }

    if (setSensorDistance(distance)) {
      logNetworkEvent("ESP-NOW", "DISTANCE_RX", nullptr);
      logDebug("Distance sensor updated: %.2f inches", distance);
      
      // Broadcast distance update event to other tasks (distance in cm * 100)
      sendEvent(EVENT_DISTANCE_UPDATED, (uint32_t)(distance * 100));
    }

    ctx.receivedMessageLen = 0;
  }
}

void handleNowMessages() {
  GlobalContext &ctx = getGlobalContext();
  
  // Fast exit if ESP-NOW not active
  if (!ctx.nowSerialActive) {
    return;
  }
  
  // ESP-NOW message handling is now done via callback
  // This function is kept for compatibility but messages
  // are processed in the ESP-NOW receive callback
}

