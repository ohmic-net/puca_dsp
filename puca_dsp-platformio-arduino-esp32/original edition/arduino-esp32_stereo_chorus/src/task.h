/*!
 *  @file       task.h
 *  Project     Blackstomp Arduino Library
 *  @brief      Blackstomp Library for the Arduino
 *  @author     Hasan Murod
 *  @date       19/11/2020
 *  @license    MIT - Copyright (c) 2020 Hasan Murod
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef BLACKSTOMP_H_
#define BLACKSTOMP_H_

#include "set_settings.h"
#include "set_module.h"

//BLACKSTOMP'S SYSTEM API

void controltask(void* arg);  //task for pots
void buttontask(void* arg);   //task for switches

//Blackstomp core setup, 
//should be called inside arduino platform's setup()
void taskSetup(); 

//optimize the analog to digital conversion range
//currently only implemented for Codec-version module 
//range: 0, 1, 2, 3, 4, default: 2 (0.25Vrms/707mVpp)
//(1Vrms/2.83Vpp, 0.5Vrms/1.41Vpp, 0.25Vrms/707mVpp, 0.125Vrms/354mVpp, 0.0625Vrms/177mVpp)
void optimizeConversion(int range=2);

//run system monitor on serial port, should be called on arduino setup when needed
//don't call this function when runScope function has been called
//don't call this function when MIDI is implemented
void runSystemMonitor(int baudRate=115200, int updatePeriod=1500);

//run 2-channel simple scope, should be called on arduino setup when needed
//don't call this function when runSystemMonitor function has been called
//don't call this function when MIDI is implemented
void runScope(int baudRate=1000000, int sampleLength=441, int triggerChannel=0, float triggerLevel=0, bool risingTrigger=true);

//probe a signal to be displayed on Arduino IDE's serial plotter
//channel: 0-1
void scopeProbe(float sample, int channel);

//set debug string (to be shown in the system monitor when it runs)
void setDebugStr(const char* str);

//set debug variables (to be shown in the system monitor when it runs)
void setDebugVars(float val1, float val2=0, float val3=0, float val4=0);

#endif
