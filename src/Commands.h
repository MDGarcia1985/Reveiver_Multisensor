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