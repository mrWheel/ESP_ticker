#ifndef RESTAPI_H
#define RESTAPI_H


#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "Debug.h"
#include "allDefines.h"
#include "littlefsSTuff.h"
#include "jsonStuff.h"
#include "settingStuff.h"

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
extern char settingHostname[];
extern uint8_t settingLocalMaxMsg;
extern uint8_t settingTextSpeed;
extern uint16_t settingLDRlowOffset;
extern uint16_t settingLDRhighOffset;
extern uint8_t settingMaxIntensity;
extern char settingWeerLiveAUTH[];
extern char settingWeerLiveLocation[];
extern uint8_t settingWeerLiveInterval;
extern char settingNewsAUTH[];
extern uint8_t settingNewsMaxMsg;
extern uint8_t settingNewsInterval;
extern char settingNewsNoWords[];
extern char fileMessage[];
//extern char _FW_VERSION[];
extern String lastReset;

#endif // RESTAPI_H
