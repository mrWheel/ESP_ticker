#include "weerlive_nl.h"

const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* host = "weerlive.nl";
const char* apiKey = "YourAPIKey";

void setupWiFi() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String getWeatherData() {
  WiFiClientSecure client;
  String url = "/api/json-data-10min.php?key=";
  url += apiKey;
  url += "&locatie=Amsterdam";
  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return "";
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  String response = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }

  while (client.available()) {
    response += client.readStringUntil('\n');
  }

  Serial.println("Closing connection");

  return response;
}
