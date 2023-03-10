# PÚCA Eurorack Module

This directory includes binary files (`firmware.bin`) for the Eurorack module. 

The `hardware_test_arduino` folder includes Arduino sketches for checking that the hardware functionality is working correctly.


Updating the firmware; 

On boot the module creates a WiFi soft access point with the network name "PÚCA DSP". If you open a browser window (Chrome, Firefox) and go to; http://puca_dsp.local you will find an html page with a firmware update function. Select the new firmware.bin file to load from your local machine and click upload. Note that you don't need to upload the partitions.bin or spiffs.bin files if using the , unless you have changed these, they are included in each folder in case you are using esptool.py or the Espressif Flash Download Tool instead of the module OTA functionality.

User interface parameters and DSP parameters for each firmware are outlined below. On the module; 
- CV1 is the left CV input jack, CV2 is the middle CV input jack and/or the top potentiometer, and CV3 is the right CV input jack and/or the bottom potentiometer. 
- TRIG1 is the top trigger input and TRIG2 is the bottom trigger input. 
- TOUCH is the capacitive touch sensor activated by touching the PÚCA's paw. 


Current firmwares; 

- SNES_Verb - emulation of a SNES type reverb with a short delay line. CV2 is delay time, and CV3 is feedback. 

- Oscillator - a wavetable oscillator with 12 single cycle, band limited waveforms. CV1 is the 1V/Octave input, CV2 is the oscillator root frequency, and CV3 is linear FM. A TOUCH input or a trigger signal on TRIG1 will change the waveform. 


Some notes on the Oscillator range; 

The 1V/Octave input has a range of 7 octaves, due to the non-linearity of the ESP32's ADC inputs the pitch tracking is accurate over 4 octaves and the highest 3 octaves do not track accurately, as can be seen from the below graph. The pitch resolution is 34.375mV per semitone or 0.34375 mV per cent. 

The Oscillator root frequency range is from midi note A0 (27.50Hz) to A3 (220Hz), used in combination with the 1V/Octave input in practice this gives a musically usable range of ~6 octaves without note drift. 

The FM input allows for linear frequency modulation up to maximum frequency range of 1 midi note. 

 
 



