#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <EventDispatcher.hpp>
#include <WiFiClient.h>

class WiFiManager {
private:
  ESP8266WiFiMulti *wifiMulti;
  EventDispatcher *dispatcher;
  bool connected = false;
  bool shouldConnect = false;

public:
  static constexpr const char *WiFiConnectedEvent = "wifi_connected";
  static constexpr const char *WiFiDisconnectedEvent = "wifi_disconnected";

  WiFiManager(ESP8266WiFiMulti *wifiMulti, EventDispatcher *dispatcher,
              const char *ssid, const char *password) {
    this->wifiMulti = wifiMulti;
    this->dispatcher = dispatcher;
    WiFi.mode(WIFI_STA);
    this->wifiMulti->addAP(ssid, password);
  }

  void connect() { this->shouldConnect = true; }

  void disconnect() {
    this->shouldConnect = false;
    WiFi.disconnect();
    this->connected = false;
    this->dispatcher->dispatch(WiFiManager::WiFiDisconnectedEvent);
  }

  void loop() {
    if (this->shouldConnect && !this->connected &&
        this->wifiMulti->run() == WL_CONNECTED) {
      this->connected = true;
      this->shouldConnect = false;
      this->dispatcher->dispatch(WiFiManager::WiFiConnectedEvent);
    }
  }
};