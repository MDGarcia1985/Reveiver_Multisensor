#include "GlobalContext.h"

static GlobalContext g_context = {
  .sensors = {0, 0.0, 0, 0.0, 0, 0, 0},
  .loraActive = false,
  .nowSerial = nullptr,
  .nowSerialActive = false,
  .peerMacAddress = {0},
  .macAddressSet = false,
  .receivedMessageBuffer = {0},
  .receivedMessageLen = 0,
  .currentTime = 0
};

GlobalContext& getGlobalContext() {
  return g_context;
}

void initializeGlobalContext() {
  g_context.sensors = {0, 0.0, 0, 0.0, 0, 0, 0};
  g_context.loraActive = false;
  g_context.nowSerial = nullptr;
  g_context.nowSerialActive = false;
  g_context.macAddressSet = false;
  g_context.receivedMessageLen = 0;
  g_context.currentTime = 0;
}