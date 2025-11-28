/**
 * Sensors.h - Environmental sensor interface
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