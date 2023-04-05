#include "set_codec.h"

// declaration of codec, an instance of Codec 
               Codec codec;

 //Some functions built to load Codec registers, most are set in Codec.init()
 void codec_sets()         //to be executed in main.cpp
 {
  codec.addaCfg(1,1);      //enable adc and dac (DAC 1/0, ADC 1,0)
  codec.inputCfg(0,1,0);   //input config, (MIC 1/0, LINE 1/0, AUX 1/0)
  codec.outputCfg(1,0);    //output MIXER config (DAC 1/0, INPUT BYPASS 1/0)
  codec.sampleRate();     // SAMPLE_RATE = 48kHz, 32kHz, 24kHz, 16kHz, 12kHz, 8kHz
  codec.hpVolSet(40,40);   //headphone volume 0 TO 63, (LEFT, RIGHT)
  codec.i2sCfg(2,0);       // I2S format MSB, 16Bit
  codec.loopback(0);       // Bypass, Input to Output when 1, no Bypass when 0
  };
