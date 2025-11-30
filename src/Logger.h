/**
 * Logger.h - Logging and telemetry abstraction layer
 * Version: 2.0.0
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

/**
 * Log levels for message filtering and prioritization
 * 
 * Hierarchical levels allow filtering of messages by importance.
 * Higher numeric values indicate more critical messages.
 */
typedef enum {
    LOG_DEBUG = 0,    // Detailed debugging information
    LOG_INFO = 1,     // General system information
    LOG_WARN = 2,     // Warning conditions that don't stop operation
    LOG_ERROR = 3,    // Error conditions that may affect functionality
    LOG_CRITICAL = 4  // Critical errors requiring immediate attention
} LogLevel;

/**
 * Log sink types for output destination selection
 * 
 * Defines available output destinations for log messages.
 * Multiple sinks can be active simultaneously.
 */
typedef enum {
    SINK_SERIAL = 0x01,   // Serial console output
    SINK_NETWORK = 0x02,  // Network telemetry (future implementation)
    SINK_STORAGE = 0x04   // Local storage (future implementation)
} LogSink;

// Logger configuration
#define LOG_BUFFER_SIZE 256
#define LOG_DEFAULT_LEVEL LOG_INFO
#define LOG_DEFAULT_SINKS SINK_SERIAL

// Logger initialization and configuration
void initLogger(LogLevel minLevel = LOG_DEFAULT_LEVEL, uint8_t sinks = LOG_DEFAULT_SINKS);
void setLogLevel(LogLevel level);
void setLogSinks(uint8_t sinks);

// Core logging functions with level-specific variants
void logMessage(LogLevel level, const char* format, ...);
void logDebug(const char* format, ...);
void logInfo(const char* format, ...);
void logWarn(const char* format, ...);
void logError(const char* format, ...);
void logCritical(const char* format, ...);

// Telemetry functions for structured data logging
void logSensorData(int temp, float humidity, int lux, float distance);
void logSystemEvent(const char* event, const char* details = nullptr);
void logNetworkEvent(const char* protocol, const char* event, const char* details = nullptr);