// ESP32 internet radio example for PICO_DSP development board 
// Choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled
// Enter your Wi-Fi settings below, press Button (I036) to change stations 

#include "WiFi.h"
#include "Audio.h"
#include "WM8978.h"
#include "esp_sleep.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
 
// Define 12S pins
#define I2S_BCLK       23  // Bit Clock  
#define I2S_LRC        25  // Left Right / WS Clock
#define I2S_DOUT       26  // Data Out 
#define I2S_DIN        27  // Data In

// Define WM8978 MCLK gpio number */
#define I2S_MCLKPIN     0

Audio audio(I2S_BCLK, I2S_LRC, I2S_DOUT);  // create an instance of audio class 
WM8978 wm8978;   // create an instance of audio codec setup. The default Sample Rate for the Codec is 48kHz
 
String ssid =     "XXXXXXXXXX";   // WiFi Network settings, enter your SSID and Password here
String password = "XXXXXXXXXX";

#define uS_TO_S_FACTOR 1000000 // Conversion factor uS to seconds
#define TIME_TO_SLEEP 2 // time in seconds to sleep for 
#define Button      36

static RTC_NOINIT_ATTR int station_select;   // variable to store in RTC 
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t complexHandlerTask;   // task handle for the sleep task 

void RTC_variable()       // keeps state of a variable after a software or deep sleep reset 
{ 
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_SW) && (reason != ESP_RST_DEEPSLEEP)) {  
  station_select = 0; }
    station_select++;
    //printf("station_select: %d\n",station_select);
    if (station_select > 3) {
        station_select = 0;
    }
}

void change_station (void) { 

    switch(station_select) {
        case 0: 
        audio.connecttohost("http://worldwidefm.out.airtime.pro:8000/worldwidefm_a"); // Worldwide FM
        break; 
        case 1: 
        audio.connecttohost("http://listen2.ntslive.co.uk/listen.pls"); // NTS
        break;
        case 2: 
        audio.connecttohost("http://dublindigitalradio.out.airtime.pro:8000/dublindigitalradio_a");  //DDR
        break;  
        case 3:
        audio.connecttohost("http://streamer.dgen.net:8000/rinseradio"); // Rinse FM
        break;
        } 
}

void IRAM_ATTR isr() {  // interrupt function is stored in IRAM, sets a flag to be checked by non interrupt code 
  portENTER_CRITICAL_ISR(&mux);  
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // notify complexHandlerTask that the buffer is full
    vTaskNotifyGiveFromISR(complexHandlerTask, &xHigherPriorityTaskWoken);  // notifies the complex task that the ISR has something for it to do
    if (xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void complexHandler(void *param) {    // the complex task that doesn't fit in the ISR
  //for (;;) {   
    //uint32_t tcount = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));  // Sleep until the ISR gives us something to do, or for 5 seconds
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);  // Block indefinitely without using CPU, until there is a notification  
    esp_deep_sleep_start(); 
    vTaskDelete(NULL); // it's a one off task, so it can be deleted once it's finished 
  //}
}

void setup() {

    Serial.begin(115200);

    wm8978.init();  // Audio codec initialisation 
    wm8978.addaCfg(1,1); 
    wm8978.inputCfg(1,0,0);     
    wm8978.outputCfg(1,0); 
    wm8978.sampleRate(0);
    wm8978.micGain(30);
    wm8978.auxGain(0);
    wm8978.lineinGain(0);
    wm8978.spkVolSet(55);  // speaker volume, range 0-63
    wm8978.hpVolSet(40,40);  // headphones volume, range 0-63
    wm8978.i2sCfg(2,0);  // I2S format MSB, 16Bit 
    wm8978.eq1Set(80,22); // eq 1 
    wm8978.eq2Set(385,6); 
    wm8978.eq5Set(11700,13); // eq5
    wm8978.spkBoost(1); // speaker boost enable

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);

    audio.setPinout(23, 25, 26, 27);
    audio.i2s_mclk_pin_select(I2S_MCLKPIN);    
    audio.setSampleRate(48000);   // I2S Sample Rate
    audio.setBitsPerSample(16);
    audio.setChannels(2); 
    audio.setVolume(20);  // range 0-21

    pinMode(Button, INPUT);
    xTaskCreatePinnedToCore(complexHandler, "ISR Handler Task", 8192, NULL, 20, &complexHandlerTask, tskNO_AFFINITY);
    attachInterrupt(digitalPinToInterrupt(Button), isr, FALLING);
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    RTC_variable();
    vTaskDelay(pdMS_TO_TICKS(30));

    change_station(); 
    ESP_LOGI(TAG, "Connected. Starting MP3..."); 
}


void loop()
{
    audio.audiotask();      // audio task 
}
 
// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
// void audio_eof_mp3(const char *info){  //end of file
//     Serial.print("eof_mp3     ");Serial.println(info);
// }
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
// void audio_commercial(const char *info){  //duration in sec
//     Serial.print("commercial  ");Serial.println(info);
// }
// void audio_icyurl(const char *info){  //homepage
//     Serial.print("icyurl      ");Serial.println(info);
// }
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
// void audio_eof_speech(const char *info){
//     Serial.print("eof_speech  ");Serial.println(info);
// }
