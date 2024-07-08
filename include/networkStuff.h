#ifndef NETWORKSTUFF_H
#define NETWORKSTUFF_H

#include <Arduino.h>
#include <Esp8266WiFi.h>

// Function prototypes
void setupWiFi(const char* ssid, const char* password);
void printWiFiStatus();
void reconnectWiFi();
void startServer();
void handleClientRequests();

// External global variables
extern WiFiServer wifiServer;
extern const char* ssid;
extern const char* password;

#endif // NETWORKSTUFF_H
