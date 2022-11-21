// Test sketch for PUCA DSP development board with 16MB Flash and no PSRAM
// In tools select Flash size 16MB with a 16MB Partition Scheme, and PSRAM disabled
// Use https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/ to upload the SPIFFS filesystem image


#include <Arduino.h>


void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);  // mount SPIFFS 
  Serial.println("Hello, Spiffs!"); 
  vTaskDelay(pdMS_TO_TICKS(1000)); 
  
}

void loop() {
//  vTaskDelay(pdMS_TO_TICKS(1000));
//  Serial.println("Hello, Spiffs!"); 
}
