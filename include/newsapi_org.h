#ifndef NEWSAPI_ORG_H
#define NEWSAPI_ORG_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "helperStuff.h"
#include "littlefsStuff.h"

// Function prototypes
bool getNewsapiData();
void removeNewsData();

// External global variables
//extern const char* newsapiHost[NEWS_HOST_SIZE];
extern const char* settingNewsAUTH[NEWS_AUTH_SIZE];
extern char      settingNewsNoWords[LOCAL_SIZE];
extern uint8_t   settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;
extern uint16_t  settingLDRlowOffset, settingLDRhighOffset;
extern char      settingWeerLiveAUTH[WEER_AUTH_SIZE], settingWeerLiveLocation[WEER_LIVE_LOC_SIZE];
extern uint8_t   settingWeerLiveInterval;
extern uint8_t   settingNewsInterval, settingNewsMaxMsg;

extern const int httpPort;
extern int newsapiStatus;
extern char newsMessage[LOCAL_SIZE];
extern int32_t maxWait;
extern char jsonResponse[];
extern char val[];
extern WiFiClient newsapiClient;

#endif // NEWSAPI_ORG_H
