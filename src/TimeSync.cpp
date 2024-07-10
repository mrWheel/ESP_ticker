#include "TimeSync.h"

TimeSync::TimeSync() {}

void TimeSync::setup(const char *timeZone) 
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov",  "time.google.com");
    setenv("TZ", timeZone, 1);
    tzset();

    Serial.printf("Timezone set to: %s\r\n", timeZone);
}

bool TimeSync::isSynced() 
{
    time_t now;
    time(&now);
    
    return (bool)(localtime(&now)->tm_year > 120);
}

bool TimeSync::sync(uint16_t maxTry)
{
  time_t now;
  time(&now);
  uint8_t tries = 0;

  Serial.print("Get time from NTP service .");
  do
  {
    time(&now);
    delay(20);
    if ((tries%10)==0) {Serial.print('.'); }
    tries++;
  } while ((localtime(&now)->tm_year <= 120) && (tries < maxTry));

  if (localtime(&now)->tm_year > 120)
        Serial.println(" Synchronized with NTP Service!");
  else  Serial.println(" NOT SYNCED!");  

  return localtime(&now)->tm_year > 120;

}

void TimeSync::logTime() 
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.printf("%s", asctime(&timeinfo));
}
