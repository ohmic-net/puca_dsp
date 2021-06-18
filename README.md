# PICO DSP

**PICO DSP** is an open source, Arduino compatible ESP32 development board for audio and digital signal processing applications. In this repository you will find all the hardware design files, and software examples for Arduino, ESP-IDF and VSCode/PlatformIO. 

# Specifications 

**Processor & Memory**
- Espressif ESP32 Pico D4 Processor
- 32-bit dual core 80MHZ / 160MHZ / 240MHZ
- 4MB SPI Flash with 8MB additional PSRAM or 16MB External SPI Flash 
- Wireless 2.4GHz Wi-Fi -802.11b/g/n
- Bluetooth BLE 4.2
- 3D Antenna

**Audio**
- Wolfson WM9878 Stereo Audio Codec
- Audio Line In on stereo 3.5mm connector
- Audio Headphone / Line Out on stereo 3.5mm connector
- Stereo Aux Line In, Audio Mono Out routed to GPIO Header
- 2 x Knowles SPM0687LR5H-1 MEMS Microphones 
- ESD protection on all audio inputs and outputs
- Support for 8, 11.025, 12, 16, 22.05, 24, 32, 44.1 and 48kHz sample rates
- 1W Speaker Driver, routed to GPIO Header

**Form Factor and Connectivity**
- Breadboard friendly
- 70mm x 24mm
- 11 x GPIO pins broken out to 2.54mm pitch header, with access to both ESP32 ADC channels, JTAG and capacitive touch pins
- USB 2.0 over USB Type C connector

**Power**
- 3.7/4.2v Lithium Polymer Rechargeable Battery, USB  or external 5V DC power source
- ESP32 and Audio Codec can be placed into low power modes under software control
- Battery voltage level detection
- ESD protection on USB data bus

# Documentation 

![PICO_DSP-Pinout](https://user-images.githubusercontent.com/66629326/122237980-ab1be080-ceb7-11eb-8d9a-83141a8bc71b.jpg)

# Pin Description
![pin_description_repo](https://user-images.githubusercontent.com/66629326/122561950-5c4a8400-d03a-11eb-9e00-627c2fa488de.jpg)

# Absolute Maximum Ratings 
![abs_max_repo](https://user-images.githubusercontent.com/66629326/122562538-15a95980-d03b-11eb-8b5e-f1d52b7d8dae.jpg)

Maximum voltage before clipping on Audio Input, Aux L & Aux R = 3.3V peak-to-peak, AC. Audio input paths are AC coupled. 
Maximum voltage on GPIO pins = 3.3V DC. GPIO pins are not overvoltage protected, for voltages higher than 3.3V use an input protection circuit such as a diode clamp or a level shifter.

# Current Consumption Characteristics 
![current_repo](https://user-images.githubusercontent.com/66629326/122562562-1e9a2b00-d03b-11eb-90f2-9c3ff6472d79.jpg)

# Physical Dimensions
![phys_dimensions_repo](https://user-images.githubusercontent.com/66629326/122562620-2e197400-d03b-11eb-876a-eb97ada749b9.jpg)
*Height measurement is with pin headers soldered. 

# Open Source Licensing 

**PICO DSP** hardware files are released as open source under the CERN v1.2 license. **PICO DSP** software examples are released under the MIT license, software examples using Faust DSP also contain additional license information relevant to Faust DSP. Please review these licenses before using these files in your own projects to understand your obligations.

# Crowd Supply link

Link to the project page on Crowd Supply - https://www.crowdsupply.com/ohmic/pico-dsp

