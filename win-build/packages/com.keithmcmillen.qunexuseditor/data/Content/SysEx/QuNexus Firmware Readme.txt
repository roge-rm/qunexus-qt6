Note: Windows does not properly display USB MIDI portname descriptors, and some applications will report "QuNexus" for the control surface, and "MIDIIN2 (QuNexus)" for the expander, and "MIDIIN3 (QuNexus)"" for the CV ports.


SysEx should be sent to the "QuNexus Control Surface" (MacOS) or "QuNexus" MIDI ports.


##########################################
QunexusEnterBootloader.syx
##########################################
Send this command first to put the QuNexus into bootloader mode. The shift button will light up blue to confirm.

##########################################
QuNexus_Firmware_v2.1.0.syx
##########################################
Once in bootloader mode, send this firmware image to the QuNexus to update the firmware. 


##########################################
Free utilities for sending SysEx files 
to MIDI devices:
##########################################

Windows:
http://miostools.midibox.org/

MacOS:
https://www.snoize.com/sysexlibrarian/
