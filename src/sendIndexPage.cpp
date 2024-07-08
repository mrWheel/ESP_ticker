#include "sendIndexPage.h"

// HTML content to be served
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP8266 Ticker</title>
  <style>
    body { text-align: center; font-family: "Trebuchet MS", Arial; margin-top: 50px; }
    h1 { color: #2f4f4f; } p { color: #696969; }
  </style>
</head>
<body>
  <h1>ESP8266 Ticker</h1>
  <p>This is a simple web server example using ESP8266.</p>
</body>
</html>
)rawliteral";

void handleRoot(ESP8266WebServer &server) {
  server.sendHeader("Content-Type", "text/html", true);
  server.send(200, "text/html", index_html);
}
