#include "newsapi_org.h"


// Define external global variables
const char* newsapiHost = "newsapi.org";
const int httpPort = 80;
int newsapiStatus = 0;
char newsMessage[NEWS_SIZE] = {};
int32_t maxWait;
char jsonResponse[1024];
char val[51] = "";
WiFiClient newsapiClient;

bool getNewsapiData() 
{
    int startPos, endPos;
    Debugln();
    DebugTf("getNewsapiData(%s)\r\n", newsapiHost);
    
    String url = "/v2/top-headlines?country=nl&apiKey=";
    url += settingNewsAUTH;
    DebugTf("Requesting URL: %s/v2/top-headlines?country=nl&apiKey=secret\r\n", newsapiHost);
    DebugFlush();

    if (!newsapiClient.connect(newsapiHost, httpPort)) 
    {
        DebugTln("connection failed");
        sprintf(tempMessage, "connection to %s failed", newsapiHost);
        for (int i = 0; i <= settingNewsMaxMsg; i++)
        {
            if (i == 1) writeFileById("NWS", i, "There is No News ....");
            else writeFileById("NWS", i, "");
        }
        newsapiClient.flush();
        newsapiClient.stop();
        return false;
    }

    newsapiClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + newsapiHost + "\r\n" +
                        "User-Agent: ESP-ticker\r\n" +
                        "Connection: close\r\n\r\n");
    delay(10);
    newsapiClient.setTimeout(5000);

    while (newsapiClient.connected() || newsapiClient.available())
    {
        yield();
        while (newsapiClient.available())
        {
            Debugln();
            if (newsapiClient.find("HTTP/1.1"))
            {
                newsapiStatus = newsapiClient.parseInt();
                DebugTf("Statuscode: [%d] ", newsapiStatus);
                if (newsapiStatus != 200)
                {
                    Debugln(" ERROR!");
                    while (newsapiClient.available())
                    {
                        char nC = newsapiClient.read();
                        Debug(nC);
                    }
                    Debugln();
                    newsapiClient.flush();
                    newsapiClient.stop();
                    for (int i = 0; i <= settingNewsMaxMsg; i++)
                    {
                        if (i == 1) writeFileById("NWS", i, "There is No News ....");
                        else writeFileById("NWS", i, "");
                    }
                    newsapiClient.flush();
                    newsapiClient.stop();
                    return false;
                }
                Debugln(" OK!");
            }
            else
            {
                DebugTln("Error reading newsapi.org.. -> bailout!");
                for (int i = 0; i <= settingNewsMaxMsg; i++)
                {
                    if (i == 1) writeFileById("NWS", i, "There is No News ....");
                    else writeFileById("NWS", i, "");
                }
                newsapiClient.flush();
                newsapiClient.stop();
                return false;
            }

            uint16_t msgIdx = 0;
            int msgNr = 0;
            while (newsapiClient.find("\"title\":\""))
            {
                for (int i = 0; i < NEWS_SIZE; i++) newsMessage[i] = 0;
                msgIdx = 0;
                while ((strIndex(newsMessage, "\"description\":") == -1) && (msgIdx < (NEWS_SIZE - 2)))
                {
                    newsMessage[msgIdx++] = (char)newsapiClient.read();
                }
                newsMessage[msgIdx] = '\0';
                if (msgIdx > 30) newsMessage[msgIdx - 16] = 0;
                Debugf("\t[%2d] %s\r\n", msgNr, newsMessage);
                //-- AaW conversie ----
                //-- AaW if (!hasNoNoWord(newsMessage) && strlen(newsMessage) > 15)
                {
                    if (msgNr <= settingNewsMaxMsg)
                    {
                        writeFileById("NWS", msgNr, newsMessage);
                    }
                    msgNr++;
                }
            }
        }
    }
    newsapiClient.flush();
    newsapiClient.stop();
    updateMessage("0", "News brought to you by 'newsapi.org'");
    return true;
}

void removeNewsData()
{
    char nwsName[15];
    for (int n = 0; n <= settingNewsMaxMsg; n++)
    {
        sprintf(nwsName, "/newsFiles/NWS-%03d", n);
        DebugTf("Remove [%s] from LittleFS ..\r\n", nwsName);
        LittleFS.remove(nwsName);
    }
}
