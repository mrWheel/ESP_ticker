/*
 *  spiffStuff 
 */

//#include <FS.h>

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

 
