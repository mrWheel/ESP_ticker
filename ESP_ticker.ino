
#define _FW_VERSION "v1.3.1 (16-05-2020)"


// Use the Parola library to scroll text on the display
// IP address for the ESP8266 is displayed on the scrolling display
// after startup initialisation and connected to the WiFi network.
//
// Connections for ESP8266 hardware SPI are:
// Vcc       3v3     LED matrices seem to work at 3.3V
// GND       GND     GND
// DIN        D7     HSPID or HMOSI
// CS or LD   D8     HSPICS or HCS
// CLK        D5     CLK or HCLK
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#define _HOSTNAME   "ESPticker"
#include "ESP_ticker.h"

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// WiFi Server object and parameters
WiFiServer server(80);

// Scrolling parameters

uint8_t  inFX, outFX;
textEffect_t  effect[] =
{
  PA_PRINT,
//PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_MESH,
  PA_SCROLL_UP_RIGHT,
//PA_BLINDS,
  PA_CLOSING,
  PA_RANDOM,
  PA_GROW_DOWN,
  PA_SCAN_VERT,
  PA_SCROLL_DOWN_LEFT,
  PA_WIPE_CURSOR,
//PA_DISSOLVE,
  PA_OPENING,
  PA_CLOSING_CURSOR,
  PA_SCROLL_DOWN_RIGHT,
  PA_SCROLL_RIGHT,
//PA_SLICE,
  PA_SCROLL_DOWN,
};


//---------------------------------------------------------------------
int16_t calculateIntensity()
{
  int aIn = 0;
  for (int l=0; l<2; l++)
  {
    //--- read analog A0
    aIn+= analogRead(A0);
    delay(200);
  }
  aIn = aIn / 2;  //-- smooth things up a bit
  DebugTf("analogRead[%d], ", aIn);
  if (aIn < settingLDRoffset) aIn = settingLDRoffset;
  Debugf(" LDRoffset corrected[%d], ", aIn);
  valueLDR = (valueLDR + aIn) / 2;
  Debugf(" ==> valueLDR[%d]\r\n", valueLDR);

  //--- map LDR to offset..1024 -> 0..settingMax
  int intensity = map(valueLDR, settingLDRoffset, 1024, 0, settingMaxIntensity);
  DebugTf("map(%d, %d, %d, 0, %d) => [%d]\r\n", valueLDR, settingLDRoffset, 1024, settingMaxIntensity, intensity);
  return intensity;

} // calculateIntensity()


//---------------------------------------------------------------------
char *updateTime()
{
  snprintf(timeMsg, 20, "%02d : %02d", hour(), minute()); 
  return timeMsg;

} // updateTime()


//---------------------------------------------------------------------
void splitNewsNoWords(const char *noNo)
{
  DebugTln(noNo);
  int8_t wc = splitString(String(noNo), ' ', noWords, MAX_NO_NO_WORDS);
  for(int8_t i=0; i<wc; i++)
  {
    noWords[i].trim();
    if (noWords[i].length() > 1)
    {
      noWords[i].toLowerCase();
      DebugTf("NoNoWord[%d] [%s]\r\n", i, noWords[i].c_str());
    }
  }
  
} // splitNewsNoWords()

//---------------------------------------------------------------------
bool hasNoNoWord(const char *cIn)
{
  for(int8_t i=0; i<MAX_NO_NO_WORDS; i++)
  {
    String sIn = String(cIn);
    sIn.toLowerCase();
    int idx = sIn.indexOf(noWords[i]);
    if ((idx > -1) && (noWords[i].length() > 1))  // yes! it's in there somewhere
    {
      DebugTf("found [%s]\r\n", noWords[i].c_str());
      return true;      
    }
  }
  //DebugTln("no NoNo words found!");
  return false;
  
} // hasNoNoWord()


//---------------------------------------------------------------------
void nextNieuwsBericht()
{
  newsMsgID++;
  if (newsMsgID > settingNewsMaxMsg) newsMsgID = 0;
  while (!readFileById("NWS", newsMsgID))
  {
    DebugTln("File not found!");
    newsMsgID++;
    if (newsMsgID > settingNewsMaxMsg) newsMsgID = 0;
  }
  snprintf(actMessage, NEWS_SIZE, "* %s **", fileMessage);
  //DebugTf("newsMsgID[%d] %s\r\n", newsMsgID, actMessage);
  utf8Ascii(actMessage);
  P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));
  
} // nextNieuwsBericht()


//---------------------------------------------------------------------
void nextLocalBericht()
{
  localMsgID++;
  if (localMsgID > (settingLocalMaxMsg -1)) localMsgID = 0;
  while (!readFileById("LCL", localMsgID))
  {
    DebugTln("File not found!");
    localMsgID++;
    if (localMsgID > (settingLocalMaxMsg -1)) localMsgID = 0;
  }
  snprintf(actMessage, LOCAL_SIZE, "* %s **", fileMessage);
  //DebugTf("localMsgID[%d] %s\r\n", localMsgID, actMessage);
  utf8Ascii(actMessage);
  P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));
  
} // nextLocalBericht()


//=====================================================================
void setup()
{
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  lastReset     = ESP.getResetReason();

  startTelnet();
  
  DebugTln("\r\n[MD_Parola WiFi Message Display]\r\n");
  DebugTf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());
  
  P.begin();
  P.displayClear();
  P.displaySuspend(false);
  P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));

  actMessage[0]  = '\0';
  valueIntensity = calculateIntensity(); // read analog input pin 0
  P.setIntensity(valueIntensity);

  
//================ SPIFFS ===========================================
  if (SPIFFS.begin()) 
  {
    DebugTln(F("SPIFFS Mount succesfull\r"));
    SPIFFSmounted = true;
  } else { 
    DebugTln(F("SPIFFS Mount failed\r"));   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
  }

  readSettings(true);
  splitNewsNoWords(settingNewsNoWords);

  // attempt to connect to Wifi network:
  int t = 0;
  while ((WiFi.status() != WL_CONNECTED) && (t < 25))
  {
    delay(100);
    Serial.print(".");
    t++;
  }
  if ( WiFi.status() != WL_CONNECTED) {
    DebugTln("Attempting to connect to WiFi network\r");
    sprintf(actMessage, "Connect to AP '%s' and configure WiFi on  192.168.4.1   ", _HOSTNAME);
    P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
    do { yield(); } while( !P.displayAnimate() );
    //P.print("   192.168.4.1");
  }
  // Connect to and initialise WiFi network
  digitalWrite(LED_BUILTIN, HIGH);
  startWiFi(_HOSTNAME, 240);  // timeout 4 minuten
  digitalWrite(LED_BUILTIN, LOW);

  startMDNS(settingHostname);
  startNTP();

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ESP.getResetReason().c_str());
  DebugTln(cMsg);

  Serial.print("\nGebruik 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' voor verdere debugging\r\n");

//================ Start HTTP Server ================================
  setupFSexplorer();
  httpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");
  httpServer.on("/",          sendIndexPage);
  httpServer.on("/index",     sendIndexPage);
  httpServer.on("/index.html",sendIndexPage);
  httpServer.serveStatic("/index.css", SPIFFS, "/index.css");
  httpServer.serveStatic("/index.js",  SPIFFS, "/index.js");
  // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.on("/api", HTTP_GET, processAPI);


  httpServer.begin();
  DebugTln("\nServer started\r");
  
  // Set up first message as the IP address
  sprintf(actMessage, "%03d.%03d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  DebugTf("\nAssigned IP[%s]\r\n", actMessage);
  P.displayScroll(actMessage, PA_LEFT, PA_NO_EFFECT, (MAX_SPEED - settingTextSpeed));
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  valueIntensity = calculateIntensity(); // read analog input pin 0
  P.setIntensity(valueIntensity);
  newsMsgID = 0;
  do { yield(); } while( !P.displayAnimate() );

  P.setFont(ExtASCII);

  inFX = 0;
  outFX= 0;
  
} // setup()


//=====================================================================
void loop()
{
  handleNTP();
  httpServer.handleClient();
  MDNS.update();
  if ((millis() > weerTimer) && (strlen(settingWeerLiveAUTH) > 5))
  {
    weerTimer = millis() + (settingWeerLiveInterval * (60 * 1000)); // Interval in Minutes!
    getWeerLiveData();
  }
  if ((millis() > newsapiTimer) && (strlen(settingNewsAUTH) > 5))
  {
    newsapiTimer = millis() + (settingNewsInterval * (60 * 1000)); // Interval in Minutes!
    getNewsapiData();
  }

  if (P.displayAnimate()) // done with animation, ready for next message
  {
    msgType++;
    DebugTf("msgType[%d]\r\n", msgType);
    
    switch(msgType)
    {
      case 1:   if (!(millis() > timeTimer))  return;
                inFX  = random(0, ARRAY_SIZE(effect));
                outFX = random(0, ARRAY_SIZE(effect));
                snprintf(actMessage, LOCAL_SIZE, weekDayName[weekday()]);
                P.displayText(actMessage, PA_CENTER, (MAX_SPEED - settingTextSpeed), 1000, effect[inFX], effect[outFX]);
                DebugTf("Animate IN[%d], OUT[%d] %s\r\n", inFX, outFX, actMessage);
                break;
      case 2:   if (!(millis() > timeTimer))  return;
                timeTimer = millis() + 60000;
                inFX  = random(0, ARRAY_SIZE(effect));
                outFX = random(0, ARRAY_SIZE(effect));
                sprintf(actMessage, "%s", updateTime());
                P.displayText(actMessage, PA_CENTER, (MAX_SPEED - settingTextSpeed), 2000, effect[inFX], effect[outFX]);
                DebugTf("Animate IN[%d], OUT[%d] %s\r\n", inFX, outFX, actMessage);
                break;
      case 3:
      case 5:   nextLocalBericht();
                P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
                break;    
      case 4:            
      case 6:            
      case 7:   nextNieuwsBericht();
                P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
                break;
      case 8:   snprintf(actMessage, LOCAL_SIZE, "* %s **", tempMessage);
                utf8Ascii(actMessage);
                P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
                break;
      case 9:   nextNieuwsBericht();
                break;
      default:  msgType = 0;
                return;
                
    } // switch()

    //DebugTln(actMessage);
    valueIntensity = calculateIntensity(); // read analog input pin 0
    DebugTf("Intensity set to [%d]\r\n", valueIntensity);
    P.setIntensity(valueIntensity);
    // Tell Parola we have a new animation
    P.displayReset();

  } // dislayAnimate()

  
} // loop()
