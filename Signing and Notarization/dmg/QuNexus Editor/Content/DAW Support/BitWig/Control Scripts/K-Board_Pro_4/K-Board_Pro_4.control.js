/*

Copyright 2017 Keith McMillen Instrumets - Evan Bogunia_____evanbeta@keithmcmillen.com

*/

//A Simple script allowing for MPE note input for KBP4 four. 
loadAPI(1);

//Define/set our controller properties [ company, device, version, uuid ]
host.defineController("Keith McMillen", "K-Board Pro 4", "1.0", "842C2587-BC05-4D94-9CA1-4AA1A069142C");
host.defineMidiPorts(1, 1);

//Define/set input/output port names (both i/o are the same)
var portNames 	= 	["K-Board Pro 4 Control Surface"];
host.addDeviceNameBasedDiscoveryPair(portNames, portNames);

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
	println("This is the K-Board Pro 4 Script")	
	
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



