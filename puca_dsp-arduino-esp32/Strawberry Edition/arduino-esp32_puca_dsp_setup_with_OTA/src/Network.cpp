// Library for handling server and ota updates 

#include "Network.h"

AsyncWebServer server(80);  // port
size_t content_len;

void network::handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    Serial.println("Update");
    content_len = request->contentLength();
    // if filename includes spiffs, update the spiffs partition
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial); }
    else {
    Serial.printf("Progress: %d%%\n", (Update.progress()*100)/Update.size());
    }

  if (final) {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    response->addHeader("Refresh", "20");  
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Update complete");
      Serial.flush();
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
