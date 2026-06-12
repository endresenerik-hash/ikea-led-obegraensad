#pragma once

#include "PluginManager.h"

#if __has_include("AsyncUDP.h")
#include "AsyncUDP.h"
#define ASYNC_UDP_ENABLED
#endif

#include <vector>

class DDPPlugin : public Plugin
{
private:
#ifdef ASYNC_UDP_ENABLED
  AsyncUDP *udp;
#endif

  std::vector<uint8_t> rxFrame;
  std::vector<uint8_t> renderFrame;

#if defined(ESP32)
  portMUX_TYPE frameMux = portMUX_INITIALIZER_UNLOCKED;
#endif

public:
  void setup() override;
  void teardown() override;
  void loop() override;
  const char *getName() const override;
};