// network.h

#pragma once 

#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#define U_PART U_SPIFFS

// const char* ssid = "PICO_DSP";
// //const char* password = ""; 

class network 
{
public:

    static void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
    static void printProgress(size_t prg, size_t sz);
    static void notFound(AsyncWebServerRequest *request);
    static boolean serverInit();
    static void mdnsInit(void);

};