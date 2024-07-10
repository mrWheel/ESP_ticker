/* 
***************************************************************************  
**  Program : FSexplorer
**  Version : 2.0   10-05-202
**
**  Mostly stolen from https://www.arduinoforum.de/User-Fips
**  For more information visit: https://fipsok.de
**  See also https://www.arduinoforum.de/arduino-Thread-LittleFS-DOWNLOAD-UPLOAD-DELETE-Esp8266-NodeMCU
**
***************************************************************************      
  Copyright (c) 2018 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************
**      Usage:
**      
**      setup()
**      {
**        setupFSexplorer();
**        httpServer.serveStatic("/FSexplorer.png",   LittleFS, "/FSexplorer.png");
**        httpServer.on("/",          sendIndexPage);
**        httpServer.on("/index",     sendIndexPage);
**        httpServer.on("/index.html",sendIndexPage);
**        httpServer.begin();
**      }
**      
**      loop()
**      {
**        httpServer.handleClient();
**        .
**        .
**      }
*/

#include "FSexplorer.h"

#define MAX_FILES_IN_LIST   25

const char Helper[] = R"(
  <br>You first need to upload these two files:
  <ul>
    <li>FSexplorer.html</li>
    <li>FSexplorer.css</li>
  </ul>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
  <br/><b>or</b> you can use the <i>Flash Utility</i> to flash firmware or LittleFS!
  <form action='/update' method='GET'>
    <input type='submit' name='SUBMIT' value='Flash Utility'/>
  </form>
)";
const char Header[] = "HTTP/1.1 303 OK\r\nLocation:FSexplorer.html\r\nCache-Control: no-cache\r\n";

//=====================================================================================
void setupFSexplorer()    // Funktionsaufruf "spiffs();" muss im Setup eingebunden werden
{    
  LittleFS.begin();
  
  if (LittleFS.exists("/FSexplorer.html")) 
  {
    httpServer.serveStatic("/FSexplorer.html", LittleFS, "/FSexplorer.html");
    httpServer.serveStatic("/FSexplorer",      LittleFS, "/FSexplorer.html");
  }
  else 
  {
    httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  }
  httpServer.on("/api/listfiles", APIlistFiles);
  httpServer.on("/LittleFSformat", formatLittleFS);
  httpServer.on("/upload", HTTP_POST, []() {}, handleFileUpload);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/update", updateFirmware);
  httpServer.onNotFound([]() 
  {
    if (Verbose) Serial.printf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());
    TelnetStream.printf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());
    if (httpServer.uri().indexOf("/api/") == 0) 
    {
      if (Verbose) Serial.printf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      TelnetStream.printf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      processAPI();
    }
    else if (httpServer.uri() == "/")
    {
      Serial.println("index requested..");
      TelnetStream.println("index requested..");
      sendIndexPage();
    }
    else
    {
      Serial.printf("next: handleFile(%s)\r\n"
                      , String(httpServer.urlDecode(httpServer.uri())).c_str());
      TelnetStream.printf("next: handleFile(%s)\r\n"
                      , String(httpServer.urlDecode(httpServer.uri())).c_str());
      if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      {
        Serial.println("File not found");
        TelnetStream.println("File not found");
        httpServer.send(404, "text/plain", "FileNotFound\r\n");
      }
    }
  });
  
} // setupFSexplorer()

//=====================================================================================
const String formatBytes(size_t const& bytes) 
{ 
  return (bytes < 1024) ? String(bytes) + " Byte" : (bytes < (1024 * 1024)) ? String(bytes / 1024.0) + " KB" : String(bytes / 1024.0 / 1024.0) + " MB";

} //formatBytes()


//=====================================================================================
void APIlistFiles()             // Senden aller Daten an den Client
{   
  FSInfo LittleFSinfo;

  typedef struct _fileMeta {
    char    Name[30];     
    int32_t Size;
  } fileMeta;

  _fileMeta dirMap[MAX_FILES_IN_LIST+1];
  int fileNr = 0;
  
  Dir dir = LittleFS.openDir("/");         // List files on LittleFS
  while (dir.next() && (fileNr < MAX_FILES_IN_LIST))  
  {
    dirMap[fileNr].Name[0] = '\0';
  //strncat(dirMap[fileNr].Name, dir.fileName().substring(1).c_str(), 29); // remove leading '/'
    strncat(dirMap[fileNr].Name, dir.fileName().c_str(), 29); 
    dirMap[fileNr].Size = dir.fileSize();
    //--- Skip files in newsFiles map
    //-aaw-if ((dir.fileName().indexOf("/newsFiles") == -1) && (dir.fileSize() > 0))
    //{
      fileNr++;
    //}
  }
  Serial.printf("fileNr[%d], Max[%d]\r\n", fileNr, MAX_FILES_IN_LIST);

  // -- bubble sort dirMap op .Name--
  for (int8_t y = 0; y < fileNr; y++) {
    yield();
    for (int8_t x = y + 1; x < fileNr; x++)  {
      //Serial.printf("y[%d], x[%d] => seq[y][%s] / seq[x][%s] ", y, x, dirMap[y].Name, dirMap[x].Name);
      if (compare(String(dirMap[x].Name), String(dirMap[y].Name)))  
      {
        //Serial.print(" !switch!");
        fileMeta temp = dirMap[y];
        dirMap[y] = dirMap[x];
        dirMap[x] = temp;
      } /* end if */
      //Serial.println();
    } /* end for */
  } /* end for */
  
  if (fileNr >= MAX_FILES_IN_LIST)
  {
    fileNr = MAX_FILES_IN_LIST;
    dirMap[fileNr].Name[0] = '\0';
    //--- if you change this message you also have to 
    //--- change FSexplorer.html
    strncat(dirMap[fileNr].Name, "More files not listed ..", 29); 
    dirMap[fileNr].Size = 0;
    fileNr++;
  }

  String temp = "[";
  for (int f=0; f < fileNr; f++)  
  {
    Serial.printf("[%3d] >> [%s]\r\n", f, dirMap[f].Name);
    if (temp != "[") temp += ",";
    temp += R"({"name":")" + String(dirMap[f].Name) + R"(","size":")" + formatBytes(dirMap[f].Size) + R"("})";
  }

  LittleFS.info(LittleFSinfo);
  temp += R"(,{"usedBytes":")" + formatBytes(LittleFSinfo.usedBytes * 1.05) + R"(",)" +       // Berechnet den verwendeten Speicherplatz + 5% Sicherheitsaufschlag
          R"("totalBytes":")" + formatBytes(LittleFSinfo.totalBytes) + R"(","freeBytes":")" + // Zeigt die Größe des Speichers
          (LittleFSinfo.totalBytes - (LittleFSinfo.usedBytes * 1.05)) + R"("}])";               // Berechnet den freien Speicherplatz + 5% Sicherheitsaufschlag

  httpServer.send(200, "application/json", temp);
  
} // APIlistFiles()


//=====================================================================================
bool handleFile(String&& path) 
{
  if (httpServer.hasArg("delete")) 
  {
    Serial.printf("Delete -> [%s]\n\r",  httpServer.arg("delete").c_str());
    LittleFS.remove(httpServer.arg("delete"));    // Datei löschen
    httpServer.sendContent(Header);
    return true;
  }
  if (!LittleFS.exists("/FSexplorer.html")) httpServer.send(200, "text/html", Helper); //Upload the FSexplorer.html
  if (path.endsWith("/")) path += "index.html";
  return LittleFS.exists(path) ? ({File f = LittleFS.open(path, "r"); httpServer.streamFile(f, contentType(path)); f.close(); true;}) : false;

} // handleFile()


//=====================================================================================
void handleFileUpload() 
{
  static File fsUploadFile;
  HTTPUpload& upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START) 
  {
    if (upload.filename.length() > 30) 
    {
      upload.filename = upload.filename.substring(upload.filename.length() - 30, upload.filename.length());  // Dateinamen auf 30 Zeichen kürzen
    }
    Serial.println("FileUpload Name: " + upload.filename);
    fsUploadFile = LittleFS.open("/" + httpServer.urlDecode(upload.filename), "w");
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) 
  {
    Serial.println("FileUpload Data: " + (String)upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if (upload.status == UPLOAD_FILE_END) 
  {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.println("FileUpload Size: " + (String)upload.totalSize);
    httpServer.sendContent(Header);
  }
  
} // handleFileUpload() 


//=====================================================================================
void formatLittleFS() 
{       //Formatiert den Speicher
  if (!LittleFS.exists("/!format")) return;
  Serial.println(F("Format LittleFS"));
  LittleFS.format();
  httpServer.sendContent(Header);
  
} // formatLittleFS()

//=====================================================================================
const String &contentType(String& filename) 
{       
  if (filename.endsWith(".htm") || filename.endsWith(".html")) filename = "text/html";
  else if (filename.endsWith(".css")) filename = "text/css";
  else if (filename.endsWith(".js")) filename = "application/javascript";
  else if (filename.endsWith(".json")) filename = "application/json";
  else if (filename.endsWith(".png")) filename = "image/png";
  else if (filename.endsWith(".gif")) filename = "image/gif";
  else if (filename.endsWith(".jpg")) filename = "image/jpeg";
  else if (filename.endsWith(".ico")) filename = "image/x-icon";
  else if (filename.endsWith(".xml")) filename = "text/xml";
  else if (filename.endsWith(".pdf")) filename = "application/x-pdf";
  else if (filename.endsWith(".zip")) filename = "application/x-zip";
  else if (filename.endsWith(".gz")) filename = "application/x-gzip";
  else filename = "text/plain";
  return filename;
  
} // &contentType()

//=====================================================================================
bool freeSpace(uint16_t const& printsize) 
{    
  FSInfo LittleFSinfo;
  LittleFS.info(LittleFSinfo);
  Serial.println(formatBytes(LittleFSinfo.totalBytes - (LittleFSinfo.usedBytes * 1.05)) + " im Spiffs frei");
  return (LittleFSinfo.totalBytes - (LittleFSinfo.usedBytes * 1.05) > printsize) ? true : false;
  
} // freeSpace()


//=====================================================================================
void updateFirmware()
{
  Serial.println(F("Redirect to updateIndex .."));
  doRedirect("wait ... ", 1, "/updateIndex", false);
      
} // updateFirmware()

//=====================================================================================
void reBootESP()
{
  Serial.println(F("Redirect and ReBoot .."));
  doRedirect("Reboot ESP - lichtKrant ..", 60, "/", true);
      
} // reBootESP()

//=====================================================================================
void doRedirect(const char *msg, int wait, const char* URL, bool reboot)
{
  String redirectHTML = 
  "<!DOCTYPE HTML><html lang='en-US'>"
  "<head>"
  "<meta charset='UTF-8'>"
  "<style type='text/css'>"
  "body {background-color: lightblue;}"
  "</style>"
  "<title>Redirect to Main Program</title>"
  "</head>"
  "<body><h1>FSexplorer</h1>"
  "<h3>"+String(msg)+"</h3>"
  "<br><div style='width: 500px; position: relative; font-size: 25px;'>"
  "  <div style='float: left;'>Redirect over &nbsp;</div>"
  "  <div style='float: left;' id='counter'>"+String(wait)+"</div>"
  "  <div style='float: left;'>&nbsp; seconden ...</div>"
  "  <div style='float: right;'>&nbsp;</div>"
  "</div>"
  "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->"
  "<br><br><hr>If you are not redirected automatically, click this <a href='/'>Main Program</a>."
  "  <script>"
  "      setInterval(function() {"
  "          var div = document.querySelector('#counter');"
  "          var count = div.textContent * 1 - 1;"
  "          div.textContent = count;"
  "          if (count <= 0) {"
  "              window.location.replace('"+String(URL)+"'); "
  "          } "
  "      }, 1000); "
  "  </script> "
  "</body></html>\r\n";
  
  Serial.println(msg);
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot) 
  {
    delay(5000);
    ESP.restart();
    delay(5000);
  }
  
} // doRedirect()