// Library for handling server and ota updates 

#include "Network.h"

AsyncWebServer server(80);  // port
size_t content_len;

void network::handleDoUpdate(AsyncWebServerRequest *request,
                             const String& filename,
                             size_t index,
                             uint8_t *data,
                             size_t len,
                             bool final)
{
    if (!index) {
        Serial.printf("OTA start: %s\n", filename.c_str());
        content_len = request->contentLength();

        int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
            Update.printError(Serial);
        }
    }

    if (len) {
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        } else {
            size_t progress = index + len;
            Serial.printf("Progress: %u%%\n", (progress * 100) / content_len);
        }
    }

    if (final) {
        if (!Update.end(true)) {
            Update.printError(Serial);
        } else {
            Serial.println("OTA complete");

            request->send(200, "text/plain", "Rebooting...");
            delay(100);
            ESP.restart();
        }
    }
}

void network::printProgress(size_t prg, size_t sz) {
  Serial.printf("Progress: %d%%\n", (prg*100)/content_len);
}

void network::notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

boolean network::serverInit() {	
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
});
  server.on("/doUpdate", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
  );
  Update.onProgress(printProgress);
  server.onNotFound(notFound); 
  server.begin();
  return true; 
}

void network::mdnsInit(void)
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set("puca_dsp");
    //set default instance
    mdns_instance_name_set("puca_dsp");

	mdns_txt_item_t serviceTxtData[] = {
            {"board", "puca_dsp"},
            {"path",  "/"}
    };

	ESP_ERROR_CHECK(mdns_service_add("puca_dsp", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));

	mdns_service_instance_name_set("_http", "_tcp", "puca_dsp");
    return;
}
