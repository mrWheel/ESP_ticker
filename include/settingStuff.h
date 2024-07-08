#ifndef SETTINGSTUFF_H
#define SETTINGSTUFF_H

#include <Arduino.h>
#include <EEPROM.h>

extern int eepromAddress;
extern int mode;

void readEEPROM();
void writeEEPROM(int value);

#endif
