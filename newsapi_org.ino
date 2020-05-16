

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

  while (newsapiClient.connected())
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
      String title;
      int msgNr = 0;
      //while (newsapiClient.find("\"title\":"))
      while (newsapiClient.find("\"author\":"))
      {
        //newsapiClient.readBytesUntil('\0',  jsonResponse, sizeof(jsonResponse));
        title = newsapiClient.readStringUntil('}');
        parseJsonKey(title.c_str(), "title", newsMessage, NEWS_SIZE);
        Debugf("[%2d] %s\r\n", msgNr, newsMessage);
        if (!hasNoNoWord(newsMessage))
        {
          if (msgNr <= settingNewsMaxMsg)
          {
            writeFileById("NWS", msgNr, newsMessage);
          }
          msgNr++;
        }
      }
    } // while available ..
    
  } // connected ..

  newsapiClient.stop();

} // getNewsapiData()

// eof
