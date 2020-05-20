/*
 *  spiffStuff 
 */

//#include <FS.h>

//====================================================================
void readLastStatus()
{
  char buffer[50] = "";
  char dummy[50] = "";
  
  File _file = SPIFFS.open("/sysStatus.csv", "r");
  if (!_file)
  {
    DebugTln("read(): No /sysStatus.csv found ..");
  }
  if(_file.available()) {
    int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    buffer[l] = 0;
    DebugTf("read lastUpdate[%s]\r\n", buffer);
    sscanf(buffer, "%[^;]; %[^;]; %u; %[^;]", cDate, cTime, &nrReboots, dummy);
    DebugTf("values timestamp[%s %s], nrReboots[%u], dummy[%s]\r\n"
                                                    , cDate
                                                    , cTime
                                                    , nrReboots
                                                    , dummy);
    yield();
  }
  _file.close();
  
}  // readLastStatus()


//====================================================================
void writeLastStatus()
{
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    return;
  }
  char buffer[50] = "";
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %010u; %s;\n"
                                          , year(), month(), day()
                                          , hour(), minute(), second()
                                          , nrReboots
                                          , "meta data");
  DebugTf("writeLastStatus() => %s\r\n", buffer);

  File _file = SPIFFS.open("/sysStatus.csv", "w");
  if (!_file)
  {
    DebugTln("write(): No /sysStatus.csv found ..");
  }
  _file.print(buffer);
  _file.flush();
  _file.close();
  
} // writeLastStatus()


//------------------------------------------------------------------------
bool readFileById(const char* fType, uint8_t mId)
{
  String rTmp;
  char fName[50] = "";
  sprintf(fName, "/%s-%03d", fType, mId);

  DebugTf("read [%s] ", fName);
  
  if (!SPIFFS.exists(fName)) 
  {
    Debugln("Does not exist!");
    return false;
  }

  File f = SPIFFS.open(fName, "r");

  while(f.available()) {
    rTmp = f.readStringUntil('\n');
    rTmp.replace("\r", "");
  }
  f.close();

  snprintf(fileMessage, LOCAL_SIZE, rTmp.c_str());
  if (strlen(fileMessage) == 0)
  {
    Debugln("file is zero bytes long");
    return false;
  }
  Debugf("OK! [%s]\r\n", fileMessage);
  return true;
  
} // readFileById()

//------------------------------------------------------------------------
bool writeFileById(const char* fType, uint8_t mId, const char *msg)
{
  String rTmp;
  char fName[50] = "";
  sprintf(fName, "/%s-%03d", fType, mId);

  DebugTf("write [%s] ", fName);

  File file = SPIFFS.open(fName, "w");
  if (!file) 
  {
    Debugf("open(%s, 'w') FAILED!!! --> Bailout\r\n", fName);
    return false;
  }
  yield();

  Debugln(F("Start writing data .. \r"));
  file.println(msg);
  file.close();

  return true;
  
} // writeFileById()


//=======================================================================
void updateMessage(const char *field, const char *newValue)
{
  int8_t msgId = String(field).toInt();
  
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (msgId < 0 || msgId > settingLocalMaxMsg)
  {
    DebugTf("msgId[%d] is to large! Bailing out!\r\n", msgId);
    return;
  }

  writeFileById("LCL", msgId, newValue);
  
} // updateMessage()


//====================================================================
void writeToLog(const char *logLine)
{
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    return;
  }
  char buffer[150] = "";
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %s;\n"
                                          , year(), month(), day()
                                          , hour(), minute(), second()
                                          , logLine);
  DebugTf("writeToLogs() => %s\r\n", buffer);

  File _file = SPIFFS.open("/sysLog.csv", "a");
  if (!_file)
  {
    DebugTln("write(): No /sysLog.csv found ..");
  }
  _file.print(buffer);
  _file.flush();
  _file.close();
  
} // writeLastStatus()

 
