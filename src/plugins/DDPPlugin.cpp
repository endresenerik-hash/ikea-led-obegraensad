#include "plugins/DDPPlugin.h"
#include <vector>

#ifdef ASYNC_UDP_ENABLED

static std::vector<uint8_t> rxFrame;     // latest received frame (network thread)
static std::vector<uint8_t> renderFrame; // frame used by loop (UI thread)
static portMUX_TYPE frameMux = portMUX_INITIALIZER_UNLOCKED;

void DDPPlugin::setup()
{
  udp = new AsyncUDP();

  if (udp->listen(4048))
  {
    Serial.println("DDP server listening at port: 4048");

    udp->onPacket([](AsyncUDPPacket packet)
    {
      if (packet.length() < 10) return;

      const uint8_t* data = packet.data() + 10;
      size_t len = packet.length() - 10;

      int count = std::min((int)(len / 3), ROWS * COLS);

      // Copy safely into RX buffer
      portENTER_CRITICAL(&frameMux);

      rxFrame.resize(count * 3);
      memcpy(rxFrame.data(), data, count * 3);

      portEXIT_CRITICAL(&frameMux);
    });
  }
}

void DDPPlugin::loop()
{
  // swap buffers safely
  portENTER_CRITICAL(&frameMux);
  renderFrame = rxFrame;
  portEXIT_CRITICAL(&frameMux);

  if (!renderFrame.empty())
  {
    int count = std::min((int)(renderFrame.size() / 3), ROWS * COLS);

    for (int i = 0; i < count; i++)
    {
      uint8_t r = renderFrame[i * 3 + 0];
      uint8_t g = renderFrame[i * 3 + 1];
      uint8_t b = renderFrame[i * 3 + 2];

      uint8_t brightness = (r + g + b) / 3;

      Screen.setPixelAtIndex(i, brightness > 4, brightness);
    }
  }

  delay(1);
}

void DDPPlugin::teardown()
{
  if (udp)
  {
    delete udp;
    udp = nullptr;
  }
}

const char* DDPPlugin::getName() const
{
  return "DDP";
}

#endif