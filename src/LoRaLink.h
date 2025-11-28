#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "pins.h"

#define LORA_TRANSMIT_INTERVAL 1000

bool initializeLoRa();
void createHub(const char* hubName, const char* sensorNames, const char* types);
void pushAllData(const char* hubName);