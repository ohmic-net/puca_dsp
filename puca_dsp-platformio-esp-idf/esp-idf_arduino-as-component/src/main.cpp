// Blink example for including Arduino as a component in ESP-IDF, for PUCA DSP Development Board

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

extern "C" {
    void app_main(void);
}

// In 'make menuconfig', 'Arduino Configuration", enable "Autostart Arduino setup and loop on boot",
#define CONFIG_AUTOSTART_ARDUINO  1 

#define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO   // You can run 'make menuconfig' to choose the GPIO to blink..

#ifndef LED_BUILTIN
#define LED_BUILTIN 5    // ...or use this line and set the LED pin here 
#endif

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#if !CONFIG_AUTOSTART_ARDUINO 
void arduinoTask(void *pvParameter) {
    pinMode(LED_BUILTIN, OUTPUT);
    while(1) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(1000);
    }
}

void app_main()
{
    initArduino();  // initialize arduino library before we start the tasks

    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&arduinoTask, "arduino_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

#else
void setup() {
    Serial.begin(115200);
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.println("Hello!");
    delay(1000);
}
#endif 