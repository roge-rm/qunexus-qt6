#### QuNexus RED Editor and Firmware Changelog

### 2.0.6 (2021-11-23)
- New Features/Changes:
	- Keyboard->MIDI latency reduced by 10ms, now down to ~2.5ms
	- Big improvements to the arpeggiator "Flow" when playing in arp hold, keys more naturally capture "rolled" chords
	- Improved stability during playback when connected to editor
		- Reduced size of sysex messages when adjusting a single parameter (now only 17 bytes)
		- Sending a full ~600 byte preset message from the editor to QuNexus (ie when loading a new preset) will now stop playback rather than break sequencer synchronization
	- Improved stability when saving patterns during playback
		- Saving patterns during playback is limited to preset slots 1-4, and you can only save to a preset slot once during playback. This is to avoid sequencer synchronization failure during 20ms memory page writes.
	- Saving a preset on the QuNexus hardware will now stop playback
	- Adjusting editor global preset data (Per Key Sensitivities, CV Trims, User Tables) now stops playback rather than break sync
	- From this version forward, editor presets will now be preserved when updating the editor. It is recommended to back up any editor presets before updating to 2.0.6
	- Enabling pressure/tilt when the current preset does not have any sources selected now enables channel aftertouch (pressure) and mod wheel (tilt)
	- Pressure and Tilt values now properly latch to previous values when keys are released
- Bug Fixes:
	- UI Improvements, editor number boxes now easily select with a single click
	- Firmware update improvements, better detection of bootloader state and fixed hang during update
	- "Cancel" button when closing the editor now works
	- Changing from arp->seq now clears notes/pattern
	- Fixed a bug where clearing a track would send CC123 (all notes off) to incorrect MIDI channels
	- Changing channels during playback now doesn't cause stuck notes
	- Pitch bend now works in MPE when arpeggiating, and sends on proper manager and member channels
	- Fixed a bug where random pitch bend values would send when tilt sensitivity was set below 50
	- Fixed a bug where the 5th ocatve C key would not reach 5V on CV outputs
	- 5x increase to USB send efficiency, more messages put into the buffer in less time
	- Added message prioritization and buffer reservation for Clock and MIDI Note on/off messages to avoid clock drift and stuck notes during "extreme" MPE playback
	- Fixed a rounding bug when incrementing/decrementing tempo that would cause values above 250 to get stuck (happy hardcore fans rejoice)
	- Fixed a bug where extreme pitch bend pressure would cause bad values


### 2.0.5
This version was used for internal/beta testing and was skipped in release

### 2.0.4 (2021-10-18)
- New Features/Changes:
	- Editor now checks for updates
- Bug Fixes:
	- Renamed USB MIDI ports to "Control Surface", "Expander", and "CV". This fixed issues where non-english operating systems translated the port names to something the editor wouldn't recognize.
	- Fixed a bug with CV input being clipped at 120
	- Fixed a bug where control layer notes would not un-toggle
	- Fixed a bug where rests would send Note C#-2 instead of resting
	- Fixed a bug where pressure/tilt would send initial values even if pressure/tilt was disabled on the keyboard

### 2.0.3 (2021-10-08)
- Bug Fixes:
	- CV Trims now reset to default values after firmware update to fix junk values from 1.0->2.0 firmware

### 2.0.2 (2021-10-03)
Editor released to public with 2.0.2 firmware
- New Features/Changes:
	- Tilt/pressure values are now captured and played back by the arpeggiator/step sequencer
	- Added external MIDI clock sync
	- Added CV clock input sync
	- Added CV Gate clock out divisions (1/2/3/24 PPQN)
	- Added Bend Pad and Pressure sensitivity adjustment
	- Long pressing octave -/+ buttons in shift menu disables/enables LEDs
	- MPE Control Message (MCM) implemented, QuNexus sends and responds to this message to/from other MPE devices
- Bug Fixes:
	- CV pitch trims and offsets now apply to all three CV outputs
	- Fixed a bug where blinking buttons in the shift menu would keep blinking after shift was released
	- Fixed downward patterns getting hung up during step sequencing

### 2.0.1 (2021-08-30)
Initial firmware sent to factory for programming/shipment
