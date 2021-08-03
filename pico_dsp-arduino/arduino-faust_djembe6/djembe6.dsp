import("stdfaust.lib");
A = pm.djembe(freq,strikePosition,strikeSharpness,gain,trigger);

freq = hslider("[0]freq", 130, 50, 800, 0.1) ;
strikePosition = hslider("[2]stPos", 0.5, 0.1, 1, 0.01) ;
strikeSharpness = hslider("[3]Sharp", 0.5, 0.1, 1, 0.01) ;
gain = 0.7; 
trigger = (ba.pulsen (1,8000)); 

process = A <:_ , _;
			