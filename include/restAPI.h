#ifndef RESTAPI_H
#define RESTAPI_H

#include "Debug.h"
#include "littlefsSTuff.h"
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// Prototypes
void processAPI();
void sendDeviceInfo();
void sendDeviceTime();
void sendDeviceSettings();
void sendLocalMessages();
void sendNewsMessages();
void postMessages();
void postSettings();
void sendApiNotFound(const char *URI);

// External variables
extern ESP8266WebServer httpServer;
extern char cMsg[];
extern bool Verbose;
extern String settingHostname;
extern uint8_t settingLocalMaxMsg;
extern uint8_t settingTextSpeed;
extern uint16_t settingLDRlowOffset;
extern uint16_t settingLDRhighOffset;
extern uint8_t settingMaxIntensity;
extern char settingWeerLiveAUTH[51];
extern String settingWeerLiveLocation;
extern int settingWeerLiveInterval;
extern String settingNewsAUTH;
extern uint8_t settingNewsMaxMsg;
extern uint8_t settingNewsInterval;
extern String settingNewsNoWords;
extern char fileMessage[];
extern char _FW_VERSION[];
extern char lastReset[];

#endif // RESTAPI_H
