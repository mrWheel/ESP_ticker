#ifndef WEERLIVE_NL_H
#define WEERLIVE_NL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

extern const char* ssid;
extern const char* password;
extern const char* host;
extern const char* apiKey;

void setupWiFi();
String getWeatherData();

#endif
