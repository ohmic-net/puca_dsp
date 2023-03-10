// Hardware test example for PUCA_DSP Eurorack Module with a faust additive synthesis example
// DSP task and i2s driver running on core 0
// Send a V/Oct signal from a sequencer into CV1 to the sine wave oscillator 
// Potentiometers and corresponding CV1 / CV2 control additive sine waves 

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sine_add.h"
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
#define BUTTON     36    // PUCA_DSP onboard button
#define TRIG1      13    // PUCA Module trigger inputs, HIGH after boot 
#define TRIG2      14
#define TOUCH      15    // PUCA Module Capacitive Touch Sensor
#define LED1        5    // PUCA_DSP onboard LED
#define LED2        2    // PUCA Module LED, LOW after boot 
#define LED3        4    // PUCA Module LED, LOW after boot 

TaskHandle_t adc_read_task;  // to run on core 1
TaskHandle_t update_parameters_task; 
TaskHandle_t read_trigger_task; 
SemaphoreHandle_t xSemaphore = NULL;  // binary semaphore for parameter x
SemaphoreHandle_t ySemaphore = NULL;  // binary semaphore for parameter y
SemaphoreHandle_t zSemaphore = NULL;

WM8978 wm8978;
sine_add* DSP = new sine_add(48000, 32);
float freq, freq_voct, freq_coarse, gain1, gain2, adc_read_1, adc_read_2, adc_read_3, map_output, map_input = 0; 
int trig1State, trig2State, trig3State, trig1, trig2, trig3 = 0; 

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

float map_adc_V_Oct() {       // function to map adc range to 1V/Octave value 

while (1) {
  float midinote = map_adc(adc_read_1, 107, 0) +24; 
  freq_voct = (float) 432.0 *(float) (pow(2, (midinote - 57) /12.0)); // exponential conversion
  return freq_voct; 
  }
}


void read_adc( void * pvParameters )     // read adc and map values 
{  
   xSemaphore = xSemaphoreCreateBinary();
   ySemaphore = xSemaphoreCreateBinary();
   zSemaphore = xSemaphoreCreateBinary(); 

   for (;;) { 
 
     for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {  
         adc_read_1 = adc1_get_raw(ADC1_CHANNEL_4); //IO32 CV1 V/Oct Input
         adc_read_2 = adc1_get_raw(ADC1_CHANNEL_5); //IO33 CV2 Top Pot
         adc_read_3 = adc1_get_raw(ADC1_CHANNEL_6); //I034 CV3 Btm Pot 
         vTaskDelay(pdMS_TO_TICKS(10)); 
         freq_voct = map_adc_V_Oct();  
         freq = freq_voct;
         gain1 = map_adc(adc_read_2, 0,1);
         gain2 = map_adc(adc_read_3, 0,1);
     }
   }

    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore !=NULL)
   {
   }
}

void update_parameters (void *pvParameters)   // task that takes the semaphores and updates the dsp parameters
{
      for (;;) {
              for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {
              DSP->setParamValue("freq", freq);
              DSP->setParamValue("gain1", gain1);
              DSP->setParamValue("gain2", gain2);
              vTaskDelay(pdMS_TO_TICKS(10));

              TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
              TIMERG0.wdt_feed=1;
              TIMERG0.wdt_wprotect=0;
            } 
        }
    if( xSemaphore != NULL && ySemaphore != NULL && zSemaphore !=NULL)
    {
        if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) && xSemaphoreTake( ySemaphore, portMAX_DELAY ) && xSemaphoreTake ( zSemaphore, portMAX_DELAY)) 
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
  Serial.println("Initialising...");

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(0,0,0);     
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(0); 
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(40,40);
  wm8978.i2sCfg(2,0);  //format 12S, length 16bit

  adc_config(); 
  xTaskCreatePinnedToCore(read_adc, "adc_read_task", 2048, NULL, 5, &adc_read_task, 1); // core 1 
  xTaskCreatePinnedToCore(update_parameters, "update_parameters_task", 2048, NULL, 6, &update_parameters_task, 1); // core 1  

  vTaskDelay(pdMS_TO_TICKS(50));
  DSP->start(); 
  Serial.println("DSP Started"); 

} 

void loop() {
  
   vTaskDelay(pdMS_TO_TICKS(10));

   Serial.print("Freq ="); 
   Serial.println(freq); 
   Serial.print("ADC1 ="); 
   Serial.println(adc_read_1);
   Serial.print("ADC2 ="); 
   Serial.println(adc_read_2);
   Serial.print("ADC3 ="); 
   Serial.println(adc_read_3);
   Serial.println(); 

}