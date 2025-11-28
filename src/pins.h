#pragma once

// Board selection - uncomment one
#define BOARD_ESP32_DEV
// #define BOARD_ESP32_S3
// #define BOARD_CUSTOM

#ifdef BOARD_ESP32_DEV
  // Standard ESP32 DevKit pins
  #define PIN_I2C_SDA         21
  #define PIN_I2C_SCL         22
  
  #define PIN_LORA_SCK        SCK   // 18
  #define PIN_LORA_MISO       MISO  // 19
  #define PIN_LORA_MOSI       MOSI  // 23
  #define PIN_LORA_CS         12
  #define PIN_LORA_RST        13
  #define PIN_LORA_DIO0       11
  
#elif defined(BOARD_ESP32_S3)
  // ESP32-S3 pins
  #define PIN_I2C_SDA         8
  #define PIN_I2C_SCL         9
  
  #define PIN_LORA_SCK        12
  #define PIN_LORA_MISO       13
  #define PIN_LORA_MOSI       11
  #define PIN_LORA_CS         10
  #define PIN_LORA_RST        9
  #define PIN_LORA_DIO0       8
  
#elif defined(BOARD_CUSTOM)
  // Custom board - define your pins here
  #define PIN_I2C_SDA         21
  #define PIN_I2C_SCL         22
  
  #define PIN_LORA_SCK        18
  #define PIN_LORA_MISO       19
  #define PIN_LORA_MOSI       23
  #define PIN_LORA_CS         5
  #define PIN_LORA_RST        14
  #define PIN_LORA_DIO0       2
  
#else
  #error "No board selected! Please define a board in pins.h"
#endif