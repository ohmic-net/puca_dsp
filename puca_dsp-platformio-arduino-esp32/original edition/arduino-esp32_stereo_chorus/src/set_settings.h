#ifndef SETTINGS_H_
#define SETTINGS_H_
  
  #pragma once
  #include "codec.h"
  #include <Arduino.h>
  #include "driver/i2s.h"

  #define SAMPLE_RATE     (48000)
  #define BITS_PER_SAMPLE (16)
  #define CHANNEL_COUNT 2

  //ESP32 PUCA PIN ASSIGNMENTS
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~

  #define POT1 33          
  #define POT2 34         
  #define POT3 32
  #define POT4 32
  
  #define LED1 4
  #define LED2 2
  
  #define KEY1 13  //TR1
  #define KEY2 14  //TR2
  #define KEY3 15  //TOUCH
  #define KEY4 36  
  #define KEY_BOARD 37

  #define TOUCH_THRESHOLD 30

//ESP32-Codec PIN SETUP
#define I2S_NUM			(0)
#define IS2_MCLK_PIN (0)
#define I2S_BCLK	  (23)
#define I2S_LRC     (25)
#define I2S_DIN     (27)
#define I2S_DOUT    (26)

#define Codec_SDA	     19  //SDA
#define Codec_SCK		   18   //SCL
#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_SDA_IO 19
#define Codec_ADDR	  0x1A  //WM8978
#define WM8978_ADDR   0X1A    //WM8978
#define I2C_MASTER_NUM 1 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

//audio processing frame length in samples (L+R) 64 samples (32R+32L) 256 Bytes
//Used as size of i2s input and output buffers
#define FRAMELENGTH    256
//audio processing priority
#define AUDIO_PROCESS_PRIORITY  10

//SRAM used for DMA = DMABUFFERLENGTH * DMABUFFERCOUNT * BITS_PER_SAMPLE/8 * CHANNEL_COUNT
//Lower number for low latency, Higher number for more signal processing time
//Must be value between 8 and 1024 in bytes
#define DMABUFFERLENGTH 64

//number of above DMA Buffers of DMABUFFERLENGTH
#define DMABUFFERCOUNT  4

  // processor timing variables for system monitor, also included in task.cpp
  extern unsigned int runningTicks;
  extern unsigned int usedticks;
  extern unsigned int availableticks;
  extern unsigned int availableticks_start;
  extern unsigned int availableticks_end;
  extern unsigned int usedticks_start;
  extern unsigned int usedticks_end;
  extern unsigned int processedframe;
  extern unsigned int audiofps;

 void I2S_init(void);

#endif