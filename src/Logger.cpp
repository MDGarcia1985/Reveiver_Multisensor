/**
 * Logger.cpp - Logging and telemetry abstraction implementation
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

#include "Logger.h"
#include <stdarg.h>

// -----------------------------------------------------------------------------
// Internal logger state
// -----------------------------------------------------------------------------

static LogLevel currentLogLevel = LOG_DEFAULT_LEVEL;
static uint8_t  activeSinks     = LOG_DEFAULT_SINKS;

// Single shared buffer for formatted log lines.
// NOTE: Not thread/task-safe if used concurrently from multiple tasks.
static char logBuffer[LOG_BUFFER_SIZE];

// Log level string representations for output formatting.
// Assumes LogLevel is ordered: DEBUG, INFO, WARN, ERROR, CRITICAL.
static const char* logLevelStrings[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "CRIT"
};

static const int LOG_LEVEL_COUNT = sizeof(logLevelStrings) / sizeof(logLevelStrings[0]);

// -----------------------------------------------------------------------------
// Internal helper: core formatting logic (va_list-based)
// -----------------------------------------------------------------------------

/**
 * Internal var-arg logging core.
 *
 * - Clamps level to valid range before indexing logLevelStrings
 * - Checks for prefix truncation before calling vsnprintf
 * - Writes into logBuffer and outputs to all configured sinks
 */
static void vlogMessage(LogLevel level, const char* format, va_list args) {
    if (!format) {
        return;
    }

    // Filter by current level (safety double-check; wrappers do this too)
    if (level < currentLogLevel) {
        return;
    }

    // Clamp level to a safe range for indexing
    int lvl = static_cast<int>(level);
    if (lvl < 0 || lvl >= LOG_LEVEL_COUNT) {
        lvl = static_cast<int>(LOG_ERROR);  // Fallback to ERROR label
    }

    // Timestamp + level prefix
    unsigned long timestamp = millis();
    int prefixLen = snprintf(
        logBuffer,
        LOG_BUFFER_SIZE,
        "[%lu] %s: ",
        timestamp,
        logLevelStrings[lvl]
    );

    // If prefix didn't fit or error occurred, bail out
    if (prefixLen < 0 || prefixLen >= (int)LOG_BUFFER_SIZE) {
        return;
    }

    // Format user message into remaining buffer space
    vsnprintf(
        logBuffer + prefixLen,
        LOG_BUFFER_SIZE - (size_t)prefixLen,
        format,
        args
    );

    // Output to active sinks
    if (activeSinks & SINK_SERIAL) {
        Serial.println(logBuffer);
    }

    // Future:
    // if (activeSinks & SINK_NETWORK) { sendToNetwork(logBuffer); }
    // if (activeSinks & SINK_STORAGE) { writeToStorage(logBuffer); }
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void initLogger(LogLevel minLevel, uint8_t sinks) {
    currentLogLevel = minLevel;
    activeSinks     = sinks;

    // Serial sink is assumed to be initialized elsewhere (e.g. setup/main)
    // Future: initialize network / storage sinks here as needed.
}

void setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void setLogSinks(uint8_t sinks) {
    activeSinks = sinks;
}

/**
 * Core logging entry point (printf-style)
 */
void logMessage(LogLevel level, const char* format, ...) {
    // Filter early to avoid building messages we won't print
    if (level < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(level, format, args);
    va_end(args);
}

// -----------------------------------------------------------------------------
// Level-specific convenience wrappers
// -----------------------------------------------------------------------------

void logDebug(const char* format, ...) {
    if (LOG_DEBUG < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(LOG_DEBUG, format, args);
    va_end(args);
}

void logInfo(const char* format, ...) {
    if (LOG_INFO < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(LOG_INFO, format, args);
    va_end(args);
}

void logWarn(const char* format, ...) {
    if (LOG_WARN < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(LOG_WARN, format, args);
    va_end(args);
}

void logError(const char* format, ...) {
    if (LOG_ERROR < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(LOG_ERROR, format, args);
    va_end(args);
}

void logCritical(const char* format, ...) {
    if (LOG_CRITICAL < currentLogLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    vlogMessage(LOG_CRITICAL, format, args);
    va_end(args);
}

// -----------------------------------------------------------------------------
// Structured helpers
// -----------------------------------------------------------------------------

void logSensorData(int temp, float humidity, int lux, float distance) {
    logInfo(
        "SENSOR_DATA temp=%d humidity=%.1f lux=%d distance=%.2f",
        temp,
        humidity,
        lux,
        distance
    );
}

void logSystemEvent(const char* event, const char* details) {
    if (details) {
        logInfo("SYSTEM_EVENT %s: %s", event, details);
    } else {
        logInfo("SYSTEM_EVENT %s", event);
    }
}

void logNetworkEvent(const char* protocol, const char* event, const char* details) {
    if (details) {
        logInfo("NETWORK_EVENT %s %s: %s", protocol, event, details);
    } else {
        logInfo("NETWORK_EVENT %s %s", protocol, event);
    }
}
