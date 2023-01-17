// PUCA_DSP Eurorack module trigger inputs and LED test 
// Send +5V trigger or gate signals into TR1 or TR2 to trigger the LEDs
// Touch the capacitive touch button to trigger the top LEDs

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

#define BUTTON     36    // PUCA_DSP onboard button
#define TRIG1      13    // PUCA Module trigger inputs, HIGH after boot 
#define TRIG2      14
#define TOUCH      15    // PUCA Module capacitive touch button
#define LED1        5    // PUCA_DSP onboard LED
#define LED2        2    // Module LED, LOW after boot 
#define LED3        4    // Module LED, LOW after boot 

int trig1State, trig2State, touchState, trig1, trig2, touch = 0; 

void setup() {

Serial.begin(115200);

pinMode(BUTTON, INPUT);
pinMode(TRIG1, INPUT); 
pinMode(TRIG2, INPUT); 
pinMode(TOUCH, INPUT); 
pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
pinMode(LED3, OUTPUT);

vTaskDelay(pdMS_TO_TICKS(100));
digitalWrite(LED1, HIGH);  
Serial.println("Initialisation Complete");  

}

void loop() {

trig1State = digitalRead(TRIG1);
trig2State = digitalRead(TRIG2);
touchState = digitalRead(TOUCH); 

  if (trig1State == 0) {
          trig1 = 1;
          digitalWrite(LED2, HIGH); 
        }
        else if (trig1State == 1) {
          trig1 = 0;
          digitalWrite(LED2, LOW);
        }
   
   if (trig2State == 0) {
          trig2 = 1;
          digitalWrite(LED3, HIGH); 
        }
        else if (trig2State == 1) {
          trig2 = 0;
          digitalWrite(LED3, LOW); 
        }

   if (touchState == 0) {
          touch = 0;
          digitalWrite(LED3, LOW);
        }
        else if (touchState == 1) {
          touch = 1;
          digitalWrite(LED3, HIGH);
  
        }
}
