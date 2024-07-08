#ifndef SENDINDEXPAGE_H
#define SENDINDEXPAGE_H

#include <ESP8266WebServer.h>

extern const char index_html[];

void handleRoot(ESP8266WebServer &server);

#endif
