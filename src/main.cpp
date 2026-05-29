#include <Arduino.h>
#include "display.h"
#include "wifi_server.h"
#include "game.h"

#define FRAME_MS 200 // snake moves 5 times per second

unsigned long lastFrame = 0;

void setup()
{
  Serial.begin(115200);
  display_init();
  wifi_init();
  game_init();
}

void loop()
{
  ws_cleanup();
  unsigned long now = millis();
  if (now - lastFrame >= FRAME_MS)
  {
    lastFrame = now;
    game_update();
    game_draw();
  }
}