#include "Commands.h"
#include "GlobalContext.h"
#include "Config.h"
#include "Sensors.h"
#include "LoRaLink.h"
#include "NowLink.h"
#include "WiFi.h"
#include <cstring>

struct CommandEntry {
  const char *name;
  void (*handler)(const char *args);
};

static CommandEntry commandTable[] = {
  {"config",  cmdConfig},
  {"sensors", cmdSensors},
  {"send",    cmdSend},
  {"status",  cmdStatus},
  {"lora",    cmdLora},
  {"reset",   cmdReset},
};

bool readSerialLine(char *buffer, size_t bufferSize) {
  if (!Serial.available() || bufferSize == 0) return false;

  int len = Serial.readBytesUntil('\n', buffer, bufferSize - 1);
  if (len <= 0) return false;

  buffer[len] = '\0';

  while (len > 0 && len < (int)bufferSize && (buffer[len - 1] == ' ' || buffer[len - 1] == '\r')) {
    buffer[--len] = '\0';
  }

  return len > 0;
}

void parseCommand(char *line, char *cmd, char *args) {
  if (!line || !cmd || !args) return;
  
  char *space = strchr(line, ' ');
  if (space) {
    *space = '\0';
    strncpy(args, space + 1, 29);
    args[29] = '\0';
  } else {
    args[0] = '\0';
  }

  int i = 0;
  for (char *p = line; *p && i < 19; ++p, ++i) {
    cmd[i] = tolower(*p);
  }
  cmd[i] = '\0';
}

void dispatchCommand(const char *cmd, const char *args) {
  size_t tableSize = sizeof(commandTable) / sizeof(commandTable[0]);
  for (size_t i = 0; i < tableSize; i++) {
    if (strcmp(cmd, commandTable[i].name) == 0) {
      commandTable[i].handler(args);
      return;
    }
  }

  Serial.println("Unknown command.");
  cmdHelp(args);
}

void handleSerialCommands() {
  char line[SERIAL_CMD_BUFFER_SIZE];
  if (readSerialLine(line, sizeof(line))) {
    char cmd[20];
    char args[30];
    parseCommand(line, cmd, args);
    dispatchCommand(cmd, args);
  }
}

void printStartupInfo() {
  Serial.println("\nCommands:");
  Serial.println("- Type 'config' to configure peer MAC address");
  Serial.println("- Type 'status' to show current configuration");
  Serial.println("- Type 'sensors' to read sensors now");
  Serial.println("- Type 'send' to send LoRa packet now");
  Serial.println("- Type 'lora' to retry LoRa initialization");
  Serial.println("- Type 'reset' to restart the device");

  GlobalContext& ctx = getGlobalContext();
  if (!ctx.macAddressSet) {
    Serial.println("\n⚠️  No peer MAC configured! Type 'config' to set up ESP-NOW");
  } else if (ctx.nowSerialActive) {
    Serial.println("\n✓ ESP-NOW ready - waiting for distance data...");
  }

  if (ctx.loraActive) {
    Serial.println("✓ LoRa ready - will transmit all sensor data!\n");
  } else {
    Serial.println("✗ LoRa NOT ready\n");
  }
}

void cmdConfig(const char *args) {
  configureMacAddress();
}

void cmdSensors(const char *args) {
  Serial.println("\nReading sensors now...");
  readEnvironmentalSensors();
  printCurrentSensorValues();
}

void cmdSend(const char *args) {
  if (!getGlobalContext().loraActive) {
    Serial.println("⚠️  LoRa not active!");
    return;
  }

  const char *hubName = (args && args[0]) ? args : "Greenhouse";

  Serial.print("Sending LoRa packet now (hub: ");
  Serial.print(hubName);
  Serial.println(")...");
  pushAllData(hubName);
}

void cmdStatus(const char *args) {
  Serial.println("\n=== DEVICE STATUS ===");
  Serial.print("This device MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("WiFi Mode: ");
  Serial.println("Station");

  Serial.print("WiFi Channel: ");
  Serial.println(1);

  GlobalContext& ctx = getGlobalContext();
  Serial.print("ESP-NOW Status: ");
  Serial.println(ctx.nowSerialActive ? "Active ✓" : "Inactive ✗");

  Serial.print("LoRa Status: ");
  Serial.println(ctx.loraActive ? "Active ✓" : "Inactive ✗");

  printCurrentSensorValues();
  Serial.println("====================\n");
}

void cmdLora(const char *args) {
  Serial.println("Retrying LoRa initialization...");
  initializeLoRa();
}

void cmdReset(const char *args) {
  Serial.println("Restarting device...");
  ESP.restart();
}

void cmdHelp(const char *args) {
  Serial.println("Available commands:");
  Serial.println("  config              - configure peer MAC address");
  Serial.println("  status              - show current configuration & sensor state");
  Serial.println("  sensors             - read sensors now");
  Serial.println("  send [hubName]      - send LoRa packet now (default: Greenhouse)");
  Serial.println("  lora                - retry LoRa initialization");
  Serial.println("  reset               - restart the device");
}