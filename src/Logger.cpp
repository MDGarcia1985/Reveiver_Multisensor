/**
 * Logger.cpp - Logging and telemetry abstraction implementation
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

#include "Logger.h"
#include <stdarg.h>

// Logger state variables
static LogLevel currentLogLevel = LOG_DEFAULT_LEVEL;
static uint8_t activeSinks = LOG_DEFAULT_SINKS;
static char logBuffer[LOG_BUFFER_SIZE];

// Log level string representations for output formatting
static const char* logLevelStrings[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "CRIT"
};

/**
 * Initialize the logging system with specified level and output sinks
 * 
 * Sets up the logging subsystem with filtering level and output destinations.
 * Must be called during system initialization before any logging occurs.
 * 
 * @param minLevel Minimum log level to output (filters lower priority messages)
 * @param sinks Bitmask of active output sinks (SINK_SERIAL, SINK_NETWORK, etc.)
 */
void initLogger(LogLevel minLevel, uint8_t sinks) {
    currentLogLevel = minLevel;
    activeSinks = sinks;
    
    // Initialize serial sink if enabled
    if (activeSinks & SINK_SERIAL) {
        // Serial already initialized in main.cpp
    }
    
    // Future: Initialize network and storage sinks
}

/**
 * Set minimum log level for message filtering
 * 
 * @param level New minimum log level (messages below this level are filtered)
 */
void setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

/**
 * Configure active output sinks for log messages
 * 
 * @param sinks Bitmask of desired output destinations
 */
void setLogSinks(uint8_t sinks) {
    activeSinks = sinks;
}

/**
 * Core logging function with level filtering and multi-sink output
 * 
 * Formats message with timestamp and level, then outputs to all active sinks
 * if message level meets or exceeds current minimum level.
 * 
 * @param level Message priority level
 * @param format Printf-style format string
 * @param ... Variable arguments for format string
 */
void logMessage(LogLevel level, const char* format, ...) {
    // Filter messages below current log level
    if (level < currentLogLevel) return;
    
    va_list args;
    va_start(args, format);
    
    // Format timestamp and level prefix
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] %s: ", 
                            timestamp, logLevelStrings[level]);
    
    // Format user message
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    
    // Output to active sinks
    if (activeSinks & SINK_SERIAL) {
        Serial.println(logBuffer);
    }
    
    // Future: Output to network and storage sinks
    // if (activeSinks & SINK_NETWORK) { sendToNetwork(logBuffer); }
    // if (activeSinks & SINK_STORAGE) { writeToStorage(logBuffer); }
}

/**
 * Level-specific logging convenience functions
 * 
 * Provide simplified interface for common log levels without explicit level parameter.
 */
void logDebug(const char* format, ...) {
    if (LOG_DEBUG < currentLogLevel) return;
    va_list args;
    va_start(args, format);
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] DEBUG: ", timestamp);
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    if (activeSinks & SINK_SERIAL) Serial.println(logBuffer);
}

void logInfo(const char* format, ...) {
    if (LOG_INFO < currentLogLevel) return;
    va_list args;
    va_start(args, format);
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] INFO: ", timestamp);
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    if (activeSinks & SINK_SERIAL) Serial.println(logBuffer);
}

void logWarn(const char* format, ...) {
    if (LOG_WARN < currentLogLevel) return;
    va_list args;
    va_start(args, format);
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] WARN: ", timestamp);
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    if (activeSinks & SINK_SERIAL) Serial.println(logBuffer);
}

void logError(const char* format, ...) {
    if (LOG_ERROR < currentLogLevel) return;
    va_list args;
    va_start(args, format);
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] ERROR: ", timestamp);
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    if (activeSinks & SINK_SERIAL) Serial.println(logBuffer);
}

void logCritical(const char* format, ...) {
    if (LOG_CRITICAL < currentLogLevel) return;
    va_list args;
    va_start(args, format);
    unsigned long timestamp = millis();
    int prefixLen = snprintf(logBuffer, LOG_BUFFER_SIZE, "[%lu] CRIT: ", timestamp);
    vsnprintf(logBuffer + prefixLen, LOG_BUFFER_SIZE - prefixLen, format, args);
    va_end(args);
    if (activeSinks & SINK_SERIAL) Serial.println(logBuffer);
}

/**
 * Structured telemetry logging for sensor data
 * 
 * Logs sensor readings in structured format for telemetry analysis.
 * Uses INFO level for regular operational data.
 * 
 * @param temp Temperature in Celsius
 * @param humidity Relative humidity percentage
 * @param lux Light level in lux
 * @param distance Distance measurement in inches
 */
void logSensorData(int temp, float humidity, int lux, float distance) {
    logInfo("SENSOR_DATA temp=%d humidity=%.1f lux=%d distance=%.2f", 
            temp, humidity, lux, distance);
}

/**
 * Log system events with optional details
 * 
 * Records significant system state changes and events for monitoring.
 * 
 * @param event Event name or identifier
 * @param details Optional additional event information
 */
void logSystemEvent(const char* event, const char* details) {
    if (details) {
        logInfo("SYSTEM_EVENT %s: %s", event, details);
    } else {
        logInfo("SYSTEM_EVENT %s", event);
    }
}

/**
 * Log network protocol events with context information
 * 
 * Records network communication events for debugging and monitoring.
 * 
 * @param protocol Network protocol name (LoRa, ESP-NOW, etc.)
 * @param event Event type (connect, disconnect, send, receive, etc.)
 * @param details Optional event-specific details
 */
void logNetworkEvent(const char* protocol, const char* event, const char* details) {
    if (details) {
        logInfo("NETWORK_EVENT %s %s: %s", protocol, event, details);
    } else {
        logInfo("NETWORK_EVENT %s %s", protocol, event);
    }
}