#include "restAPI.h"

// Definitions for the functions
void processAPI() {
    char fName[40] = "";
    char URI[50] = "";
    String words[10];
    strncpy(URI, httpServer.uri().c_str(), sizeof(URI));
    if (httpServer.method() == HTTP_GET)
        DebugTf("from[%s] URI[%s] method[GET] \r\n", httpServer.client().remoteIP().toString().c_str(), URI); 
    else
        DebugTf("from[%s] URI[%s] method[PUT] \r\n", httpServer.client().remoteIP().toString().c_str(), URI); 
    if (ESP.getFreeHeap() < 8500) {
        DebugTf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap());
        httpServer.send(500, "text/plain", "500: internal server error (low heap)\r\n");
        return;
    }
    int8_t wc = splitString(URI, '/', words, 10);
    if (Verbose) {
        DebugT(">>");
        for (int w=0; w<wc; w++) {
            Debugf("word[%d] => [%s], ", w, words[w].c_str());
        }
        Debugln(" ");
    }
    if (words[1] != "api") {
        sendApiNotFound(URI);
        return;
    }
    if (words[2] != "v0") {
        sendApiNotFound(URI);
        return;
    }
    if (words[3] == "devinfo") {
        sendDeviceInfo();
    } else if (words[3] == "devtime") {
        sendDeviceTime();
    } else if (words[3] == "settings") {
        if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST) {
            postSettings();
        } else {
            sendDeviceSettings();
        }
    } else if (words[3] == "messages") {
        if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST) {
            postMessages();
        } else {
            sendLocalMessages();
        }
    } else if (words[3] == "news") {
        sendNewsMessages();
    } else {
        sendApiNotFound(URI);
    }
}

void sendDeviceInfo() {
    sendStartJsonObj("devinfo");
    sendNestedJsonObj("author", "Willem Aandewiel (www.aandewiel.nl)");
    sendNestedJsonObj("fwversion", _FW_VERSION);
    snprintf(cMsg, sizeof(cMsg), "%s %s", __DATE__, __TIME__);
    sendNestedJsonObj("compiled", cMsg);
    sendNestedJsonObj("hostname", settingHostname);
    sendNestedJsonObj("ipaddress", WiFi.localIP().toString().c_str());
    sendNestedJsonObj("macaddress", WiFi.macAddress().c_str());
    sendNestedJsonObj("freeheap", ESP.getFreeHeap());
    sendNestedJsonObj("maxfreeblock", ESP.getMaxFreeBlockSize());
    sendNestedJsonObj("chipid", String(ESP.getChipId(), HEX).c_str());
    sendNestedJsonObj("coreversion", String(ESP.getCoreVersion()).c_str());
    sendNestedJsonObj("sdkversion", String(ESP.getSdkVersion()).c_str());
    sendNestedJsonObj("cpufreq", ESP.getCpuFreqMHz());
    sendNestedJsonObj("sketchsize", formatFloat((ESP.getSketchSize() / 1024.0), 3));
    sendNestedJsonObj("freesketchspace", formatFloat((ESP.getFreeSketchSpace() / 1024.0), 3));
    snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());
    sendNestedJsonObj("flashchipid", cMsg);
    sendNestedJsonObj("flashchipsize", formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3));
    sendNestedJsonObj("flashchiprealsize", formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3));
    LittleFS.info(LittleFSinfo);
    sendNestedJsonObj("spiffssize", formatFloat((LittleFSinfo.totalBytes / (1024.0 * 1024.0)), 0));
    sendNestedJsonObj("flashchipspeed", formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0));
    FlashMode_t ideMode = ESP.getFlashChipMode();
    sendNestedJsonObj("flashchipmode", flashMode[ideMode]);
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
    sendNestedJsonObj("ssid", WiFi.SSID().c_str());
    sendNestedJsonObj("wifirssi", WiFi.RSSI());
    sendNestedJsonObj("lastreset", lastReset);
    httpServer.sendContent("\r\n]}\r\n");
}

void sendDeviceTime() {
    char actTime[50];
    sendStartJsonObj("devtime");
    snprintf(actTime, 49, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
    sendNestedJsonObj("dateTime", actTime);
    sendNestedJsonObj("epoch", (int)now());
    sendEndJsonObj();
}

void sendDeviceSettings() {
    DebugTln("sending device settings ...\r");
    sendStartJsonObj("settings");
    sendJsonSettingObj("hostname", settingHostname, "s", sizeof(settingHostname) - 1);
    sendJsonSettingObj("localMaxMsg", settingLocalMaxMsg, "i", 1, 20);
    sendJsonSettingObj("textSpeed", settingTextSpeed, "i", 10, MAX_SPEED);
    sendJsonSettingObj("LDRlowOffset", settingLDRlowOffset, "i", 0, 500);
    sendJsonSettingObj("LDRhighOffset", settingLDRhighOffset, "i", 500, 1024);
    sendJsonSettingObj("maxIntensity", settingMaxIntensity, "i", 0, 15);
    sendJsonSettingObj("weerliveAUTH", settingWeerLiveAUTH, "s", sizeof(settingWeerLiveAUTH) - 1);
    sendJsonSettingObj("weerliveLocation", settingWeerLiveLocation, "s", sizeof(settingWeerLiveLocation) - 1);
    sendJsonSettingObj("weerliveInterval", settingWeerLiveInterval, "i", 15, 120);
    sendJsonSettingObj("newsapiAUTH", settingNewsAUTH, "s", sizeof(settingNewsAUTH) - 1);
    sendJsonSettingObj("newsapiMaxMsg", settingNewsMaxMsg, "i", 1, 20);
    sendJsonSettingObj("newsapiInterval", settingNewsInterval, "i", 15, 120);
    sendJsonSettingObj("newsNoWords", settingNewsNoWords, "s", sizeof(settingNewsNoWords) - 1);
    sendEndJsonObj();
}

void sendLocalMessages() {
    int mID;
    DebugTln("sending local Messages ...\r");
    sendStartJsonObj("messages");
    for (mID = 1; mID <= settingLocalMaxMsg; mID++) {
        if (readFileById("LCL", mID)) {
            char newMsg[LOCAL_SIZE] = "";
            String tmp = String(fileMessage);
            tmp.replace("\\", "\\\\");
            sprintf(newMsg, "%s", tmp.c_str());
            sendJsonSettingObj(intToStr(mID), newMsg, "s", sizeof(newMsg) - 1);
        } else {
            sendJsonSettingObj(intToStr(mID), "", "s", sizeof(fileMessage) - 1);
        }
    }
    httpServer.sendContent("\r\n]}\r\n");
}

void sendNewsMessages() {
    int mID;
    DebugTln("sending News Messages ...\r");
    sendStartJsonObj("news");
    for (mID = 1; mID <= settingNewsMaxMsg; mID++) {
        if (readFileById("NWS", mID)) {
            char newMsg[NEWS_SIZE] = "";
            String tmp = String(fileMessage);
            tmp.replace("\\", "\\\\");
            sprintf(newMsg, "%s", tmp.c_str());
            sendJsonSettingObj(intToStr(mID), newMsg, "s", sizeof(newMsg) - 1);
        } else {
            sendJsonSettingObj(intToStr(mID), "", "s", sizeof(fileMessage) - 1);
        }
    }
    httpServer.sendContent("\r\n]}\r\n");
}

void postMessages() {
    DynamicJsonDocument doc(1024);
    if (httpServer.hasArg("plain")) {
        DeserializationError error = deserializeJson(doc, httpServer.arg("plain"));
        if (error) {
            httpServer.send(400, "application/json", "{\"status\":\"Invalid JSON\"}");
            return;
        }
    } else {
        httpServer.send(400, "application/json", "{\"status\":\"No JSON body\"}");
        return;
    }

    for (JsonPair kv : doc.as<JsonObject>()) {
        const char* key = kv.key().c_str();
        const char* value = kv.value();
        DebugTf("key[%s] value[%s]\r\n", key, value);
        writeFileById("LCL", atoi(key), value);
    }
    httpServer.send(200, "application/json", "{\"status\":\"ok\"}");
}

void postSettings() {
    DynamicJsonDocument doc(1024);
    if (httpServer.hasArg("plain")) {
        DeserializationError error = deserializeJson(doc, httpServer.arg("plain"));
        if (error) {
            httpServer.send(400, "application/json", "{\"status\":\"Invalid JSON\"}");
            return;
        }
    } else {
        httpServer.send(400, "application/json", "{\"status\":\"No JSON body\"}");
        return;
    }

    for (JsonPair kv : doc.as<JsonObject>()) {
        const char* key = kv.key().c_str();
        const char* value = kv.value();
        DebugTf("key[%s] value[%s]\r\n", key, value);
        if (String(key) == "hostname") {
            settingHostname = String(value);
        } else if (String(key) == "localMaxMsg") {
            settingLocalMaxMsg = atoi(value);
        } else if (String(key) == "textSpeed") {
            settingTextSpeed = atoi(value);
        } else if (String(key) == "LDRlowOffset") {
            settingLDRlowOffset = atoi(value);
        } else if (String(key) == "LDRhighOffset") {
            settingLDRhighOffset = atoi(value);
        } else if (String(key) == "maxIntensity") {
            settingMaxIntensity = atoi(value);
        } else if (String(key) == "weerliveAUTH") {
            settingWeerLiveAUTH = String(value).c_str();
        } else if (String(key) == "weerliveLocation") {
            settingWeerLiveLocation = String(value);
        } else if (String(key) == "weerliveInterval") {
            settingWeerLiveInterval = atoi(value);
        } else if (String(key) == "newsapiAUTH") {
            settingNewsAUTH = String(value);
        } else if (String(key) == "newsapiMaxMsg") {
            settingNewsMaxMsg = atoi(value);
        } else if (String(key) == "newsapiInterval") {
            settingNewsInterval = atoi(value);
        } else if (String(key) == "newsNoWords") {
            settingNewsNoWords = String(value);
        }
    }
    httpServer.send(200, "application/json", "{\"status\":\"ok\"}");
}

void sendApiNotFound(const char* URI) {
    DebugTf("==> URI[%s] not found ...\r\n", URI);
    httpServer.send(404, "application/json", "{\"status\":\"Not found\"}");
}
