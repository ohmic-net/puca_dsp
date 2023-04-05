/*!
 *  @file       task.cpp
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
  
#include "task.h"
#include "set_module.h" 

//codec instance
static bool _muteLeftAdcIn = false;
static bool _muteRightAdcIn = false;
bool res = false;

static bool _codecIsReady = false;
static float _outCorrectionGain = 1;
static int _optimizedRange = 2;

 // processor timing variables for system monitor
  unsigned int runningTicks;
  unsigned int usedticks;
  unsigned int availableticks;
  unsigned int availableticks_start;
  unsigned int availableticks_end;
  unsigned int usedticks_start;
  unsigned int usedticks_end;
  unsigned int processedframe;
  unsigned int audiofps;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~POT CONTROL TASK~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void controltaskx(void* arg)   //short test task
{  
  while(true)
  {
    vTaskDelay(1);
  
    float val = analogRead(myPedal->control[0].pin);
    myPedal->control[0].value =val;
    Serial.printf("CTRL-%d %s: %d\n",0,myPedal->control[0].name.c_str(),myPedal->control[0].value);
	}
}



void controltask(void* arg)
{ 
  int controlState[6];
  int stateCounter[6];
  //Serial.println("Starting controltask");

  while(true)
  {
    vTaskDelay(1);
    runningTicks++;
    
    for(int i=0;i<6;i++)  //service each of 6 possible pot controllers
    {
      
      ////////////////////////////////////////////////////////////////////////////////
      //POTENTIOMETER CONTROL MODE
      if(myPedal->control[i].mode == CM_POT)
      {
        //read the analog port
        float val = analogRead(myPedal->control[i].pin);
        if(myPedal->control[i].inverted)
        val = 4095-val;

          int increment = 4096/myPedal->control[i].levelCount;
          int position = val/increment;
          
          //normalize the unexpected
          if(position >= myPedal->control[i].levelCount)
            position = myPedal->control[i].levelCount -1;
          if(position < 0) position = 0;

          //add some hysteresis for stability
          int modulus = (int)val % increment;
          if(position > myPedal->control[i].value)
          {
            if((position-myPedal->control[i].value > 1)||(modulus > (increment>>2)))
            {
              myPedal->control[i].value = position;
              myPedal->onControlChange(i);
            }
          }
          else if(position < myPedal->control[i].value)
          {
            if(((myPedal->control[i].value - position) > 1)||(modulus < (increment-(increment>>2))))
            {
              myPedal->control[i].value = position;
              myPedal->onControlChange(i);
            }
          }
      }
      ////////////////////////////////////////////////////////////////////////////////
      //SELECTOR CONTROL MODE
      else if(myPedal->control[i].mode == CM_SELECTOR)
      {
        //read the analog port
        float val = analogRead(myPedal->control[i].pin);
        if(myPedal->control[i].inverted)
        val = 4095-val;

        //find the selector channel from val
        int channelwidth = 4096/myPedal->control[i].levelCount;
        int readchannel = val/channelwidth;

        //normalize the unexpected
        if(readchannel >= myPedal->control[i].levelCount)
          readchannel = myPedal->control[i].levelCount -1;
        if(readchannel < 0) readchannel = 0;
          
        if(readchannel != myPedal->control[i].value) //the value has changed
        {
          myPedal->control[i].value = readchannel;
          myPedal->onControlChange(i);
        }
            
      }
      ////////////////////////////////////////////////////////////////////////////////
      //TOGGLE PUSH BUTTON CONTROL MODE
      else if(myPedal->control[i].mode == CM_TOGGLE)
      {
        //read the analog port
        float val = analogRead(myPedal->control[i].pin);
        if(myPedal->control[i].inverted)
        val = 4095-val;

        switch(controlState[i])
        {
          case 0: //wait press
          {
            if(val < 2048)
            {
              controlState[i] = 1; //wait stable press
              stateCounter[i]=0;
            }
            break;
          }
          case 1: //wait stable press
          {
            if(val < 2048)
            {
              stateCounter[i]++;
              if(stateCounter[i] > 2)
              {
                if(myPedal->control[i].value==0)
                  myPedal->control[i].value=1;
                else myPedal->control[i].value=0;
                
                myPedal->onControlChange(i);
                controlState[i] = 2; //wait release
              }
            }
            else //not < 2048
            {
              controlState[i] = 0; //back to wait press
            }
            break;
          }
          case 2: //wait release
          {
            if(val > 2048)
            {
              controlState[i] = 3; //wait stable release
              stateCounter[i] = 0;
            }
          }
          case 3: //wait stable release
          {
            if(val > 2048)
            {
              stateCounter[i]++;
              if(stateCounter[i] > 2)
              {
                controlState[i] = 0; //back to wait press
              }
            }
            else controlState[i] = 2; //back to wait release
            break;
          }
        }
      }
      ////////////////////////////////////////////////////////////////////////////////
      //MOMENTARY PUSH BUTTON CONTROL MODE
      else if(myPedal->control[i].mode == CM_MOMENTARY)
      {
        //read the analog port
        float val = analogRead(myPedal->control[i].pin);
        if(myPedal->control[i].inverted)
        val = 4095-val;

          int tempval = 0;
          if(val < 2048)
            tempval = 1;
          if(myPedal->control[i].value == tempval)
            stateCounter[i]=0;
          else
          {
            stateCounter[i]++;
            if(stateCounter[i] > 2)
            {
              myPedal->control[i].value = tempval;
              myPedal->onControlChange(i);
            }
          }
      }
      
      else //mode = CM_DISABLED
      {
           //do nothing
      }
    }
  }
}  //End Control Task

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~BUTTON CONTROL TASK~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void buttontask(void* arg)
{  
  //state variables
  int bstate[6];
  int bstatecounter[6];
  
		//initialize pin mode
	  //do this in the main effects sketch
		//pinMode(pin, INPUT_PULLUP);
		
  for(int i=0;i<6;i++) 
  {
    bstate[i]=0;
    bstatecounter[i] = 0;
  }

  while(true)
  {
    vTaskDelay(1);

    for(int i=0;i<6;i++)  //service each of 6 possible switches
    {
      
      if(myPedal->button[i].mode == BM_TOGGLE)
      { 
        int temp = 0;
        if(myPedal->button[i].touch){
            if(touchRead(myPedal->button[i].pin) >= TOUCH_THRESHOLD)
                 { temp = 0; }
            else { temp = 1; }
        }
        else {
              temp = !digitalRead(myPedal->button[i].pin);
        }

        if(myPedal->button[i].inverted)
        temp = !temp;
        
        if(temp!= bstate[i])
        {
          bstatecounter[i]++;
          if(bstatecounter[i]>9) //debouncing
          {
            bstate[i] = temp;
            bstatecounter[i]=0;
            if(temp)
            {
              myPedal->onButtonPress(i);
              if(myPedal->button[i].value == 1)
                myPedal->button[i].value = 0;
              else
                myPedal->button[i].value = 1;
                
              myPedal->onButtonChange(i);
            }
            else
            {
              myPedal->onButtonRelease(i);
            }
          }
        }
        else bstatecounter[i]=0;
      }
      else if(myPedal->button[i].mode == BM_MOMENTARY)
      {
        int temp = 0;
        if(myPedal->button[i].touch){
            if(touchRead(myPedal->button[i].pin) >= TOUCH_THRESHOLD)
                 { temp = 0; }
            else { temp = 1; }
        }
        else {
              temp = !digitalRead(myPedal->button[i].pin);
        }
        if(myPedal->button[i].inverted)
        temp = !temp;
        
        if(temp != myPedal->button[i].value)
        {
          bstatecounter[i]++;
          if(bstatecounter[i]>9) //debouncing
          {
            bstatecounter[i]=0;
            if(temp==0)
            {
              myPedal->onButtonRelease(i);
              myPedal->button[i].value = 0;
              myPedal->onButtonChange(i);
            }
            else //temp=1
            {
             myPedal->onButtonPress(i);
              myPedal->button[i].value = 1;
              myPedal->onButtonChange(i);
            }
          }
        }
        else //temp = myPedal->button[i].value
        {
          bstatecounter[i]=0;
        }
      }

    }
  }
} //End Button task

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~FRAME COUNTER TASK~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void framecounter_task(void* arg)  //Only used for System Monitor
{
  while(true)
  {
    audiofps = processedframe;
    processedframe = 0;
    vTaskDelay(1000);
  }
  vTaskDelete(NULL);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~DEBUG VARIABLES~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static char debugStringPtr[50] = "None";
static float debugVars[]={0,0,0,0};

void setDebugStr(const char* str)
{
	//debugStringPtr = (char*) str;
	
}
void setDebugVars(float val1, float val2, float val3, float val4)
{
	debugVars[0]=val1;
	debugVars[1]=val2;
	debugVars[2]=val3;
	debugVars[3]=val4;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~MAIN BLACKSTOMP~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void taskSetup() 
{
  //validate the pot settings, correct for user mistakes in settings_main.h
  for(int i=0;i<6;i++)
  {
	  switch(myPedal->control[i].mode)
	  {
		  case CM_POT:
		  {
			  if(myPedal->control[i].levelCount > 256)
				myPedal->control[i].levelCount = 256;
			  if(myPedal->control[i].levelCount < 2)
				myPedal->control[i].levelCount = 2;
			  myPedal->control[i].min = 0;
			  myPedal->control[i].max = myPedal->control[i].levelCount -1;
			  break;
		  }
		  case CM_SELECTOR:
		  {
			  if(myPedal->control[i].levelCount > 12)
				myPedal->control[i].levelCount = 12;
			  if(myPedal->control[i].levelCount < 2)
				myPedal->control[i].levelCount = 2;
			  myPedal->control[i].min = 0;
			  myPedal->control[i].max = myPedal->control[i].levelCount -1;
			  break;
		  }
		  case CM_DISABLED:
		  {
			// do nothing
			break;
		  }
		  case CM_TOGGLE:
		  {
			// do nothing
			break;
		  }
		  case CM_MOMENTARY:
		  {
			// do nothing
			break;
		  }
	  }
  }
  
  
	//the main audio processing task is placed in the main loop() of main.cpp (core1) 

	//decoding button presses 
	xTaskCreatePinnedToCore(buttontask, "buttontask", 4096, NULL, AUDIO_PROCESS_PRIORITY, NULL,0);

	//decoding potentiometer and other analog sensors
	xTaskCreatePinnedToCore(controltask, "controltask", 4096, NULL, AUDIO_PROCESS_PRIORITY, NULL,0);

	//audio frame monitoring task used by systemMonitor
	xTaskCreatePinnedToCore(framecounter_task, "framecounter_task", 4096, NULL, AUDIO_PROCESS_PRIORITY, NULL,0);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~SYSTEM MONITOR TASK~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void sysmon_task(void *arg)
{
	int* period = (int*)(arg);
	while(true)
	{
	  //System info
	  Serial.printf("\nSYSTEM INFO:\n");
	  Serial.printf("Internal Total heap %d, internal Free Heap %d\n",ESP.getHeapSize(),ESP.getFreeHeap());
	  Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n",ESP.getPsramSize(),ESP.getFreePsram());
	  Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n",ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
	  Serial.printf("Flash Size %d, Flash Speed %d\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
	  
	  //Blackstomp application info
	  Serial.printf("\nAPPLICATION INFO:\n");
	  Serial.printf("Pedal Name: %s\n",myPedal->name.c_str());
	  Serial.printf("Audio frame per second: %d fps\n",audiofps);
	  Serial.printf("CPU ticks per frame period: %d\n",availableticks);
	  Serial.printf("Used CPU ticks: %d\n",usedticks);
    Serial.printf("running ticks: %d\n",runningTicks);
	  Serial.printf("CPU Usage: %.2f %%\n", 100.0*((float)usedticks/(float)availableticks));
	  for(int i=0;i<6;i++)
	  {
		  if(myPedal->control[i].mode != 0)  // not CM_DISABLED
			Serial.printf("CTRL-%d %s: %d\n",i,myPedal->control[i].name.c_str(),myPedal->control[i].value);
	  }
	  for(int i=0;i<6;i++)
	  {
		  if(myPedal->button[i].mode != 0)  // not CM_DISABLED
			Serial.printf("BUTTON-%d %s: %d\n",i,myPedal->button[i].name.c_str(),myPedal->button[i].value);
	  }
	  char debugstring[51];
	  strncpy(debugstring,debugStringPtr,50);
	  Serial.printf("Debug String: %s\n", debugstring);
	  Serial.printf("Debug Variables: %g, %g, %g, %g\n", debugVars[0], debugVars[1], debugVars[2], debugVars[3]);
	  vTaskDelay(period[0]);
	}
	vTaskDelete(NULL);
}

int _updatePeriod;
void runSystemMonitor(int baudRate, int updatePeriod)
{
	Serial.begin(baudRate);
	_updatePeriod = updatePeriod;
	//run the performance monitoring task at 0 (idle) priority
	xTaskCreatePinnedToCore(sysmon_task, "sysmon_task", 4096, &_updatePeriod, 0, NULL,0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~SCOPE TASK~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum {scp_disabled, scp_startWaitTrigger, scp_waitTrigger, scp_probing, scp_waitdisplay};
static int scpState = scp_disabled;

static int scSampleCount;
static int scSampleLength;
static int scTriggerChannel;
static float scPrevSample;
static float scTriggerLevel;
static bool scRisingTrigger;
static float** scData = NULL;

void scope_task(void *arg)
{
	scSampleCount = 0;
	scpState = scp_startWaitTrigger;
	while(true)
	{
		if(scpState==scp_waitdisplay)
		{
			//send the data to serial display
			for(int j=0;j<scSampleLength;j++)
			{
					Serial.printf("ch-%d:%f, ch-%d:%f\n", 0,scData[0][j],1,scData[1][j]);
			}
			Serial.flush();
			vTaskDelay(1);
			scpState = scp_startWaitTrigger;
		}	
		vTaskDelay(100);
	}
	vTaskDelete(NULL);
}

void runScope(int baudRate, int sampleLength, int triggerChannel, float triggerLevel, bool risingTrigger)
{
	Serial.begin(baudRate);
	scSampleLength = sampleLength;
	scTriggerLevel = triggerLevel;
	scRisingTrigger = risingTrigger;
	scTriggerChannel = triggerChannel;
	
	scData = new float*[2];
	for(int i=0;i<2;i++)
		scData[i]=(float*)ps_malloc((scSampleLength +1) * sizeof(float));
	for(int i=0;i<scSampleLength;i++)
	{
		scData[0][i] = 0;
		scData[1][i] = 0;
	}
	xTaskCreatePinnedToCore(scope_task, "scope_task", 4096, NULL, AUDIO_PROCESS_PRIORITY-1, NULL,0);
}

void scopeProbe(float sample, int channel)
{
	switch(scpState)
	{
		case scp_probing:
		{
			if(scSampleCount < scSampleLength)
			scData[channel][scSampleCount] = sample;
			if(channel == scTriggerChannel)
			{
				scSampleCount++;
				if(scSampleCount>= scSampleLength)
				{
					scpState = scp_waitdisplay;
				}
			}
			break;
		}
		case scp_startWaitTrigger:
		{
			if(channel == scTriggerChannel)
			{
				scPrevSample = sample;
				scpState = scp_waitTrigger;
			}
			break;
		}
		case scp_waitTrigger:
		{
			if(channel == scTriggerChannel)
			{
				if(scRisingTrigger)
				{
					if((sample > scTriggerLevel)&&(scPrevSample <= scTriggerLevel))
					{
						scSampleCount = 0;
						scpState = scp_probing;
					}
				}
				else 
				{
					if((sample < scTriggerLevel)&&(scPrevSample >= scTriggerLevel))
					{
						scSampleCount = 0;
						scpState = scp_probing;
					}
				}
				scPrevSample = sample;

			}
			break;
		}
	}
}

