#include "jsonStuff.h"

// Define external global variables
char objSprtr[10] = "";
ESP8266WebServer httpServer(80);

void sendStartJsonObj(const char *objName) {
    char sBuff[50] = "";
    objSprtr[0] = '\0';
    snprintf(sBuff, sizeof(sBuff), "{\"%s\":[\r\n", objName);
    httpServer.sendHeader("Access-Control-Allow-Origin", "*");
    httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    httpServer.send(200, "application/json", sBuff);
}

void sendEndJsonObj() {
    httpServer.sendContent("\r\n]}\r\n");
}

void sendNestedJsonObj(const char *cName, const char *cValue) {
    char jsonBuff[JSON_BUFF_MAX] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}", objSprtr, cName, cValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendNestedJsonObj(const char *cName, String sValue) {
    char jsonBuff[JSON_BUFF_MAX] = "";
    if (sValue.length() > (JSON_BUFF_MAX - 65)) {
        Serial.printf("[2] sValue.length() [%d]\r\n", sValue.length());
    }
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}", objSprtr, cName, sValue.c_str());
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendNestedJsonObj(const char *cName, int32_t iValue) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d}", objSprtr, cName, iValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendNestedJsonObj(const char *cName, uint32_t uValue) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u}", objSprtr, cName, uValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendNestedJsonObj(const char *cName, float fValue) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f}", objSprtr, cName, fValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
             objSprtr, cName, fValue, fType, minValue, maxValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces) {
    char jsonBuff[200] = "";
    switch (decPlaces) {
        case 0:
            snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.0f, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
                     objSprtr, cName, fValue, fType, minValue, maxValue);
            break;
        case 2:
            snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.2f, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
                     objSprtr, cName, fValue, fType, minValue, maxValue);
            break;
        case 5:
            snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.5f, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
                     objSprtr, cName, fValue, fType, minValue, maxValue);
            break;
        default:
            snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %f, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
                     objSprtr, cName, fValue, fType, minValue, maxValue);
    }
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d, \"type\": \"%s\", \"min\": %d, \"max\": %d}",
             objSprtr, cName, iValue, iType, minValue, maxValue);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}

void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen) {
    char jsonBuff[200] = "";
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\":\"%s\", \"type\": \"%s\", \"maxlen\": %d}",
             objSprtr, cName, cValue, sType, maxLen);
    httpServer.sendContent(jsonBuff);
    sprintf(objSprtr, ",\r\n");
}
