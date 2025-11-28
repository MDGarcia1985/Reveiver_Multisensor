/**
 * EventQueue.h - Minimal event queue system for inter-task communication
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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * Event types for inter-task communication
 * 
 * Defines all possible event types that can be sent between FreeRTOS tasks.
 * Each event represents a specific system state change or request.
 */
typedef enum {
    EVENT_SENSOR_DATA_READY,    // Environmental sensors have new data
    EVENT_DISTANCE_UPDATED,     // Distance sensor updated via ESP-NOW
    EVENT_LORA_SEND_REQUEST,    // Manual LoRa transmission requested
    EVENT_LORA_SEND_COMPLETE,   // LoRa transmission completed
    EVENT_CONFIG_CHANGED,       // System configuration modified
    EVENT_SYSTEM_ERROR          // System error occurred
} EventType;

/**
 * Event message structure for inter-task communication
 * 
 * Contains event type and optional data payloads for passing information
 * between FreeRTOS tasks through the global event queue.
 */
typedef struct {
    EventType type;    // Event identifier from EventType enum
    uint32_t data;     // Optional 32-bit numeric data
    void* ptr;         // Optional pointer to additional data
} EventMessage;

// Event queue configuration constants
#define EVENT_QUEUE_SIZE 10      // Maximum events in queue
#define EVENT_TIMEOUT_MS 100     // Default timeout for queue operations

// Global event queue handle
extern QueueHandle_t eventQueue;

// Event queue management functions
bool initEventQueue();
bool sendEvent(EventType type, uint32_t data = 0, void* ptr = nullptr);
bool receiveEvent(EventMessage* event, TickType_t timeout = pdMS_TO_TICKS(EVENT_TIMEOUT_MS));
bool sendEventFromISR(EventType type, uint32_t data = 0, void* ptr = nullptr);