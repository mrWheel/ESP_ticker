#ifndef LITTLEFS_STUFF_H
#define LITTLEFS_STUFF_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "timeStuff.h"

extern char cDate[11];
extern char cTime[9];
extern uint32_t nrReboots;
extern char fileMessage[LOCAL_SIZE];
extern char tempMessage[LOCAL_SIZE];
extern uint8_t  settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;


void readLastStatus();
void writeLastStatus();
bool readFileById(const char* fType, uint8_t mId);
bool writeFileById(const char* fType, uint8_t mId, const char *msg);
void updateMessage(const char *field, const char *newValue);
void writeToLog(const char *logLine);

#endif // LITTLEFS_STUFF_H
