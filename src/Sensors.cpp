#include "Sensors.h"
#include "GlobalContext.h"
#include "SensorDataAccess.h"
#include "Tasks.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cmath>

static Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
static Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void initializeSensors() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.print("I2C initialized - SDA: GPIO");
  Serial.print(PIN_I2C_SDA);
  Serial.print(", SCL: GPIO");
  Serial.println(PIN_I2C_SCL);
  
  Serial.print("Initializing TSL2561 light sensor... ");
  if (!tsl.begin()) {
    Serial.println("FAILED!");
    Serial.println("⚠️  No TSL2561 detected. Light readings will be 0.");
  } else {
    configureTSL2561();
    Serial.println("OK!");
  }

  Serial.print("Initializing HTU21D-F temp/humidity sensor... ");
  if (!htu.begin()) {
    Serial.println("FAILED!");
    Serial.println("⚠️  No HTU21D-F detected. Temp/humidity will be 0.");
  } else {
    Serial.println("OK!");
  }
}

void configureTSL2561() {
  tsl.setGain(TSL2561_GAIN_16X);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}

void readEnvironmentalSensors() {
  float temp_c = htu.readTemperature();
  float humidity = htu.readHumidity();
  
  GlobalContext& ctx = getGlobalContext();
  
  if (!isnan(temp_c) && temp_c >= -40 && temp_c <= 85) {
    ctx.sensors.temperature = (int)temp_c;
  }
  
  if (!isnan(humidity) && humidity >= 0 && humidity <= 100) {
    ctx.sensors.humidity = humidity;
  }
  
  sensors_event_t event;
  if (tsl.getEvent(&event)) {
    if (event.light > 0 && event.light < 100000) {
      ctx.sensors.lux = (int)event.light;
    }
  }
  
  ctx.sensors.lastEnvironmentalUpdate = millis();
}



void printCurrentSensorValues() {
  printSensorDataSafe();
}