#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebSocket ws;

void wifi_init();
void ws_cleanup();
void ws_broadcast(const char *msg);