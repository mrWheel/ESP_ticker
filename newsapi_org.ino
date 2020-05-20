

// http://newsapi.org/v2/top-headlines?country=nl&apiKey=API_KEY
// apiKey: e04bbc17756d4884bae3bfcfd022344c

/*
 * Opvragen actueel weer in Nederland
 */

void getNewsapiData() {
  const char* newsapiHost    = "newsapi.org";
  const int   httpPort        = 80;
  int         newsapiStatus  = 0;
  char        newsMessage[NEWS_SIZE];
  int         startPos, endPos;
  int32_t     maxWait;
  char        jsonResponse[1024];
  char        val[51] = "";
  
  WiFiClient newsapiClient;

  DebugTf("getNewsapiData(%s)\r\n", newsapiHost);

  // We now create a URI for the request
  String url = "/v2/top-headlines?country=nl&apiKey=";
  url += settingNewsAUTH;

  DebugTf("Requesting URL: %s/v2/top-headlines?country=nl&key=secret\r\n", newsapiHost);

  if (!newsapiClient.connect(newsapiHost, httpPort)) {
    DebugTln("connection failed");
    sprintf(tempMessage, "connection to %s failed", newsapiHost);
    return;
  }

  // This will send the request to the server
  newsapiClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + newsapiHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  newsapiClient.setTimeout(5000);
  while (newsapiClient.connected() || newsapiClient.available())
  {
    yield();
    while(newsapiClient.available())
    {
      //--- skip to find HTTP/1.1
      //--- then parse response code
      if (newsapiClient.find("HTTP/1.1"))
      {
        newsapiStatus = newsapiClient.parseInt(); // parse status code
        Debugf("Statuscode: [%d] ", newsapiStatus); 
        if (newsapiStatus != 200)
        {
          Debugln(" ERROR!");
          newsapiClient.stop();
          return;  
        }
        Debugln(" OK!");
      }
      else
      {
        DebugTln("Error reading newsapi.org.. -> bailout!");
        return;
      }
      //--- skip headers
      uint16_t  msgIdx  = 0;
      int       msgNr   = 0;
      while (newsapiClient.find("\"title\":\""))
      {
        for (int i=0; i<NEWS_SIZE; i++) newsMessage[i] = 0;
        msgIdx          = 0;
        while( (strIndex(newsMessage, "\"description\":") == -1) && (msgIdx < (NEWS_SIZE - 2)) )
        {
          newsMessage[msgIdx++] = (char)newsapiClient.read();
        }
        newsMessage[msgIdx] = '\0';
        if (msgIdx > 30)  newsMessage[msgIdx - 16] = 0;
        Debugf("\t[%2d] %s\r\n", msgNr, newsMessage);
        if (!hasNoNoWord(newsMessage) && strlen(newsMessage) > 15)
        {
          if (msgNr <= settingNewsMaxMsg)
          {
            writeFileById("NWS", msgNr, newsMessage);
          }
          msgNr++;
        }
      } // while find(title)
    } // while available ..
    
  } // connected ..

  newsapiClient.stop();

} // getNewsapiData()

// eof
