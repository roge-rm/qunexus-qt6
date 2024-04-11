/*

Copyright 2017 Keith McMillen Instrumets - Evan Bogunia_____evanbeta@keithmcmillen.com

*/

//A Simple script allowing for MPE note input for KBP4 four. 
loadAPI(1);

//Define/set our controller properties [ company, device, version, uuid ]
host.defineController("Keith McMillen Instruments", "BopPad", "1.0", "1DCEFB1A-B19C-4404-AA50-FEC2B11F3169");
host.defineMidiPorts(1, 1);

//Define/set input/output port names (both i/o are the same)
var portNames 	= 	["BopPad"];
host.addDeviceNameBasedDiscoveryPair(portNames, portNames);

//------------------------------------ Init -----------------------------------//
function init()
{
	//-------- Set MIDI callbacks / port
	host.getMidiInPort(0).setMidiCallback(onMidiPort1);
	
	
	//-------- Note Inputs (see REF below for argument details
	noteIn = host.getMidiInPort(0).createNoteInput("BopPad","8?????",
"9?????",
"B?40??",
"B?4A??",
"C?????",
"D?????",
"E?????");
	noteIn.setUseMultidimensionalPolyphonicExpression(true,0);
	noteIn.setUseExpressiveMidi(true, 0, 12);
	noteIn.setShouldConsumeEvents(false);

	println("This is the BopPad Script")
}

//--------------------------- MIDI Callbacks / Port ---------------------------//
function onMidiPort1(status, data1, data2)
{
	
	//println("Port 1 [status, data1, data2]: " + status + ", " + data1 + ", " + data2);

}


function exit()
{
	println("exit.");
}
/*
"8?????",
"9?????",
"B?40??",
"B?4A??",
"C?????",
"D?????",
"E?????"

*/