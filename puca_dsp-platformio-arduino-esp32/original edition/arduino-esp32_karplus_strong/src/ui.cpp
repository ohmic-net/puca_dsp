#include "ui.h"
#include "esp_timer.h"
#include "esp_err.h"

UI::UI() {}

// ------------------------------------------------
// Init GPIO + ISR
// ------------------------------------------------
void UI::init()
{
    gpio_config_t io_conf{};
    io_conf.intr_type = GPIO_INTR_POSEDGE; // rising edge
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << UI_BUTTON_PIN);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    // Install ISR service (safe to call once globally ideally)
    static bool isrServiceInstalled = false;
    if (!isrServiceInstalled) {
        gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
        isrServiceInstalled = true;
    }

    gpio_isr_handler_add(UI_BUTTON_PIN, isrHandler, this);
}

// ------------------------------------------------
// ISR (IRAM-safe, minimal)
// ------------------------------------------------
void IRAM_ATTR UI::isrHandler(void* arg)
{
    UI* self = static_cast<UI*>(arg);
    self->isrFlag = true;
}

// ------------------------------------------------
// Update (call from task context)
// ------------------------------------------------
void UI::update()
{
    if (!isrFlag) return;

    isrFlag = false;

    int64_t now = esp_timer_get_time(); // microseconds

    if ((now - lastDebounceTimeUs) > debounceTimeUs) {
        lastDebounceTimeUs = now;
        pluckEvent = true;
    }
}

// ------------------------------------------------
// Get event
// ------------------------------------------------
bool UI::getPluckEvent()
{
    if (pluckEvent) {
        pluckEvent = false;
        return true;
    }
    return false;
}