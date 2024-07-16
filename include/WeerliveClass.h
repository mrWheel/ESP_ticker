#ifndef WEERLIVE_H
#define WEERLIVE_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>              // http web access library
#include <ArduinoJson.h>                    // JSON decoding library

class Weerlive 
{
  public:
      Weerlive(WiFiClient& client);
      void setup(const char* key, const char* city);
      const char* request();

  private:
      WiFiClient& client;
      String apiUrl;
      static const char* apiHost;
      StaticJsonDocument<3000> doc;
      StaticJsonDocument<500> filter;
      String payload;
      String weerliveText;
      int alarmInd = -1;

      void configureFilters();
};

#endif // WEERLIVE_H
