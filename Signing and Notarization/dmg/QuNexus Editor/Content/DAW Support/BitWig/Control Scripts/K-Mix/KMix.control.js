//Load the bitwig API
loadAPI(1);

//Define/set our controller properties [ company, device, version, uuid ]
host.defineController("Keith McMillen Instruments", "K-Mix", "1.0", "4034A480-F81D-11E5-A837-0800200C9A66");
host.defineMidiPorts(1,1);

//Define/set input/output port names
host.addDeviceNameBasedDiscoveryPair(["K-Mix Control Surface"],["K-Mix Control Surface"]);

var transport;
var position = 0;
var rewind_length = 1.0 //this is a value in bars

function init()
{
     //Setting the MIDI callback, and creating our transport object.
     host.getMidiInPort(0).setMidiCallback(onMidiPort1);
     transport = host.createTransport();
     //getting the transport position.
     time = transport.getPosition();
     time.addRawValueObserver(function(value)
     {
          position = Math.round(value);
     })
}

function onMidiPort1(status, data1, data2)
{
println("Port 1 [status, data1, data2]: " + status + ", " + data1 + ", " + data2);
}

if(status == 144){
     if(data1 == 93){
          //Stopping the transport
          transport.stop();
     }else if(data1 == 94){
          //playing the transport
          transport.play();
     }else if(data1 == 95){
          //arming the session
          transport.record();
     }else if(data1 == 91){
          //setting the new transport position
          time.setRaw(position - rewind_length)
          transport.rewind();
     }
}