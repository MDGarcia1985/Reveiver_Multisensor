/**
 * Commands.h - Serial command interface
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

#define SERIAL_CMD_BUFFER_SIZE 50

bool readSerialLine(char *buffer, size_t bufferSize);
void parseCommand(char *line, char *cmd, char *args);
void dispatchCommand(const char *cmd, const char *args);
void handleSerialCommands();
void printStartupInfo();

// Command handlers
void cmdConfig(const char *args);
void cmdSensors(const char *args);
void cmdSend(const char *args);
void cmdStatus(const char *args);
void cmdLora(const char *args);
void cmdReset(const char *args);
void cmdHelp(const char *args);