// =====================================================================
// SuperCollider Workspace
// =====================================================================

s.boot

StormData.data;


StormData.playData;


//: Synth

(
SynthDef(\klank01, {|out = 0, gate = 1, vol = 0.0001, freq = 440, freq3 = 999, freq2 = 444  decay = 0.02, cutoff = 2000, amp = 0.01, trig = 1,
         freqs (#[100, 200, 300, 600]), 
         amps (#[0.3, 0.2, 1, 0.05]),
         rings (#[1, 0.1, 0.5, 2]), pan = 0|
         var env, source, filter;
         env = EnvGen.kr(Env([0, 0.8, 0], [2, 2]), gate, doneAction: 2);
         source = DynKlank.ar(`[freqs*freq, amps, rings], Dust.ar(trig)+WhiteNoise.ar(amp)
                              +SinOsc.ar(SinOsc.ar(freq*2, freq2, freq3), 0, 0.3)*SinOsc.ar(SinOsc.ar(freq*2.43, freq2*2/35+12, 1.2.rand+[2000, 200.202]), 0, 0.3)*0.003);
	//	 source = Decay.ar(source, decay, mul: amp).softclip;

	source = LPF.ar(source, cutoff, 0.4, amp).softclip;
	source =  LeakDC.ar(source, 0.995);

         Out.ar(out, Pan2.ar(source*env, pan))
}).add;
)


(
{
	~load.(~files.first) do: { | row |
		//row.postln;
		//		(dur: 0.1, degree: row[0].abs.cos.postln).play;

		var addr = NetAddr("127.0.0.1", 12345);
"TO - SYNTH".postln;
		//	row[1] = row[1]+1.0;
			Synth(\klank01, [\freq, row[0].abs.tan.postln, \freq2, row[1].abs.squared.postln, \freq3, row[2].abs.squared.postln, \cutoff, 6000,  //row[1].abs.pow(2).postln, \amp,
row[1].abs.tan.postln, \legato, 1]);
		
		row[0] = row[0]+320;
		row[1] = row[1]-60;
		"Data-TO-OF-Fluids".postln;
		addr.sendMsg("/data", row[1].abs.asFloat.postln//,row[0].abs.asFloat.postln
		);

	
		row[0] = row[0]+800.0;
		row[1] = row[1]+400.0;
		row[2] = row[2]+900.0;
				"Vertex-TO-OF-3D-Model".postln;

		addr.sendMsg("/vertex", row[0].abs.asFloat.postln,
		row[1].abs.asFloat.postln,
	row[2].abs.asFloat.neg.postln, 1.0.rand, 1.0.rand, 1.0.rand);
	
		0.07.wait;//70 miliseconds
		
		
	}
}.fork;
)
