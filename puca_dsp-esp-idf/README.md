# ESP-IDF v5.0 Example

The examples in this directory build against [Espressif ESP-IDF development Framework v5.0](https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/get-started/index.html). Follow the documentation on the linked page to install v5.0 to build these examples.

If you are hoping to build against ESP-IDF v4.0, refer to the `puca_dsp-platformio-esp-idf` [examples](https://github.com/ohmic-net/puca_dsp/tree/main/puca_dsp-platformio-esp-idf).


To build, once the ESP-IDF toolchain is initialised (usually `get_idf`), run:
```
idf.py build
```
In this example's directory (`puca_dsp/puca_dsp-esp-idf`).

Two example I2S functions are provided in `main.cpp`:
- `passthrough()` sends all I2S data received from the ADC to the DAC, so that the PÚCA DSP is transparent.
- `sine_wave()` generates a 200Hz sine wave at the output.
