#include "NowLink.h"
#include "GlobalContext.h"
#include "Config.h"
#include "SensorDataAccess.h"
#include "Tasks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstring>

bool initializeNowSerial(uint8_t* mac) {
  GlobalContext& ctx = getGlobalContext();
  if (ctx.nowSerial != nullptr) {
    delete ctx.nowSerial;
    ctx.nowSerial = nullptr;
    ctx.nowSerialActive = false;
  }

  Serial.println("\n--- Initializing ESP-NOW ---");
  Serial.print("My WiFi Mode: ");
  Serial.println("Station");
  Serial.print("Attempting to connect to peer: ");
  printMacAddress(mac);

  MacAddress peer_mac({mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]});
  ctx.nowSerial = new ESP_NOW_Serial_Class(peer_mac, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IF);

  if (ctx.nowSerial->begin(115200)) {
    Serial.println("✓ ESP-NOW communication started successfully!");
    setMacAddress(mac);
    ctx.nowSerialActive = true;
    Serial.println("-----------------------------\n");
    return true;
  } else {
    Serial.println("✗ Failed to start ESP-NOW communication");
    Serial.println("-----------------------------\n");
    delete ctx.nowSerial;
    ctx.nowSerial = nullptr;
    return false;
  }
}

void initializeNowFromEEPROM() {
  Serial.print("\nWiFi Mode: ");
  Serial.println("Station");
  WiFi.mode(ESPNOW_WIFI_MODE);

  Serial.print("Channel: ");
  Serial.println(ESPNOW_WIFI_CHANNEL);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  while (!(WiFi.STA.started() || WiFi.AP.started())) {
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

void handleNowMessages() {
  GlobalContext& ctx = getGlobalContext();
  if (ctx.nowSerialActive && ctx.nowSerial != nullptr && ctx.nowSerial->available()) {
    int bytesProcessed = 0;
    const int maxBytesPerLoop = 32;
    
    while (ctx.nowSerial->available() && bytesProcessed < maxBytesPerLoop) {
      char receivedChar = ctx.nowSerial->read();
      bytesProcessed++;

      if (receivedChar == '\n') {
        if (ctx.receivedMessageLen > 0 && ctx.receivedMessageLen < (int)sizeof(ctx.receivedMessageBuffer)) {
          ctx.receivedMessageBuffer[ctx.receivedMessageLen] = '\0';
          
          float distance;
          if (parseDistance(ctx.receivedMessageBuffer, &distance)) {
            if (setSensorDistance(distance)) {
              Serial.print("ESP-NOW: Distance updated -> ");
              Serial.print(distance, 2);
              Serial.println(" in");
            }
          }
          
          ctx.receivedMessageLen = 0;
        }
      } else if (receivedChar != '\r' && receivedChar >= 32 && receivedChar <= 126) {
        if (ctx.receivedMessageLen < (int)sizeof(ctx.receivedMessageBuffer) - 1) {
          ctx.receivedMessageBuffer[ctx.receivedMessageLen++] = receivedChar;
        } else {
          ctx.receivedMessageLen = 0;
        }
      }
    }
  }
}

