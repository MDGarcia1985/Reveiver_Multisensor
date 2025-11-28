#include "LoRaLink.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include <cmath>

bool initializeLoRa() {
  Serial.println("\n========== Initializing LoRa ==========");
  Serial.println("Pin Configuration:");
  Serial.print("  SCK:  GPIO "); Serial.println(PIN_LORA_SCK);
  Serial.print("  MISO: GPIO "); Serial.println(PIN_LORA_MISO);
  Serial.print("  MOSI: GPIO "); Serial.println(PIN_LORA_MOSI);
  Serial.print("  CS:   GPIO "); Serial.println(PIN_LORA_CS);
  Serial.print("  RST:  GPIO "); Serial.println(PIN_LORA_RST);
  Serial.print("  DIO0: GPIO "); Serial.println(PIN_LORA_DIO0);
  Serial.println();
  
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  
  Serial.println("Attempting LoRa.begin(915E6)...");
  if (!LoRa.begin(915E6)) {
    Serial.println("✗✗✗ Starting LoRa FAILED! ✗✗✗");
    Serial.println("\nPossible issues:");
    Serial.println("1. Check LoRa module wiring");
    Serial.println("2. Verify pin definitions match your board");
    Serial.println("3. Ensure LoRa module has power (3.3V)");
    Serial.println("4. Check antenna is connected");
    Serial.println("========================================\n");
    return false;
  }
  
  Serial.println("✓✓✓ LoRa initialized successfully! ✓✓✓");
  getGlobalContext().loraActive = true;
  
  Serial.println("Creating LoRa hub...");
  delay(100);
  
  createHub("Greenhouse", "Temperature,Humidity,Lux,Distance", "1,2,1,2");
  
  Serial.println("========================================\n");
  return true;
}

void createHub(const char* hubName, const char* sensorNames, const char* types) {
  if (!getGlobalContext().loraActive) {
    Serial.println("ERROR: Cannot create hub - LoRa not active!");
    return;
  }
  
  if (!hubName || !sensorNames || !types) {
    Serial.println("ERROR: Invalid parameters for hub creation");
    return;
  }
  
  if (!LoRa.beginPacket()) {
    Serial.println("ERROR: Failed to begin LoRa packet");
    return;
  }
  
  LoRa.print("    ");
  LoRa.print("CH");
  LoRa.print(">");
  LoRa.print(hubName);
  LoRa.print(":");
  LoRa.print(sensorNames);
  LoRa.print(":");
  LoRa.print(types);
  
  if (!LoRa.endPacket()) {
    Serial.println("ERROR: Failed to send LoRa packet");
    return;
  }
  
  Serial.print("LoRa: Hub created - ");
  Serial.println(hubName);
  Serial.print("  Sensors: ");
  Serial.println(sensorNames);
  Serial.print("  Types: ");
  Serial.println(types);
}

void pushAllData(const char* hubName) {
  if (!getGlobalContext().loraActive) {
    Serial.println("ERROR: Cannot send data - LoRa not active!");
    return;
  }
  
  if (!hubName) {
    Serial.println("ERROR: Invalid hub name");
    return;
  }
  
  int temp;
  float humidity, distance;
  int lux;
  
  if (!getAllSensorData(&temp, &humidity, &lux, &distance)) {
    Serial.println("ERROR: Failed to read sensor data");
    return;
  }
  
  Serial.println("=== Sensor Readings ===");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");
  Serial.print("Lux: ");
  Serial.println(lux);
  Serial.print("Distance: ");
  Serial.print(distance, 2);
  Serial.println(" in");
  
  if (!LoRa.beginPacket()) {
    Serial.println("ERROR: Failed to begin LoRa packet");
    return;
  }
  
  LoRa.print("    ");
  LoRa.print("PD");
  LoRa.print(">");
  LoRa.print(hubName);
  LoRa.print(":");
  LoRa.print(temp);
  LoRa.print(",");
  LoRa.print(humidity, 1);
  LoRa.print(",");
  LoRa.print(lux);
  LoRa.print(",");
  LoRa.print(distance, 2);
  LoRa.print(",");
  
  if (!LoRa.endPacket()) {
    Serial.println("ERROR: Failed to send LoRa packet");
    return;
  }
  
  Serial.println("  -> LoRa: Sent all sensor data!");
  Serial.println("=======================\n");
  
  getGlobalContext().sensors.lastLoRaTransmit = millis();
}

