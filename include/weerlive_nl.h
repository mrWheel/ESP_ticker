#ifndef WEERLIVE_NL_H
#define WEERLIVE_NL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>

#include "Debug.h"
#include "allDefines.h"
#include "helperStuff.h"
#include "jsonStuff.h"

// Function prototype
void getWeerLiveData();

// External global variables
extern char settingWeerLiveAUTH[];
extern char settingWeerLiveLocation[];
extern char tempMessage[];
extern char cMsg[];
extern const int LOCAL_SIZE;


// Utility function prototypes (assuming they are defined elsewhere in your project)
void strTrimCntr(char *str, int maxLen);
void parseJsonKey(const char *json, const char *key, char *value, int maxLen);

#endif // WEERLIVE_NL_H
