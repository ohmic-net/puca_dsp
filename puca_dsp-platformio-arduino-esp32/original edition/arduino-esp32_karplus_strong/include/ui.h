#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_attr.h"

#define UI_BUTTON_PIN GPIO_NUM_36

class UI {
public:
    UI();
    void init();
    void update();  // call from main loop/task
    bool getPluckEvent(); // one-shot event

private:
    static void IRAM_ATTR isrHandler(void* arg);

    gpio_num_t buttonPin;

    volatile bool isrFlag = false;
    bool pluckEvent = false;

    int64_t lastDebounceTimeUs = 0;
    const int64_t debounceTimeUs = 20000; // 20 ms
};