
//#include <ESP8266WiFi.h>

#include <Timezone.h>           // https://github.com/JChristensen/Timezone
#include <TimeLib.h>            // https://github.com/PaulStoffregen/Time
#include <TelnetStream.h>       // https://github.com/jandrassy/TelnetStream/commit/1294a9ee5cc9b1f7e51005091e351d60c8cddecf
#include "Debug.h"
#include "networkStuff.h"

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "parola_Fonts_data.h"
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers are for ESP8266 hardware SPI and will probably not
// work with your hardware and may need to be adapted
//#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES  8
#define MAX_SPEED   50

//#define CLK_PIN   14 // or SCK
//#define DATA_PIN  13 // or MOSI
#define CS_PIN      15 // or SS

#define SETTINGS_FILE   "/settings.ini"
#define LOCAL_SIZE      255
#define NEWS_SIZE       512
#define JSON_BUFF_MAX   255
#define MAX_NO_NO_WORDS  20

bool      Verbose = false;
char      cDate[15], cTime[10];
uint32_t  nrReboots;
// Global message buffers shared by Wifi and Scrolling functions
char      cMsg[NEWS_SIZE];
char      tempMessage[LOCAL_SIZE] = "";
uint8_t   msgType;
char      actMessage[NEWS_SIZE], timeMsg[20];
char      fileMessage[LOCAL_SIZE];
uint8_t   newsMsgID   = 0;
uint8_t   localMsgID  = 0;
int16_t   valueLDR, valueIntensity;
char      fChar[10];
String    lastReset   = "";
uint32_t  timeTimer   = 0;
uint32_t  ntpTimer    = millis() + 30000;
uint32_t  weerTimer   = 0;
uint32_t  newsapiTimer = 0;
String    noWords[MAX_NO_NO_WORDS+1];
char      settingHostname[41];
char      settingNewsNoWords[LOCAL_SIZE];
uint8_t   settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;
uint16_t  settingLDRlowOffset, settingLDRhighOffset;
char      settingWeerLiveAUTH[51], settingWeerLiveLocation[51];
uint8_t   settingWeerLiveInterval;
char      settingNewsAUTH[51];
uint8_t   settingNewsInterval, settingNewsMaxMsg;

const char *weekDayName[]  {  "Unknown", "Zondag", "Maandag", "Dinsdag", "Woensdag"
                            , "Donderdag", "Vrijdag", "Zaterdag", "Unknown" };
const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "Unknown" };

// eof
