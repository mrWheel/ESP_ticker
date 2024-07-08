#include "settingStuff.h"

int eepromAddress = 0;
int mode = 0;

void readEEPROM() {
  EEPROM.begin(512);
  mode = EEPROM.read(eepromAddress);
  EEPROM.end();
}

void writeEEPROM(int value) {
  EEPROM.begin(512);
  EEPROM.write(eepromAddress, value);
  EEPROM.commit();
  EEPROM.end();
}
