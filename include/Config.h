/**
 * Config.h - EEPROM configuration management
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