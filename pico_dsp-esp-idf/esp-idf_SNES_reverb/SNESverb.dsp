// For NES-like reverb sound, you can use a single delay line, very short time ~0.15, no feedback, width set to 0.5.
// For SNES-like reverb sound, you can play with longer delay times and add some feedback to simulate a reverb tail.

import("stdfaust.lib");

delaytime = hslider("[1]delay", 0.15, 0.1, 0.3, 0.01) : si.smoo;
feedback = hslider("[2]feedback", 0.1, 0.1, 0.3, 0.01) : si.smoo;   

//=============================================

width = _ <: *(1-pan),*(pan)
with{
	pan = hslider("width",0.5,0,1,0.01) : si.smoo;
};

process = _,_ : + <: _,(( + : de.sdelay(ba.sec2samp(0.5), 64, ba.sec2samp(delaytime))) ~* (feedback)) * 0.5 :> _ <: width: _,_;
			