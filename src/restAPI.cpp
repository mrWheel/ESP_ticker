/*
***************************************************************************
**  Program  : restAPI, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "restAPI.h"


//=======================================================================
void processAPI()
{
  //char fName[40] = "";
  char URI[50]   = "";
  String words[10];

  strncpy( URI, httpServer.uri().c_str(), sizeof(URI) );

  if (httpServer.method() == HTTP_GET)
    Serial.printf("from[%s] URI[%s] method[GET] \r\n"
                  , httpServer.client().remoteIP().toString().c_str()
                  , URI);
  else  Serial.printf("from[%s] URI[%s] method[PUT] \r\n"
                        , httpServer.client().remoteIP().toString().c_str()
                        , URI);

  if (ESP.getFreeHeap() < 2500) // to prevent firmware from crashing!
  {
    Serial.printf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap() );
    httpServer.send(500, "text/plain", "500: internal server error (low heap)\r\n");
    return;
  }

  int8_t wc = splitString(URI, '/', words, 10);

  if (Verbose)
  {
    Serial.print(">>");
    for (int w=0; w<wc; w++)
    {
      Serial.printf("word[%d] => [%s], ", w, words[w].c_str());
    }
    Serial.println(" ");
  }

  if (words[1] != "api")
  {
    sendApiNotFound(URI);
    return;
  }

  if (words[2] != "v0")
  {
    sendApiNotFound(URI);
    return;
  }

  if (words[3] == "devinfo")
  {
    sendDeviceInfo();
  }
  else if (words[3] == "devtime")
  {
    sendDeviceTime();
  }
  else if (words[3] == "settings")
  {
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      postSettings();
    }
    else
    {
      sendDeviceSettings();
    }
  }
  else if (words[3] == "messages")
  {
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      postMessages();
    }
    else
    {
      sendLocalMessages();
    }
  }
  else if (words[3] == "news")
  {
    sendNewsMessages();
  }
  else if (words[3] == "onticker")
  {
    Serial.println("words[3] is \"onticker\"!!!");
    sendTickerMessage();
  }
  else sendApiNotFound(URI);

} // processAPI()


//=======================================================================
void sendDeviceInfo()
{
  sendStartJsonObj("devinfo");

  sendNestedJsonObj("author", "Willem Aandewiel (www.aandewiel.nl)");
  sendNestedJsonObj("fwversion", _FW_VERSION);

  snprintf(cMsg, NEWS_SIZE, "%s %s", __DATE__, __TIME__);
  sendNestedJsonObj("compiled", cMsg);
  sendNestedJsonObj("hostname", settingHostname);
  sendNestedJsonObj("ipaddress", WiFi.localIP().toString().c_str());
  sendNestedJsonObj("macaddress", WiFi.macAddress().c_str());
  sendNestedJsonObj("freeheap", ESP.getFreeHeap());
  sendNestedJsonObj("maxfreeblock", ESP.getMaxFreeBlockSize());
  sendNestedJsonObj("chipid", String( ESP.getChipId(), HEX ).c_str());
  sendNestedJsonObj("coreversion", String( ESP.getCoreVersion() ).c_str() );
  sendNestedJsonObj("sdkversion", String( ESP.getSdkVersion() ).c_str());
  sendNestedJsonObj("cpufreq", ESP.getCpuFreqMHz());
  sendNestedJsonObj("sketchsize", formatFloat( (ESP.getSketchSize() / 1024.0), 3));
  sendNestedJsonObj("freesketchspace", formatFloat( (ESP.getFreeSketchSpace() / 1024.0), 3));

  snprintf(cMsg, NEWS_SIZE, "%08X", ESP.getFlashChipId());
  sendNestedJsonObj("flashchipid", cMsg);  // flashChipId
  sendNestedJsonObj("flashchipsize", formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3));
  sendNestedJsonObj("flashchiprealsize", formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3));
  /*** --AaW-
    LittleFS.info(LittleFSinfo);
    sendNestedJsonObj("spiffssize", formatFloat( (LittleFSinfo.totalBytes / (1024.0 * 1024.0)), 0));
  ***/
  sendNestedJsonObj("flashchipspeed", formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0));
  FlashMode_t ideMode = ESP.getFlashChipMode();
  sendNestedJsonObj("flashchipmode", flashMode[ideMode]);
  /*** --AaW-
    sendNestedJsonObj("boardtype",
  #ifdef ARDUINO_ESP8266_NODEMCU
       "ESP8266_NODEMCU"
  #endif
  #ifdef ARDUINO_ESP8266_GENERIC
       "ESP8266_GENERIC"
  #endif
  #ifdef ESP8266_ESP01
       "ESP8266_ESP01"
  #endif
  #ifdef ESP8266_ESP12
       "ESP8266_ESP12"
  #endif
    );
  ***/
  sendNestedJsonObj("ssid", WiFi.SSID().c_str());
  sendNestedJsonObj("wifirssi", WiFi.RSSI());
  //sendNestedJsonObj("uptime", upTime());

  sendNestedJsonObj("lastreset", lastReset);

  httpServer.sendContent("\r\n]}\r\n");

} // sendDeviceInfo()


//=======================================================================
void sendDeviceTime()
{
  char actTime[50];

  sendStartJsonObj("devtime");
  snprintf(actTime, 49, "%02d-%02d-%04d %02d:%02d"
           , localtime(&now)->tm_mday, localtime(&now)->tm_mon+1, localtime(&now)->tm_year+1900
           , localtime(&now)->tm_hour, localtime(&now)->tm_min);
  sendNestedJsonObj("dateTime", actTime);
  //--aaw- sendNestedJsonObj("epoch", (int)now());

  sendEndJsonObj();

} // sendDeviceTime()


//=======================================================================
void sendDeviceSettings()
{
  Serial.println("sending device settings ...\r");

  sendStartJsonObj("settings");

  sendJsonSettingObj("hostname",          settingHostname,         "s", HOSTNAME_SIZE -1);
  sendJsonSettingObj("localMaxMsg",       settingLocalMaxMsg,      "i",   1,   20);
  sendJsonSettingObj("textSpeed",         settingTextSpeed,        "i",  10, MAX_SPEED);
  sendJsonSettingObj("LDRlowOffset",      settingLDRlowOffset,     "i",   0,  500);
  sendJsonSettingObj("LDRhighOffset",     settingLDRhighOffset,    "i", 500, 1024);
  sendJsonSettingObj("maxIntensity",      settingMaxIntensity,     "i",   0,   15);
  sendJsonSettingObj("weerliveAUTH",      settingWeerLiveAUTH,     "s", WEER_AUTH_SIZE -1);
  sendJsonSettingObj("weerliveLocation",  settingWeerLiveLocation, "s", WEER_LIVE_LOC_SIZE -1);
  sendJsonSettingObj("weerliveInterval",  settingWeerLiveInterval, "i",  2,  120);
  sendJsonSettingObj("newsapiAUTH",       settingNewsAUTH,         "s", NEWS_AUTH_SIZE -1);
  sendJsonSettingObj("newsapiMaxMsg",     settingNewsMaxMsg,       "i",   1,   20);
  sendJsonSettingObj("newsapiInterval",   settingNewsInterval,     "i",  15,  120);
  sendJsonSettingObj("newsNoWords",       settingNewsNoWords,      "s", LCL_SIZE -1);

  sendEndJsonObj();

} // sendDeviceSettings()


//=======================================================================
void sendLocalMessages()
{
  int mID;

  Serial.println("sending local Messages ...\r");

  sendStartJsonObj("messages");

  for (mID=1; mID <= settingLocalMaxMsg; mID++)
  {
    if (readFileById("LCL", mID))
    {
      //--- next 5 lines are realy dirty ...
      char   newMsg[LCL_SIZE] = "";
      String tmp = String(fileMessage);
      tmp.replace("\\", "\\\\");
      snprintf(newMsg, LCL_SIZE, "%s", tmp.c_str());
      //sendJsonSettingObj(intToStr(mID), fileMessage, "s", sizeof(fileMessage) -1);
      sendJsonSettingObj(intToStr(mID), newMsg, "s", JSON_RESPONSE_SIZE -1);
    }
    else
    {
      sendJsonSettingObj(intToStr(mID), "", "s", JSON_RESPONSE_SIZE -1);
    }
  }

  sendEndJsonObj();

} // sendlocalMessages()


//=======================================================================
void sendNewsMessages()
{
  int nID;

  Serial.println("sending news Messages ...\r");

  sendStartJsonObj("newsapi");

  for (nID=0; nID <= settingNewsMaxMsg; nID++)
  {
    if (readFileById("NWS", nID))
    {
      sendJsonSettingObj(intToStr(nID), fileMessage, "s", LCL_SIZE -1);
    }
  }

  sendEndJsonObj();

} // sendNewsMessages()


//=======================================================================
void sendTickerMessage()
{
  Serial.printf("sending on Ticker Message ...[%s]\r\n", onTickerMessage);

  sendStartJsonObj("onticker");

  sendJsonSettingObj("onticker", onTickerMessage, "s", LCL_SIZE -1);

  sendEndJsonObj();

} // sendNewsMessages()


//=======================================================================
void postMessages()
{
  //------------------------------------------------------------
  // json string: {"name":"4","value":"Bericht tekst"}
  //------------------------------------------------------------
  // so, why not use ArduinoJSON library?
  // I say: try it yourself ;-) It won't be easy
  String wOut[5];
  String wPair[5];
  String jsonIn  = httpServer.arg(0).c_str();
  char field[25] = "";
  char newValue[101]="";
  jsonIn.replace("{", "");
  jsonIn.replace("}", "");
  jsonIn.replace("\"", "");
  int8_t wp = splitString(jsonIn.c_str(), ',',  wPair, 5) ;
  for (int i=0; i<wp; i++)
  {
    //Serial.printf("[%d] -> pair[%s]\r\n", i, wPair[i].c_str());
    int8_t wc = splitString(wPair[i].c_str(), ':',  wOut, 5) ;
    //Serial.printf("==> [%s] -> field[%s]->val[%s]\r\n", wPair[i].c_str(), wOut[0].c_str(), wOut[1].c_str());
    if (wOut[0].equalsIgnoreCase("name"))  strCopy(field, sizeof(field), wOut[1].c_str());
    if (wOut[0].equalsIgnoreCase("value")) strCopy(newValue, sizeof(newValue), wOut[1].c_str());
  }
  Serial.printf("--> field[%s] => newValue[%s]\r\n", field, newValue);
  updateMessage(field, newValue);
  httpServer.send(200, "application/json", httpServer.arg(0));

} // postMessages()


//=======================================================================
void postSettings()
{
  //------------------------------------------------------------
  // json string: {"name":"settingInterval","value":9}
  // json string: {"name":"settingHostname","value":"abc"}
  //------------------------------------------------------------
  // so, why not use ArduinoJSON library?
  // I say: try it yourself ;-) It won't be easy
  String wOut[5];
  String wPair[5];
  String jsonIn  = httpServer.arg(0).c_str();
  char field[25] = "";
  char newValue[101]="";
  jsonIn.replace("{", "");
  jsonIn.replace("}", "");
  jsonIn.replace("\"", "");
  int8_t wp = splitString(jsonIn.c_str(), ',',  wPair, 5) ;
  for (int i=0; i<wp; i++)
  {
    //Serial.printf("[%d] -> pair[%s]\r\n", i, wPair[i].c_str());
    int8_t wc = splitString(wPair[i].c_str(), ':',  wOut, 5) ;
    //Serial.printf("==> [%s] -> field[%s]->val[%s]\r\n", wPair[i].c_str(), wOut[0].c_str(), wOut[1].c_str());
    if (wOut[0].equalsIgnoreCase("name"))  strCopy(field, sizeof(field), wOut[1].c_str());
    if (wOut[0].equalsIgnoreCase("value")) strCopy(newValue, sizeof(newValue), wOut[1].c_str());
  }
  Serial.printf("--> field[%s] => newValue[%s]\r\n", field, newValue);
  updateSetting(field, newValue);
  httpServer.send(200, "application/json", httpServer.arg(0));

} // postSettings()


//====================================================
void sendApiNotFound(const char *URI)
{
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send ( 404, "text/html", "<!DOCTYPE HTML><html><head>");

  strCopy(cMsg,   NEWS_SIZE, "<style>body { background-color: lightgray; font-size: 15pt;}");
  strConcat(cMsg, NEWS_SIZE, "</style></head><body>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg,   NEWS_SIZE, "<h1>ESP - lichtKrant</h1><b1>");
  httpServer.sendContent(cMsg);

  strCopy(cMsg,   NEWS_SIZE, "<br>[<b>");
  strConcat(cMsg, NEWS_SIZE, URI);
  strConcat(cMsg, NEWS_SIZE, "</b>] is not a valid ");
  httpServer.sendContent(cMsg);

  strCopy(cMsg, NEWS_SIZE, "</body></html>\r\n");
  httpServer.sendContent(cMsg);

} // sendApiNotFound()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
****************************************************************************
*/