#### QuNexus RED Editor and Firmware Changelog

### Editor 2.2.2, Firmware 2.2.1 (2026-08-04)
- New Features/Changes:
	- Migrated to Windows MIDI Services (WMS), with automatic fallback to WinMM on machines without the WMS runtime installed.
	- Firmware updates now use a packetized, chunk-safe transfer with a per-packet identity handshake instead of sending the whole firmware image as one block.
	- Updated firmware to 2.2.1: adds a boot-time tare capture for key sensors to reduce stuck-key false triggers, and fixes a bug where a positive per-key sensitivity offset could permanently zero out pressure, bend, aftertouch, and CC output for that key.
	- The Windows installer now detects an existing installation and offers to remove it before installing, instead of failing with no way forward.
	- Added Help menu items: "Open Log File Location" (debug console output is now also captured to a log file) and, on Windows, "Install Windows MIDI Services" (links to Microsoft's WMS installer page).
- Bug Fixes:
	- Fixed a firmware update over the new packetized transport occasionally being reported as failed even though it completed successfully — the app wasn't waiting for the device's MIDI port to reopen after it reboots from bootloader into application mode.
	- Fixed the Windows installer bundling a years-stale Visual C++ Redistributable that was never actually installed, which could crash the app on first launch on a machine with no compatible runtime present.
	- Fixed the Windows installer silently omitting most of its bundled content (manuals, DAW support scripts, SysEx presets, and the app icon), which left the Start Menu shortcut without an icon.

### 2.2.1 (2024-11-10)
- New Features/Changes:
	- Added new Arpeggiator Velocity Modulation.
		- This mode only works when MPE is not enabled
		- If Velocity ("Velo B") and pressure ("Pres C") are enabled, then each key's PRESSURE will update the velocity of the note being arpeggiated.
	- Editor:
		- Key On and Off threshold values was previously treated as a per-preset setting. Now when changing this setting in the editor, ALL of the presets in the library are updated with the new value, effectively making it a global setting. However the data is still stored in each preset on the device, so for the new settings to take effect, you will need to re-load the presets on your device.
		- Added "Factory Reset" to the hardware menu - this resets cv trims, per key sensitivities, globals, and reloads the four factory presets. This also resets Key On and Key Off thresholds to 20/15 defaults (and all presets in library)
		- Added a "Reset Factory Presets to Defaults" function to the file menu, which will check the library for factory presets and return them to the original defaults
		- Added a "RESET" button to the CV Trims window. 
- Bug Fixes:
	- Toggling through arp modes (play, hold, modulation edit) now correctly returns to play mode
	- Bad CV Trim or LED brightness values are now caught and corrected when the QuNexus powers on

### 2.1.1 beta B (2024-04-010)
- Bug Fixes:
	- Per Key Sensitivites now correctly adjust individual key on/off thresholds as well as gain
	- Global on/off threshold limits and default values increased to fix issues with sticking keys
	- Increased LED blanking for inactive keys. This shouod greatly reduce (but not 100% eliminate) issues with active key LEDs bleeding into inactive keys in dark rooms. Reduce LED brightness in the settings menu to improve this issue.
	- Bend pad now uses a peak detection algorithm which improves performance on most devices. 
	- Bend pad sensitivity is now a multipler. If bend pad isn't hitting the extreme values, increase the multiplier until left and right hit max.
	- Note: Extreme pressure on one side of the bend pad can physically push the other side down, which can bring the bend back to center. A light to moderate amount of pressure on the pad with a sufficient bend pad sensitivity setting should produce consistent and correct results. 

### 2.1.0 (2022-02-24)
- Bug Fixes:
	- Fixed bug with remote LED control

### 2.0.9 (2022-02-01)
- Bug Fixes:
	- Fixed a bug where playing a new chord very quickly in arpeggiator hold mode would sometimes not register all the played notes.
	- Improved how MPE channels are assigned between the sequencer and keyboard when both are played simultaneously.

### 2.0.8 (2022-01-20)
- Bug Fixes:
	- Only send midi realtime STOP if transport is playing
	- Fix pitch CV resetting to 0 on noteoff
	- Fixed LED display issue when routing clock and track outputs to USB1 and Expander ports

### 2.0.7 (2022-01-18)
- New Features/Changes:
	- Added arpeggiator octaves with a range of 1-4 octaves. The Octave +/- keys in the shift menu will increase/decrease range. This feature also works with the step sequencer.
	- Global and Track transpose range increased to +/- 48 semitones, octave +/- keys in transposition menus will inc/dec transposition by 1 octave.
	- Removed all notes off (CC123) messages when transport stops
	- Loading a QuNexus presets will now stop playback and clear all track patterns. 
	- Editor can now modify MPE member channels.
	- MPE member channels limited to 13 (14 channels total in the MPE zone when including the manager channel 1)
	- If sequencer tracks 2 and 3 are assigned to MIDI channels that overlap with the MPE zone (track 1 manager channel + member channels) then the track 2 and/or 3 MIDI channels will be reassigned to channels 15 or 16. 
	- "QuNexus MIDI Thru" port on settings tab now remembers the last device it was set to, and will automatically connect when it is detected.
	- Windows 11 now tested/supported
- Bug Fixes:
	- Added buffer overflow checks to improve playback when generating a lot of MPE data
	- Many improvements to Editor<->QuNexus menu and ui synchronization.
	- Patterns now properly load and save to all slots, including pattern length.
	- Fixed a bug where program changes would only send if track 1 had a PC enabled.
	- Changing from arp->seq or seq->arp from hardware or editor now completely clears the pattern and any toggled notes.
	- QuNexus now updates editor when MPE member channels are changed.
	- Patterns now consistently show slots that have been saved during playback.
	- Fixes around how hold behaves when step sequencing, holding keys, and shifting octaves
	- Hold mode now turns on by default when loading a pattern into the arpeggiator.
	- Fixed out of range errors and false crash alerts on MacOS.
	- Firmware update process on Windows improved, refreshing the MIDI driver when entering/exiting bootloader will force the editor to re-launch.

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
