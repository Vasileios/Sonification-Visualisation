// =====================================================================
// SuperCollider Workspace
// =====================================================================

// VA0517 Sonification Magnetic Storm NOA

s.boot

a = CSVFileReader.read("../data/MagneticStorm12-15\ March2016_NOA\'s\ magnetometer/VLI_20161012_000000.dat.txt");

//: 	~parameters = a[7][0];
	~date = a[9][0];
	~time = a[9][1];
	~xNt = a[9][2];
	~yNt = a[9][3];
	~zNt = a[9][4];
	~tSc = a[9][5];
	~tEc = a[9][6];
	// 
	~all01 = a[9..10][0];
	~all02 = a[10..10][0];
~all03 = a[11..10][0];

//:

// OSC send to other application (OF)

b = NetAddr.new("127.0.0.1", 12345); Ê Ê// create the NetAddr

//-------------------OSC RESPONDER:: SC->OF ----------------------//  
~openframeworks = NetAddr("127.0.0.1", 12345);

~openframeworks.sendMsg("\data", 100);

//: Another example

~files = "../data/MagneticStorm12-15\ March2016_NOA\'s\ magnetometer/*.dat.txt".pathMatch;

~load = { | path |
	var data;
	// select only these rows which contain 7 columns exactly:
	data = CSVFileReader.read(path) select: { | row |
		row.size == 7;
	};
	data.flop[2..4].flop collect: { | row |
		row collect: { | string |
			string.replace("+", "").interpret;
		}
	};
};

//:test in SC
{
	~load.(~files.first) do: { | row |
		row.postln;
		(dur: 0.1, midinote: row.abs).play;
		0.1.wait;
	}
}.fork;

// send to OF
(
~send = {
	~load.(~files.first) do: { | row |
		//row.postln;
	var addr = NetAddr("127.0.0.1", 12345);
		//	(dur: 0.1, midinote: row.abs).play;
		addr.sendMsg("/data", row[1].abs.asFloat.postln);
			0.1.wait;

		//	}).add;
	}
}.fork;
)

