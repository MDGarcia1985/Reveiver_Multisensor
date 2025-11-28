/**
 * EventQueue.cpp - Minimal event queue system implementation
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

#include "EventQueue.h"

// Global FreeRTOS queue handle for inter-task communication
QueueHandle_t eventQueue = NULL;

/**
 * Initialize the global event queue for inter-task communication
 * 
 * Creates a FreeRTOS queue with EVENT_QUEUE_SIZE capacity for EventMessage structures.
 * Must be called during system initialization before any tasks are created.
 * 
 * @return true if queue created successfully, false on failure
 */
bool initEventQueue() {
  eventQueue = xQueueCreate(EVENT_QUEUE_SIZE, sizeof(EventMessage));
  return (eventQueue != NULL);
}

/**
 * Send an event message to the global event queue
 * 
 * Constructs an EventMessage and sends it to the queue with timeout protection.
 * Non-blocking operation that returns immediately if queue is full.
 * 
 * @param type Event type identifier from EventType enum
 * @param data Optional 32-bit data payload (default: 0)
 * @param ptr Optional pointer payload (default: nullptr)
 * @return true if event sent successfully, false if queue full or invalid
 */
bool sendEvent(EventType type, uint32_t data, void* ptr) {
  if (!eventQueue) return false;
  
  EventMessage event = {
    .type = type,
    .data = data,
    .ptr = ptr
  };
  
  return xQueueSend(eventQueue, &event, pdMS_TO_TICKS(EVENT_TIMEOUT_MS)) == pdTRUE;
}

/**
 * Receive an event message from the global event queue
 * 
 * Blocks until an event is available or timeout expires.
 * Used by tasks to wait for inter-task communication events.
 * 
 * @param event Pointer to EventMessage structure to receive data
 * @param timeout Maximum time to wait (default: EVENT_TIMEOUT_MS)
 * @return true if event received, false on timeout or invalid parameters
 */
bool receiveEvent(EventMessage* event, TickType_t timeout) {
  if (!eventQueue || !event) return false;
  
  return xQueueReceive(eventQueue, event, timeout) == pdTRUE;
}

/**
 * Send an event message from interrupt service routine context
 * 
 * ISR-safe version of sendEvent() that can be called from interrupt handlers.
 * Automatically yields to higher priority tasks if queue operation unblocks them.
 * 
 * @param type Event type identifier from EventType enum
 * @param data Optional 32-bit data payload (default: 0)
 * @param ptr Optional pointer payload (default: nullptr)
 * @return true if event sent successfully, false if queue full or invalid
 */
bool sendEventFromISR(EventType type, uint32_t data, void* ptr) {
  if (!eventQueue) return false;
  
  EventMessage event = {
    .type = type,
    .data = data,
    .ptr = ptr
  };
  
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t result = xQueueSendFromISR(eventQueue, &event, &xHigherPriorityTaskWoken);
  
  // Yield to higher priority task if queue operation unblocked one
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
  
  return result == pdTRUE;
}