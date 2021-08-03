// Example Frequency Modulated Oscillator with OTA sketch for PICO_DSP development board
// Choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled 
// Upload the spiffs image to the ESP32 using ESP32 Sketch Data Upload 
// AsyncTCP task running on core 0; Arduino loop, ADC, DSP tasks running on core 1, i2s driver on core 1
// Attach a potentiometer between GND, 3.3V and IO34 to modulate the oscillator 

// For Strawberry Edition PICO_DSP with 16MB Flash, choose a board with 16MB Flash, a 16MB Partition scheme with spiffs and PSRAM Disabled


#include <Arduino.h>
#include "Network.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fm_em.h"
#include "WM8978.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_adc_cal.h"
#include "soc/soc.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define DEFAULT_VREF    1060
#define NO_OF_SAMPLES   1000 

const char* ssid = "PICO_DSP";
//const char* password = ""; 

TaskHandle_t adc_read_task;  // to run on core 1
TaskHandle_t update_parameters_task; 
TaskHandle_t freq_sequence_task; 
SemaphoreHandle_t xSemaphore = NULL;  // binary semaphore for parameter x
SemaphoreHandle_t ySemaphore = NULL;  // binary semaphore for parameter y
SemaphoreHandle_t zSemaphore = NULL;  // binary semaphore for parameter z

network net; 
WM8978 wm8978;
fm_em* DSP = new fm_em(48000, 16);

float adc_read_1, adc_read_2, adc_read_3, map_output, map_input, freq, hratio, idx = 0;

void adc_config()
{   
REG_WRITE(0x3FF48800,461088); // write to register for ADC1 to change ADC clk div
vTaskDelay(pdMS_TO_TICKS(100));
esp_adc_cal_characteristics_t *adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));  
esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);         
}

float map_adc (float map_input, float output_start, float output_end) {         // function to map adc range, gets called from send_value_task 
    float input_start = 0;
    float input_end = 4095; 
    
    while(1) {
    float slope = 1.0 * (output_end - output_start) / (input_end - input_start);  
    map_output = output_start + slope * (map_input - input_start);  //  map_output = output_start + round (slope * (map_input - input_start));
    return map_output; 
    } 
}

float map_adc_VO() {       // function to map adc range to 1V/Octave value 

while (1) {
float midinote = map_adc(adc_read_1, 0, 107) + 24;  // range 8 octaves, convert to midi note numbers 0 to 107, start note C1
freq = (float) 440.0 *(float) (pow(2, (midinote - 57) /12.0)); // exponential conversion
return freq; 
  }
}

void freq_sequence( void * pvParameters )  {

zSemaphore = xSemaphoreCreateBinary();

for (;;) { 

  while (1) { 

  freq = 349.23;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 311.13;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 311.13;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 233.08;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 233.08;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 220;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 196; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 220;
  vTaskDelay(pdMS_TO_TICKS(100));  
  freq = 233.08; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63;
  vTaskDelay(pdMS_TO_TICKS(100));  // end G Minor 
  
  freq = 261.63;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 233.08;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 220;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 196;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 220;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 233.08;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 233.08;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 311.13;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 261.63; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 293.66;
  vTaskDelay(pdMS_TO_TICKS(100));  
  freq = 311.13; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 349.23;
  vTaskDelay(pdMS_TO_TICKS(100));  // end G Minor 

  freq = 698.46;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 622.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 622.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 466.16;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 466.16;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 440;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 392; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 440;
  vTaskDelay(pdMS_TO_TICKS(100));  
  freq = 466.16; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25;
  vTaskDelay(pdMS_TO_TICKS(100)); // end G Minor +octave
  
  freq = 523.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 466.16;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 440;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 392;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 440;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 466.16;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 466.16;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 622.25;
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 523.25; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 587.33;
  vTaskDelay(pdMS_TO_TICKS(100));  
  freq = 622.25; 
  vTaskDelay(pdMS_TO_TICKS(100));
  freq = 698.46;
  vTaskDelay(pdMS_TO_TICKS(100)); // end G Minor +octave
    }
  }

  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;
  if( zSemaphore != NULL ) 
     {
   }
}

void read_adc( void * pvParameters )     // read adc and map values 
{  
   xSemaphore = xSemaphoreCreateBinary();
   ySemaphore = xSemaphoreCreateBinary();
   //zSemaphore = xSemaphoreCreateBinary(); 

   for (;;) { 
 
     for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {  
         //adc_read_1 = adc1_get_raw(ADC1_CHANNEL_4); //IO32
         //adc_read_2 = adc1_get_raw(ADC1_CHANNEL_5); //IO33 
         adc_read_3 = adc1_get_raw(ADC1_CHANNEL_6); //IO34 
         vTaskDelay(pdMS_TO_TICKS(30));
         //hratio = map_adc(adc_read_2, 0, 10);  
         hratio = 4.3; 
         idx = map_adc(adc_read_3, 0, 1000); 
         //freq = map_adc_VO(); 
     }
   }

    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    if( xSemaphore != NULL && ySemaphore != NULL)
    //if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore != NULL)
   {
   }
}

void update_parameters (void *pvParameters)   // task that takes the semaphores and updates the dsp parameters
{
      for (;;) {
              for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {
              DSP->setParamValue("freq", freq);
              DSP->setParamValue("harmonicity Ratio", hratio);
              DSP->setParamValue("index", idx); 
              vTaskDelay(pdMS_TO_TICKS(50));

              TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
              TIMERG0.wdt_feed=1;
              TIMERG0.wdt_wprotect=0;
            } 
        }
    //if( xSemaphore != NULL && ySemaphore != NULL)
    if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore != NULL)
    {
        //if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) && xSemaphoreTake( ySemaphore, portMAX_DELAY ))
        if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) && xSemaphoreTake( ySemaphore, portMAX_DELAY ) && xSemaphoreTake( zSemaphore, portMAX_DELAY))    
        // wait forever without blocking
        {
        //vTaskDelay(pdMS_TO_TICKS(10)); 
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
    
            xSemaphoreGive( xSemaphore );
            xSemaphoreGive( ySemaphore ); 
            xSemaphoreGive( zSemaphore ); 
        }
        else
        {
        }
    }
}


void setup(){

  Serial.begin(115200);
  SPIFFS.begin(true);
  vTaskDelay(pdMS_TO_TICKS(100));

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(0,0,0);     
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(0); 
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(45,45);
  wm8978.monoOut(1);  // routes Left channel audio to MN (mono) output
  wm8978.i2sCfg(2,0);  // format 12S, length 16bit
  vTaskDelay(pdMS_TO_TICKS(100)); 

  adc_config(); 
  xTaskCreatePinnedToCore(freq_sequence, "freq_sequence_task", 2048, NULL, 2, &freq_sequence_task, 1); // core 1 
  xTaskCreatePinnedToCore(read_adc, "adc_read_task", 2048, NULL, 5, &adc_read_task, 1); // core 1 
  xTaskCreatePinnedToCore(update_parameters, "update_parameters_task", 2048, NULL, 6, &update_parameters_task, 1); // core 1 

  vTaskDelay(pdMS_TO_TICKS(100)); 
  DSP->start(); 
  //digitalWrite(PIN_LED1, HIGH); 
  Serial.println("DSP Started"); 

  vTaskDelay(pdMS_TO_TICKS(100)); 

  WiFi.softAP(ssid);  // WiFi mode, STA or AP
  net.mdnsInit(); 
  net.serverInit();
//  digitalWrite(PIN_LED2, HIGH); 

}

void loop() {

  vTaskDelay(pdMS_TO_TICKS(10)); 

  Serial.print("Freq ="); 
  Serial.println(freq); 
  Serial.print("Harmonicity Ratio ="); 
  Serial.println(hratio);
  Serial.print("Index ="); 
  Serial.println(idx);
}
