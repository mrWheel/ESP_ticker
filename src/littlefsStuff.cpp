#include "littlefsStuff.h"

char cDate[11];
char cTime[9];
uint32_t nrReboots;
char fileMessage[LOCAL_SIZE];

void readLastStatus() {
    char buffer[50] = "";
    char dummy[50] = "";
    
    File _file = LittleFS.open("/sysStatus.csv", "r");
    if (!_file) {
        DebugTln("read(): No /sysStatus.csv found ..");
    }
    if (_file.available()) {
        int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[l] = 0;
        DebugTf("read lastUpdate[%s]\r\n", buffer);
        sscanf(buffer, "%[^;]; %[^;]; %u; %[^;]", cDate, cTime, &nrReboots, dummy);
        DebugTf("values timestamp[%s %s], nrReboots[%u], dummy[%s]\r\n", cDate, cTime, nrReboots, dummy);
        yield();
    }
    _file.close();
}

void writeLastStatus() {
    if (ESP.getFreeHeap() < 8500) {
        DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
        return;
    }
    char buffer[50] = "";
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %010u; %s;\n",
             year(), month(), day(), hour(), minute(), second(), nrReboots, "meta data");
    DebugTf("writeLastStatus() => %s\r\n", buffer);

    File _file = LittleFS.open("/sysStatus.csv", "w");
    if (!_file) {
        DebugTln("write(): No /sysStatus.csv found ..");
    }
    _file.print(buffer);
    _file.flush();
    _file.close();
}

bool readFileById(const char* fType, uint8_t mId) {
    String percChar = "%%";
    String backSlash = "\\";
    String rTmp;
    char fName[50] = "";
    
    sprintf(fName, "/newsFiles/%s-%03d", fType, mId);
    DebugTf("read [%s] ", fName);
    
    if (!LittleFS.exists(fName)) {
        Debugln("Does not exist!");
        return false;
    }

    File f = LittleFS.open(fName, "r");
    while (f.available()) {
        rTmp = f.readStringUntil('\n');
        rTmp.replace("\r", "");
    }
    f.close();

    rTmp.replace("@1@", ":");
    rTmp.replace("@2@", "{");
    rTmp.replace("@3@", "}");
    rTmp.replace("@4@", ",");
    rTmp.replace("@5@", backSlash);
    rTmp.replace("@6@", percChar);
    
    snprintf(fileMessage, LOCAL_SIZE, rTmp.c_str());
    if (strlen(fileMessage) == 0) {
        Debugln("file is zero bytes long");
        return false;
    }
    Debugf("OK! \r\n\t[%s]\r\n", fileMessage);

    if (mId == 0) {
        LittleFS.remove("/newsFiles/LCL-000");
        DebugTln("Remove LCL-000 ..");
    }

    return true;
}

bool writeFileById(const char* fType, uint8_t mId, const char *msg) {
    char fName[50] = "";
    sprintf(fName, "/newsFiles/%s-%03d", fType, mId);

    DebugTf("write [%s]-> [%s]\r\n", fName, msg);

    if (strlen(msg) < 3) {
        LittleFS.remove(fName);
        Debugln("Empty message, file removed!");
        return true;
    }

    DebugTln("LittleFS.open()...");
    File file = LittleFS.open(fName, "w");
    if (!file) {
        Debugf("open(%s, 'w') FAILED!!! --> Bailout\r\n", fName);
        return false;
    }
    yield();

    Debugln(F("Start writing data .. \r"));
    DebugFlush();
    Debugln(msg);
    file.println(msg);
    file.close();

    DebugTln("Exit writeFileById()!");
    return true;
}

void updateMessage(const char *field, const char *newValue) {
    int8_t msgId = String(field).toInt();
    
    DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

    if (msgId < 0 || msgId > settingLocalMaxMsg) {
        DebugTf("msgId[%d] is out of scope! Bailing out!\r\n", msgId);
        return;
    }

    writeFileById("LCL", msgId, newValue);
}

void writeToLog(const char *logLine) {
    if (ESP.getFreeHeap() < 8500) {
        DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
        return;
    }
    char buffer[150] = "";
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %s;\n",
             year(), month(), day(), hour(), minute(), second(), logLine);
    DebugTf("writeToLogs() => %s\r\n", buffer);
    File _file = LittleFS.open("/sysLog.csv", "a");
    if (!_file) {
        DebugTln("write(): No /sysLog.csv found ..");
    }
    _file.print(buffer);
    _file.flush();
    _file.close();
}
