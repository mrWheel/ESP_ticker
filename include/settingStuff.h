#ifndef SETTINGSSTUFF_H
#define SETTINGSSTUFF_H

#include <Arduino.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "helperStuff.h"
#include "networkStuff.h"
#include "newsapi_org.h"

// Function Prototypes
void writeSettings(bool show);
void readSettings(bool show);
void updateSetting(const char *field, const char *newValue);

// External global variables
extern char settingHostname[HOSTNAME_SIZE];
extern char settingNewsNoWords[LOCAL_SIZE];
extern uint8_t settingLocalMaxMsg;
extern uint8_t settingTextSpeed;
extern uint16_t settingLDRlowOffset;
extern uint16_t settingLDRhighOffset;
extern uint8_t settingMaxIntensity;
extern char settingWeerLiveAUTH[WEER_AUTH_SIZE];
extern char settingWeerLiveLocation[WEER_LIVE_LOC_SIZE];
extern uint8_t settingWeerLiveInterval;
extern const char *settingNewsAUTH[NEWS_AUTH_SIZE];
extern uint8_t settingNewsMaxMsg;
extern uint8_t settingNewsInterval;
extern bool Verbose;
//extern const char SETTINGS_FILE[];
//extern const char _HOSTNAME[HOSTNAME_SIZE];
extern char tempMessage[LOCAL_SIZE];

#endif // SETTINGSSTUFF_H
