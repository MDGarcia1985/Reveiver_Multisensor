#include "Config.h"
#include "GlobalContext.h"
#include "NowLink.h"
#include <cstring>

void initializeConfig() {
  EEPROM.begin(EEPROM_SIZE);
}

void printMacAddress(uint8_t* mac) {
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void saveMacToEEPROM(uint8_t* mac) {
  if (!mac) {
    Serial.println("ERROR: Invalid MAC address pointer");
    return;
  }
  
  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    EEPROM.write(EEPROM_MAC_ADDR + i, mac[i]);
  }
  
  EEPROM.write(EEPROM_INIT_FLAG, 0xAA);
  if (!EEPROM.commit()) {
    Serial.println("ERROR: Failed to commit MAC to EEPROM");
    return;
  }
  
  Serial.println("MAC address saved to EEPROM!");
}

bool loadMacFromEEPROM(uint8_t* mac) {
  if (!mac) return false;
  
  if (EEPROM.read(EEPROM_INIT_FLAG) != 0xAA) {
    return false;
  }
  
  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    mac[i] = EEPROM.read(EEPROM_MAC_ADDR + i);
  }
  return true;
}

bool parseMacAddress(const char* macStr, uint8_t* mac) {
  if (!macStr || !mac) return false;
  
  char macCopy[20];
  strncpy(macCopy, macStr, sizeof(macCopy) - 1);
  macCopy[sizeof(macCopy) - 1] = '\0';
  
  char cleaned[13];
  int cleanedIdx = 0;
  for (int i = 0; macCopy[i] != '\0' && cleanedIdx < 12 && i < 19; i++) {
    if (macCopy[i] != ':') {
      cleaned[cleanedIdx++] = toupper(macCopy[i]);
    }
  }
  cleaned[cleanedIdx] = '\0';

  if (cleanedIdx != 12) return false;

  for (int i = 0; i < 6; i++) {
    if ((i * 2 + 1) >= cleanedIdx) return false;
    char byteStr[3] = {cleaned[i * 2], cleaned[i * 2 + 1], '\0'};
    char* endPtr;
    long val = strtol(byteStr, &endPtr, 16);
    if (*endPtr != '\0' || val < 0 || val > 255) return false;
    mac[i] = (uint8_t)val;
  }
  return true;
}

void configureMacAddress() {
  Serial.println("\n=== MAC ADDRESS CONFIGURATION ===");
  Serial.println("Enter peer MAC address (format: AA:BB:CC:DD:EE:FF):");
  Serial.println("Or type 'show' to display current MAC, 'clear' to reset");

  char inputBuffer[50];
  unsigned long timeout = millis() + 30000;
  
  while (millis() < timeout) {
    if (Serial.available()) {
      int len = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);
      if (len < 0 || len >= (int)sizeof(inputBuffer)) {
        Serial.println("Input too long or invalid");
        continue;
      }
      
      inputBuffer[len] = '\0';
      
      while (len > 0 && len < (int)sizeof(inputBuffer) && (inputBuffer[len-1] == ' ' || inputBuffer[len-1] == '\r')) {
        inputBuffer[--len] = '\0';
      }

      if (strcasecmp(inputBuffer, "show") == 0) {
        GlobalContext& ctx = getGlobalContext();
        if (ctx.macAddressSet) {
          Serial.print("Current peer MAC: ");
          printMacAddress(ctx.peerMacAddress);
        } else {
          Serial.println("No MAC address set");
        }
        continue;
      }

      if (strcasecmp(inputBuffer, "clear") == 0) {
        EEPROM.write(EEPROM_INIT_FLAG, 0x00);
        if (!EEPROM.commit()) {
          Serial.println("Failed to clear EEPROM");
          continue;
        }
        Serial.println("MAC address cleared from EEPROM");
        getGlobalContext().macAddressSet = false;
        continue;
      }

      uint8_t newMac[6];
      if (parseMacAddress(inputBuffer, newMac)) {
        if (initializeNowSerial(newMac)) {
          saveMacToEEPROM(newMac);
          Serial.print("Peer MAC address set to: ");
          printMacAddress(newMac);
          Serial.println("Configuration saved!\n");
          break;
        } else {
          Serial.println("Failed to initialize ESP-NOW with new MAC");
        }
      } else {
        Serial.println("Invalid MAC address format. Use AA:BB:CC:DD:EE:FF");
      }
    }
    delay(10);
  }
  
  if (millis() >= timeout) {
    Serial.println("Configuration timeout - returning to main loop");
  }
}

void setMacAddress(uint8_t* mac) {
  if (mac) {
    GlobalContext& ctx = getGlobalContext();
    memcpy(ctx.peerMacAddress, mac, 6);
    ctx.macAddressSet = true;
  }
}

