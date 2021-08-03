//harmonic frequency modulation with smaller table size 
import("stdfaust.lib");

oscsin(freq) = rdtable(tablesize, os.sinwaveform(tablesize), int(os.phasor(tablesize,freq)))
with{
	tablesize = 1 << 10;
};

harmfm(cFreq, hRatio, idx) = oscsin(cFreq + oscsin(cFreq*hRatio) * idx*hRatio);

fm = hgroup("[0]FM", harmfm(cFreq, harmRatio, index))
with{
cFreq = hslider("freq", 330, 50, 2000, 0.1) : si.smoo;
harmRatio = hslider("harmonicity Ratio", 2, 0, 10, 0.01) : si.smoo;
index = hslider("index", 375,0,1000,0.01) : si.smoo; 
};

process = fm;


			