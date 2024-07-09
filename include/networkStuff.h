#ifndef NETWORKSTUFF_H
#define NETWORKSTUFF_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <TelnetStream.h>
#include "allDefines.h"

// External global variables
extern ESP8266WebServer httpServer;
extern ESP8266HTTPUpdateServer httpUpdater;
//extern FSInfo LittleFSinfo;
extern bool LittleFSmounted;
extern bool isConnected;

// Function prototypes
void configModeCallback(WiFiManager *myWiFiManager);
void startWiFi(const char* hostname, int timeOut);
void startTelnet();
void startMDNS(const char *Hostname);

// Update server HTML content (assuming they are defined elsewhere in your project)
extern const char* UpdateServerIndex;
extern const char* UpdateServerSuccess;

#endif // NETWORKSTUFF_H
