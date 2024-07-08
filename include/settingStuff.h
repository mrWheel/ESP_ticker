#ifndef SETTINGSSTUFF_H
#define SETTINGSSTUFF_H

#include <Arduino.h>
#include <LittleFS.h>
#include "allDefines.h"

// Function Prototypes
void writeSettings(bool show);
void readSettings(bool show);
void updateSetting(const char *field, const char *newValue);

// External global variables
extern char settingHostname[HOSTNAME_SIZE];
extern char settingNewsNoWords[LOCAL_SIZE];
extern uint8_t settingLocalMaxMsg;
extern uint8_t settingTextSpeed;
extern uint8_t settingLDRlowOffset;
extern uint8_t settingLDRhighOffset;
extern uint8_t settingMaxIntensity;
extern char settingWeerLiveAUTH[];
extern char settingWeerLiveLocation[];
extern uint8_t settingWeerLiveInterval;
extern char settingNewsAUTH[51];
extern uint8_t settingNewsMaxMsg;
extern uint8_t settingNewsInterval;
extern bool Verbose;
//extern const char SETTINGS_FILE[];
extern const char _HOSTNAME[];
extern char tempMessage[];

#endif // SETTINGSSTUFF_H
