// SNES Reverb emulator example for PUCA DSP development board with 4MB Flash & PSRAM
// Connect potentiometers to IO32, I033 and IO34 to change DSP Parameters

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SNESverb.h"
#include "WM8978.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

extern "C" {
    void app_main(void);
}

#define DEFAULT_VREF    1060
#define NO_OF_SAMPLES   1024 

TaskHandle_t adc_read_task;  // to run on core 1
TaskHandle_t update_parameters_task; 
SemaphoreHandle_t xSemaphore = NULL;  // binary semaphore for parameter x
SemaphoreHandle_t ySemaphore = NULL;  // binary semaphore for parameter y
SemaphoreHandle_t zSemaphore = NULL;  // binary semaphore for parameter z
WM8978 wm8978;
SNESverb* DSP = new (std::nothrow) SNESverb(36000, 128);

float adc_read_1, adc_read_2, adc_read_3, map_output, map_input, delaytime, feedback, width = 0;

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
        delaytime = map_adc(adc_read_1, 0, 1); 
        feedback = map_adc(adc_read_2, 0, 1);  
        width = map_adc(adc_read_3, 0, 1); 
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
             DSP->setParamValue("delaytime", delaytime);
             DSP->setParamValue("feedback", feedback);
             DSP->setParamValue("width", width); 
             vTaskDelay(pdMS_TO_TICKS(50));

             TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
             TIMERG0.wdt_feed=1;
             TIMERG0.wdt_wprotect=0;
           } 
       }
   if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore != NULL)
   {
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


void app_main() {

  if (DSP == nullptr) { //check for nullity 
        printf ("Memory allocation failed");
        DSP->~SNESverb();  // Return the memory being pointed to if allocation fails       
    } 

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(0,1,0);    // Line input 
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(1);  // Sample Rate 36kHz
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(4);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(55,55);
  wm8978.monoOut(1);  // routes Left channel audio to MN (mono) output
  wm8978.i2sCfg(2,0);  // format 12S, length 16bit

  adc_config(); 
  xTaskCreatePinnedToCore(read_adc, "adc_read_task", 2048, NULL, 5, &adc_read_task, 0); // core 1 
  xTaskCreatePinnedToCore(update_parameters, "update_parameters_task", 2048, NULL, 6, &update_parameters_task, 1); // core 1 

  DSP->start(); 
  vTaskDelay(pdMS_TO_TICKS(100)); 
  printf("DSP Started");

  while(1) {
  vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}






