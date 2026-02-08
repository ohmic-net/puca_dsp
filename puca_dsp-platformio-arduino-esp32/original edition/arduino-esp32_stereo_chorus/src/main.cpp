// This example is by contributed by John Talbert - http://www.jtalbert.xyz/ESP32/
// Read the example documentation here - https://jtalbert.xyz/Downloads/PUCA_Software.pdf

#include <Arduino.h>
#include "set_settings.h"
#include "set_module.h"
#include "set_codec.h"
#include "task.h"


//~~~~~~~~~~~~~~~SETUP~~~~~~~~~~~~~~~~~//

void setup() 
{
  //~~~~~~~~~~~codec is initialized  See Codec.cpp~~~~~~~~~~~~~~~~~
  //~~~~i2c is initialized within codec.init() with initI2C()~~~~~~
 
      Serial.println("Initialise Codec ");
        codec.init();
        codec_sets();
        Serial.println("Codec Init success!!");

  //~~~~~~I2S is initialized. See set_settings.cpp~~~~~~~~~
 
      I2S_init();
  
  //~~~~~~~~~~~~~~Monitor (can be commented out)~~~~~~~~~~

      Serial.begin(115200);
      delay(1000);
       
      Serial.println("I2S setup complete");
      runSystemMonitor();  //for testing only

} //Setup End


//~~~~~~~~~~~~~~~MAIN LOOP~~~~~~~~~~~~~~~~~~~~~

void loop() 
{

  //leave the main loop dedicated only to the I2S audio task

  size_t readsize = 0;
  int16_t rxbuf[FRAMELENGTH], txbuf[FRAMELENGTH]; //128 L+R signed 16 bit samples 
  //int32_t rxbuf[FRAMELENGTH], txbuf[FRAMELENGTH]; //128 L+R signed 32 bit samples 
  float rxl, rxr, txl, txr;  //left and right single samples, processed as floats

  myPedal->init();
  taskSetup();

  while(1){   //signal processing loop

      setDebugVars(myPedal->depth, myPedal->freq, 0, 0);

  //gather some input samples into receive buffer from the DMA memory,
  i2s_read(I2S_NUM_0, rxbuf, FRAMELENGTH*2, &readsize, 20);
  
  for (int i=0; i<(FRAMELENGTH); i+=2) {  //process samples one at a time from buffers
    
    rxl = (float) (rxbuf[i]) ;   //convert sample to float
    rxr = (float) (rxbuf[i+1]) ; 

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~stereoChorus Processing~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    delay1.write(rxl);
    delay2.write(rxr); //write anyway, no matter it's stereo or mono input
      
    lfo1.update();
    lfo2.update();
    float dt1 = (1 + lfo1.getOutput())* myPedal->depth;
    float dt2;
    if(myPedal->asynch == 0) //asynchronous
      dt2 = (1 + lfo2.getOutput())* myPedal->depth;
    else  //synchronous
      dt2 = (1 + lfo1.getOutput(myPedal->phaseDiff))* myPedal->depth;

    txl = (0.7 * rxl) + (0.7 * delay1.read(dt1));
    if(myPedal->stereo) //if stereo input
      txr = (0.7 * rxr) + (0.7 * delay2.read(dt2));
    else //if mono
      txr = (0.7 * rxl) + (0.7 * delay1.read(dt2));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

    txbuf[i]   = ((int16_t) txl) ; //convert sample back to integer
    txbuf[i+1] = ((int16_t) txr) ;
  }
  // play processed receive buffer by loading transmit buffer into DMA memory
  i2s_write(I2S_NUM_0, txbuf, FRAMELENGTH*2, &readsize, 20);

} // End of while(1) loop

} // End of Main Loop

