// faust djembe example for PICO_DSP development board 
// Choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled 
// Attach a potentiometers between GND, 3.3V and IO32, I033 and IO34 to change the DSP parameters
// DSP task and ADC task running on Core 0
// For Strawberry Edition PICO_DSP with 16MB Flash, choose a board with 16MB Flash, a 16MB Partition scheme with spiffs and PSRAM Disabled

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WM8978.h"
#include "djembe6.h"
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

#define DEFAULT_VREF    1093
#define NO_OF_SAMPLES   512 

TaskHandle_t adc_read_task;  
TaskHandle_t update_parameters_task;

SemaphoreHandle_t xSemaphore = NULL;  // binary semaphores for parameters
SemaphoreHandle_t ySemaphore = NULL;  
SemaphoreHandle_t zSemaphore = NULL;

WM8978 wm8978;
djembe6* DSP = new djembe6(32000, 32);

float adc_read_1, adc_read_2, adc_read_3, map_output, map_input, freq, stPos, Sharp  = 0;

void adc_config()
{   
REG_WRITE(0x3FF48800,461088); // write to register for ADC1 to change ADC clk div
vTaskDelay(pdMS_TO_TICKS(10));
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

void read_adc( void * pvParameters )     // read adc and map values 
{  
   xSemaphore = xSemaphoreCreateBinary();
   ySemaphore = xSemaphoreCreateBinary();
   zSemaphore = xSemaphoreCreateBinary(); 

   for (;;) { 
 
     for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {  
         adc_read_1 = adc1_get_raw(ADC1_CHANNEL_4); //IO32
         adc_read_2 = adc1_get_raw(ADC1_CHANNEL_5); //IO33 
         adc_read_3 = adc1_get_raw(ADC1_CHANNEL_6); //IO34 
         vTaskDelay(pdMS_TO_TICKS(30));
         stPos = map_adc(adc_read_2, 0, 1);  
         Sharp = map_adc(adc_read_3, 0, 1); 
         freq = map_adc(adc_read_1, 50, 800);
         
     }
   }

    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    //if( xSemaphore != NULL && ySemaphore != NULL)
    if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore != NULL)
   {
   }
}

void update_parameters (void *pvParameters)   // task that takes the semaphores and updates the dsp parameters
{
      for (;;) {
              for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {

              DSP->setParamValue("freq", freq);
              DSP->setParamValue("stPos", stPos);
              DSP->setParamValue("Sharp", Sharp); 
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

void setup() {

  Serial.begin(115200);

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(0,0,0);     
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(1);
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(50,50);
  wm8978.i2sCfg(2,0);  //format 12S, length 16bit
  
  vTaskDelay(pdMS_TO_TICKS(100));
  adc_config(); 
  xTaskCreatePinnedToCore(read_adc, "adc_read_task", 2048, NULL, 5, &adc_read_task, 0);  
  xTaskCreatePinnedToCore(update_parameters, "update_parameters_task", 2048, NULL, 6, &update_parameters_task, 0); 
 
  DSP->start(); 
  vTaskDelay(pdMS_TO_TICKS(100));
}

void loop() {
     //freq = rand()%(700-50 + 1) + 50; 
     vTaskDelay(pdMS_TO_TICKS(50));
     Serial.println("Freq = "); 
     Serial.print(freq); 
}
