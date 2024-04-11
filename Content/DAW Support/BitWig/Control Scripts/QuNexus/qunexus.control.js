/*

Copyright 2014 Evan Bogunia_____evanbeta@keithmcmillen.com


*/



//Load the bitwig API, obviously.
loadAPI(1);

//Define/set our controller properties [ company, device, version, uuid ]
host.defineController("Keith McMillen Instruments", "QuNexus", "2.0", "860FA290-9111-11E3-BAA8-0800200C9A66");
host.defineMidiPorts(3, 3);

//Define/set input/output port names (both i/o are the same)
var portNames 	= 	["QuNexus Control Surface", "QuNexus Expander", "QuNexus CV"];
host.addDeviceNameBasedDiscoveryPair(portNames, portNames);

//Define/set sysex call/response (deprecated, included for good measure)
host.defineSysexDiscovery("F0 7E 7F 06 01 F7", "F0 7E 00 06 02 00 01 5F 19 00 00 00 ?? ?? ?? ?? ?? ?? F7"); 

//------------------------------------ Init -----------------------------------//
function init()
{
	//-------- Set MIDI callbacks / port
	host.getMidiInPort(0).setMidiCallback(onMidiPort1);
	
	
	//-------- Note 
	noteIn = host.getMidiInPort(0).createNoteInput("",
      "8?????",
      "9?????",
      "B?????",
      "B?????",
      "C?????",
      "D?????",
      "E?????");
	noteIn.setUseMultidimensionalPolyphonicExpression(true,0);
	noteIn.setUseExpressiveMidi(true, 0, 12);
	noteIn.setShouldConsumeEvents(false);

	//tracks = host.createTrackBank(4,2,4)
	println("This is the QuNexus 2.0 Script")	
	
	var ranges = ["12", "24", "36", "48", "60"];
   	bendRange = host.getPreferences().getEnumSetting("Bend Range", "MIDI", ranges, "12");
   	bendRange.addValueObserver(function (selected_range)
   	{
      	var bend = parseInt(selected_range);
      	noteIn.setUseExpressiveMidi(true, 0, bend);
   	});
}

//--------------------------- MIDI Callbacks / Port ---------------------------//
function onMidiPort1(status, data1, data2)
{
	/*
	if(status == 185 && data1 == 125){
		for (i = 0; i < 4; i ++){
			if(i == data2){
				trackToArm = tracks.getTrack(i);
				trackToArm.getArm().set(true);
				trackToArm.selectInMixer();
			}else{
				tracks.getTrack(i).getArm().set(false);
			}

		}
		

	}
	*/
	//println("Port 1 [status, data1, data2]: " + status + ", " + data1 + ", " + data2);

}


function exit()
{
	println("exit.");
}






