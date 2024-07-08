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
extern char cMsg[NEWS_SIZE];
extern bool Verbose;
extern char settingHostname[41];
extern uint8_t settingLocalMaxMsg;
extern uint8_t settingTextSpeed;
extern uint8_t settingLDRlowOffset;
extern uint8_t settingLDRhighOffset;
extern uint8_t settingMaxIntensity;
extern char settingWeerLiveAUTH[51];
extern char settingWeerLiveLocation[51];
extern uint8_t settingWeerLiveInterval;
extern char settingNewsAUTH[51];
extern uint8_t settingNewsMaxMsg;
extern uint8_t settingNewsInterval;
extern char settingNewsNoWords[LOCAL_SIZE];
extern char fileMessage[LOCAL_SIZE];
extern char _FW_VERSION[20];
extern String lastReset;

#endif // RESTAPI_H
