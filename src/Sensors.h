#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_HTU21DF.h>
#include "pins.h"

#define ENVIRONMENTAL_SENSOR_INTERVAL 1000

void initializeSensors();
void configureTSL2561();
void readEnvironmentalSensors();
void printCurrentSensorValues();