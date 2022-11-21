// faust sawtooth example for PUCA_DSP development board 
// Choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sawtooth.h"
#include "WM8978.h"
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

WM8978 wm8978;

TaskHandle_t adc_read_task;  // to run on core 1
TaskHandle_t update_parameters_task;
SemaphoreHandle_t xSemaphore = NULL;  // binary semaphore for parameter x
SemaphoreHandle_t ySemaphore = NULL;
sawtooth* DSP = new sawtooth(48000, 32);

float adc_read_1, adc_read_2, adc_read_3, map_output, map_input, freq, gain = 0;

void adc_config()
{   
REG_WRITE(0x3FF48800,461088); // write to register for ADC1 to change ADC clk div
vTaskDelay(pdMS_TO_TICKS(100));
esp_adc_cal_characteristics_t *adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));  
esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);         
}

float map_adc (float map_input, float output_start, float output_end) {         // function to linearly map adc range, gets called from send_value_task 
  
    float input_start = 0;
    float input_end = 4095; 
    
    while(1) {
    float slope = 1.0 * (output_end - output_start) / (input_end - input_start);  
    map_output = output_start + slope * (map_input - input_start);  //  map_output = output_start + round (slope * (map_input - input_start));
    return map_output; 
    } 
}

// map_adc_exponential // map the full adc range to 20-20000Hz, correct for ESP32 non-linearity

float map_adc_V_Oct() {       // function to map adc range to 1V/Octave value 

while (1) {
  float midinote = map_adc(adc_read_1, 0, 107) +24; 
  freq = (float) 440.0 *(float) (pow(2, (midinote - 57) /12.0)); // exponential conversion
  //freq = (float) 440.0 + freq_coarse *(float) (pow(2, (midinote - 57) /12.0)); // exponential conversion
  return freq; 
  }
}

void read_adc( void * pvParameters )     // read adc and map values 
{  
   xSemaphore = xSemaphoreCreateBinary();
   ySemaphore = xSemaphoreCreateBinary();

   for (;;) { 
 
     for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {  
         adc_read_1 = adc1_get_raw(ADC1_CHANNEL_5); //IO32 CV1
         adc_read_2 = adc1_get_raw(ADC1_CHANNEL_6); //IO34 CV3 
         vTaskDelay(pdMS_TO_TICKS(10));
         //freq_coarse = map_adc(adc_read_2, 27.5, 16744); 
         freq = map_adc_V_Oct(); 
         gain = map_adc(adc_read_2, 0,1);
     }
   }

    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    if( xSemaphore != NULL && ySemaphore != NULL)
   {
   }
}

void update_parameters (void *pvParameters)   // task that takes the semaphores and updates the dsp parameters
{
      for (;;) {
              for (uint32_t i = 0; i < NO_OF_SAMPLES; i++) {
              DSP->setParamValue("freq", freq);
              DSP->setParamValue("gain", gain);
              vTaskDelay(pdMS_TO_TICKS(10));

              TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
              TIMERG0.wdt_feed=1;
              TIMERG0.wdt_wprotect=0;
            } 
        }
        
    if( xSemaphore != NULL && ySemaphore != NULL)
    {
        if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) && xSemaphoreTake( ySemaphore, portMAX_DELAY ))
          
        // wait forever without blocking
        {
        //vTaskDelay(pdMS_TO_TICKS(10)); 
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
    
            xSemaphoreGive( xSemaphore );
            xSemaphoreGive( ySemaphore );

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
  wm8978.sampleRate(0); 
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(50,50);   // set 50 for approx 10V peak to peak output
  wm8978.i2sCfg(2,0);  //format 12S, length 16bit
  vTaskDelay(pdMS_TO_TICKS(100));

  adc_config(); 
  xTaskCreatePinnedToCore(read_adc, "adc_read_task", 2048, NULL, 5, &adc_read_task, 0); 
  xTaskCreatePinnedToCore(update_parameters, "update_parameters_task", 2048, NULL, 6, &update_parameters_task, 0); 
  
  //gain = 0.9; 
  DSP->start(); 
  Serial.println("DSP Started");
  //DSP->setParamValue("gain", gain); 

  vTaskDelay(pdMS_TO_TICKS(50));
}

void loop() {

 vTaskDelay(pdMS_TO_TICKS(10));
 Serial.print("Freq ="); 
 Serial.println(freq); 
 Serial.print("ADC1 ="); 
 Serial.println(adc_read_1);
 Serial.print("Gain ="); 
 Serial.println(gain); 
 Serial.print("ADC2 ="); 
 Serial.println(adc_read_2);

}
