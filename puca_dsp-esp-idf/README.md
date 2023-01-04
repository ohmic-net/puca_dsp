This directory contains software examples for; 
- Espressif ESP-IDF development Framework v5.0

If you are hoping to build against ESP-IDF v4.0, refer to the `puca_dsp-platformio-esp-idf` example.


To build, once the ESP-IDF toolchain is initialised (usually `get_idf`), run:
```
idf.py build
```
In this example's directory (`puca_dsp/puca_dsp-esp-idf`).

Two example I2S functions are provided in `main.cpp`:
- `passthrough()` sends all I2S data received from the ADC to the DAC, so that the PÚCA DSP is transparent.
- `sine_wave()` generates a 200Hz sine wave at the output.


