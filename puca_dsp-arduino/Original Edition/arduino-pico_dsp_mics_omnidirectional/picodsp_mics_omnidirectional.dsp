import("stdfaust.lib");
fs = 48000; 

// pico dsp left(L) microphone is the one closest to the TRS connectors, right(R) microphone is the one closest to the RF antenna  //

L = _ : fi.dcblocker;  // adjust for mic input dc offset, which is ~ 0.7v
R = _ : fi.dcblocker; 

// omnidirectional //
process = L, R;   // comment out this line for both mics in omnidirectional configuration 

// broadside array (sound at 90 and 270 degrees off axis is attenuated) //
// (for an array with 2 microphones where d (the distance between the microphones) is = 14.29mm and /// the speed of sound(c) is 343 m/sec, there should be a perfect null at ~ 12kHz (0.5 x c/fs)
// (343 m/sec ÷ (0.01429 m × 2) = 12kHz  
//process = L, R : +; // comment out this line for broadside array configuration

// differential endfire array (directional, cardiod/hypercardiod pickup pattern) // 
// delay the signal from the back microphone by the same time that it takes the sound to travel between the two microphones, and subtract it (or invert and sum) from the front microphone signal  
// mic spacing is (d = n x c/fs), where n = number of samples to delay // 14.29 = 2 x (343/48000)
//process = L @3, R: -;  // differential endfire array 
//process = L @2 *-1, R: +;  // differential endfire array, invert and sum
			
