/*******************************************************************************
 I2C Configuration for a Wolfson Codec Audio Codec.
 This file was written based on the information provided in the Codec Specs
 and on the NuovotonDuino project: https://github.com/DFRobot/NuvotonDuino.
 *******************************************************************************/

#ifndef __Codec_H
#define __Codec_H

#include <stdio.h>
#include <stdint.h>

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define ACK_CHECK_EN 0x1

#define EQ1_80Hz 0X00
#define EQ1_105Hz 0X01
#define EQ1_135Hz 0X02
#define EQ1_175Hz    0X03

#define EQ2_230Hz    0X00
#define EQ2_300Hz    0X01
#define EQ2_385Hz    0X02
#define EQ2_500Hz    0X03

#define EQ3_650Hz    0X00
#define EQ3_850Hz    0X01
#define EQ3_1100Hz 0X02
#define EQ3_14000Hz 0X03

#define EQ4_1800Hz 0X00
#define EQ4_2400Hz 0X01
#define EQ4_3200Hz 0X02
#define EQ4_4100Hz 0X03

#define EQ5_5300Hz 0X00
#define EQ5_6900Hz 0X01
#define EQ5_9000Hz 0X02
#define EQ5_11700Hz 0X03

class Codec
{
public:
    uint8_t init(void);
    void initI2C(void);
    void addaCfg(uint8_t dacen,uint8_t adcen);
    void inputCfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
    void outputCfg(uint8_t dacen,uint8_t bpsen);
    void micGain(uint8_t gain);
    void lineinGain(uint8_t gain);
    void auxGain(uint8_t gain);
    uint8_t writeReg(uint8_t reg,uint16_t val);
    uint16_t readReg(uint8_t reg);
    void hpVolSet(uint8_t voll,uint8_t volr);
    void spkVolSet(uint8_t volx);
    void i2sCfg(uint8_t fmt,uint8_t len);
    void threeDSet(uint8_t depth);
    void eq3DDir(uint8_t dir);
    void eq1Set(uint8_t cfreq,uint8_t gain);
    void eq2Set(uint8_t cfreq,uint8_t gain);
    void eq3Set(uint8_t cfreq,uint8_t gain);
    void eq4Set(uint8_t cfreq,uint8_t gain);
    void eq5Set(uint8_t cfreq,uint8_t gain);
    void noiseSet(uint8_t enable,uint8_t gain);
    void alcSet(uint8_t enable, uint8_t maxgain, uint8_t mingain);
    void sleep(uint8_t enable); 
    void resume(void); 
    void monoOut(uint8_t enable); 
    void spkBoost(uint8_t enable); 
    void sampleRate(void);
    void loopback(uint8_t enable);
    void aMute(uint8_t enable);
    void softMute(uint8_t enable);

};

#endif

