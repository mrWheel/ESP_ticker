/* 
***************************************************************************  
**  Program  : allDefines.h, part of ESP_ticker
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#ifndef _ALLDEFINES_H
#define  _ALLDEFINES_H

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
#define HOSTNAME_SIZE    40
#define LOCAL_SIZE      255
#define NEWS_SIZE       512
#define JSON_BUFF_MAX   255
#define MAX_NO_NO_WORDS  20

// Define the maximum buffer size
#define JSON_BUFF_MAX 200



#endif


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