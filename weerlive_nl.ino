/*
 * Opvragen actueel weer in Nederland
 */

void getWeerLiveData() {
  const char* weerliveHost    = "weerlive.nl";
  const int   httpPort        = 80;
  int         weerliveStatus  = 0;
  String      tempString;
  int         startPos, endPos;
  int32_t     maxWait;
  char        jsonResponse[1024];
  char        val[51] = "";
  
  WiFiClient weerliveClient;

  DebugTf("getWeerLiveData(%s)\r\n", weerliveHost);

  // We now create a URI for the request
  String url = "/api/json-data-10min.php?key=";
  url += settingWeerLiveAUTH;
  url += "&locatie=";
  url += settingWeerLiveLocation;

  DebugTf("Requesting URL: %s/api/json-data-10min.php?key=secret&locatie=%s\r\n", weerliveHost, settingWeerLiveLocation);
  //Debugln(url);
  if (!weerliveClient.connect(weerliveHost, httpPort)) {
    DebugTln("connection failed");
    sprintf(tempMessage, "connection to %s failed", weerliveHost);
    return;
  }

  // This will send the request to the server
  weerliveClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + weerliveHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  weerliveClient.setTimeout(5000);

  while (weerliveClient.connected())
  {
    yield();
    while(weerliveClient.available())
    {
      //--- skip to find HTTP/1.1
      //--- then parse response code
      if (weerliveClient.find("HTTP/1.1"))
      {
        weerliveStatus = weerliveClient.parseInt(); // parse status code
        Debugf("Statuscode: [%d] ", weerliveStatus); 
        if (weerliveStatus != 200)
        {
          Debugln(" ERROR!");
          weerliveClient.stop();
          return;  
        }
        Debugln(" OK!");
      }
      else
      {
        DebugTln("Error reading weerLive.. -> bailout!");
        return;
      }
      //--- skip headers
      if (weerliveClient.find("\r\n\r\n"))
      {
        weerliveClient.readBytesUntil('\0',  jsonResponse, sizeof(jsonResponse));
      }
    } // while available ..
    
  } // connected ..

  weerliveClient.stop();
  strTrimCntr(jsonResponse, 1023);
  DebugTf("\r\njsonResponse now holds[%s]\r\n", jsonResponse);
  
  // jsonResponse = "{ \"liveweer\": [{\"plaats\": \"Baarn\", \"temp\": \"9.8\", \"gtemp\": \"8.9\", \"samenv\": \"Licht bewolkt\", \"lv\": \"53\", \"windr\": \"NW\", \"windms\": \"2\", \"winds\": \"2\", \"windk\": \"3.9\", \"windkmh\": \"7.2\", \"luchtd\": \"1020.5\", \"ldmmhg\": \"765\", \"dauwp\": \"-0\", \"zicht\": \"35\", \"verw\": \"Perioden met zon en hier en daar een bui\", \"sup\": \"05:45\", \"sunder\": \"21:25\", \"image\": \"halfbewolkt\", \"d0weer\": \"halfbewolkt\", \"d0tmax\": \"13\", \"d0tmin\": \"2\", \"d0windk\": \"2\", \"d0windknp\": \"4\", \"d0windms\": \"2\", \"d0windkmh\": \"7\", \"d0windr\": \"ZW\", \"d0neerslag\": \"8\", \"d0zon\": \"53\", \"d1weer\": \"halfbewolkt\", \"d1tmax\": \"13\", \"d1tmin\": \"0\", \"d1windk\": \"2\", \"d1windknp\": \"6\", \"d1windms\": \"3\", \"d1windkmh\": \"11\", \"d1windr\": \"VAR\", \"d1neerslag\": \"40\", \"d1zon\": \"50\", \"d2weer\": \"halfbewolkt\", \"d2tmax\": \"12\", \"d2tmin\": \"5\", \"d2windk\": \"2\", \"d2windknp\": \"6\", \"d2windms\": \"3\", \"d2windkmh\": \"11\", \"d2windr\": \"N\", \"d2neerslag\": \"20\", \"d2zon\": \"50\", \"alarm\": 0}]}";                          

  parseJsonKey(jsonResponse, "plaats", val, 50);
  snprintf(tempMessage, LOCAL_SIZE, val);
  parseJsonKey(jsonResponse, "samenv", val, 50);
  snprintf(cMsg, LOCAL_SIZE, " %s %s ", tempMessage, val);
  parseJsonKey(jsonResponse, "d0tmin", val, 50);
  snprintf(tempMessage, LOCAL_SIZE, "%s min %s°C ", cMsg, val);
  parseJsonKey(jsonResponse, "d0tmax", val, 50);
  snprintf(cMsg, LOCAL_SIZE, "%s max %s°C", tempMessage, val);
  parseJsonKey(jsonResponse, "luchtd", val, 50);
  snprintf(tempMessage, LOCAL_SIZE, "%s  luchtdruk %s hPa ", cMsg, val);

  parseJsonKey(jsonResponse, "d1weer", val, 50);
  snprintf(cMsg, LOCAL_SIZE, "%s  morgen %s ", tempMessage, val);
  parseJsonKey(jsonResponse, "d1tmin", val, 50);
  snprintf(tempMessage, LOCAL_SIZE, "%s min %s°C ", cMsg, val);
  parseJsonKey(jsonResponse, "d1tmax", val, 50);
  snprintf(cMsg, LOCAL_SIZE, "%s max %s°C", tempMessage, val);

  snprintf(tempMessage, LOCAL_SIZE, "%s", cMsg);
  
  Debugln(tempMessage);
  
} // getWeerLiveData()

// eof
