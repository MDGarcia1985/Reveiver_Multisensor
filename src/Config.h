#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 64
#define MAC_ADDRESS_SIZE 6
#define EEPROM_MAC_ADDR 0
#define EEPROM_INIT_FLAG 48

void initializeConfig();
void printMacAddress(uint8_t* mac);
void saveMacToEEPROM(uint8_t* mac);
bool loadMacFromEEPROM(uint8_t* mac);
bool parseMacAddress(const char* macStr, uint8_t* mac);
void configureMacAddress();
void setMacAddress(uint8_t* mac);