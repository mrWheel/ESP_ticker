#ifndef TIMESTUFF_H
#define TIMESTUFF_H

#include <Arduino.h>
#include <TimeLib.h>
#include "allDefines.h"
#include "helperStuff.h"

// Function Prototypes
String buildDateTimeString(const char* timeStamp, int len);
void epochToTimestamp(time_t t, char *ts, int8_t len);
int8_t SecondFromTimestamp(const char *timeStamp);
int8_t MinuteFromTimestamp(const char *timeStamp);
int8_t HourFromTimestamp(const char *timeStamp);
int8_t DayFromTimestamp(const char *timeStamp);
int8_t MonthFromTimestamp(const char *timeStamp);
int8_t YearFromTimestamp(const char *timeStamp);
int32_t HoursKeyTimestamp(const char *timeStamp);
time_t epoch(const char *timeStamp, int8_t len, bool syncTime);

// External global variables
extern bool Verbose;

#endif // TIMESTUFF_H
