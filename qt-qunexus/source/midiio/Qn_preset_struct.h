// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
/*
Ported from firmware to QT by EB July 2021
=======================================================================

*/

#ifndef QN_PRESET_STRUCTS_H_
#define QN_PRESET_STRUCTS_H_

//** DEFINE DROP-DOWN MENU ITEMS - Update soon!//

#define MPE_ACTIVE 1
#define LOCAL_LED_MODE 1
#define REMOTE_LED_MODE 0

#define NUM_KEYBOARD_CC 3

#define MAX_SEQ_POLYPHONY 4
#define NUM_TRACKS 3

#define PRESET_STRUCT_IS_V2 36 // magic number to verify that a struct is valid

// clock sources
enum
{
	CK_INTERNAL,
	CK_USB,
	CK_EXPANDER,
	CK_CV1,
	CK_CV2
};


//sources
enum
{
	NONE,
	VELOCITY,
	KEY_PITCH,
	KEY_NUMBER,
	NOTE_ON,
	PRESSURE,
	TILT,
	BEND_PAD,
	EXPRESSION_PEDAL
};

//destinations (CVs have single sources, not listed here)
enum
{
	//DESTINATION_OFF,
	//USB,
	USB1,
	//USB2,
	EXPANDER,
	USB1_AND_EXPANDER,
	DESTINATION_OFF // put at the end for backwards compatibility to 1.0 presets
};

//cv input - midi routing dests.
enum
{
	DUMMY_A, // dummy inserted to conform with conner's editor code
	CV_EXPANDER, // 1 in the editor
	CV_USB3, // 2 in the editor
	DUMMY_B, // 3
	DUMMY_C, // 4
	CV_USB3_AND_EXPANDER, // 5 in the editor
};

//global key modes

/*
enum
{
	NORMAL,
	TOGGLE,
	LEGATO
//	TOGGLE_CLEAR
};
*/

enum
{
	LATEST_KEY,
	EARLIEST_KEY,
	HIGHEST_NOTE,
	LOWEST_NOTE
};

//curves
enum
{
	LINEAR,
	LOG,
	SINE,
	COSINE,
	EXPONENTIAL,
	INVERT,
	USER_1,
	USER_2,
};

#define TRACK1_BIT		(1)
#define TRACK2_BIT		(1<<1)
#define TRACK3_BIT		(1<<2)
#define EXPANDER_BIT 	(1<<3)
#define USB3_BIT 		(1<<4)
#define CLOCK_BIT 		(1<<7)

enum // midiInputDevice
{
	CV_OFF = 0, 										
	TRACK1 = 1,									
	TRACK2 = 2,					
	TRACK3 = 4,		
	ALL_TRACKS = 7, 	
	MIDI_EXPANDER = 8,	
	USB3 = 16,					
	USB3_AND_EXPANDER = 24,
	TRACK1_USB3_EXPANDER = 25,
	TRACK2_USB3_EXPANDER = 26,
	TRACK3_USB3_EXPANDER = 28,
	ALLTRACKS_USB3_EXPANDER = 31,
	CLOCK = 128
};


enum // midiInputType, note/velocity/pb/aftertouch are tacked on at the end
{
	CC_0_CV, 
	CC_1_CV,
	CC_2_CV,
	CC_3_CV,
	CC_4_CV,
	CC_5_CV,
	CC_6_CV,
	CC_7_CV,
	CC_8_CV,
	CC_9_CV,
	CC_10_CV,
	CC_11_CV,
	CC_12_CV,
	CC_13_CV,
	CC_14_CV,
	CC_15_CV,
	CC_16_CV,
	CC_17_CV,
	CC_18_CV,
	CC_19_CV,
	CC_20_CV,
	CC_21_CV,
	CC_22_CV,
	CC_23_CV,
	CC_24_CV,
	CC_25_CV,
	CC_26_CV,
	CC_27_CV,
	CC_28_CV,
	CC_29_CV,
	CC_30_CV,
	CC_31_CV,
	CC_32_CV,
	CC_33_CV,
	CC_34_CV,
	CC_35_CV,
	CC_36_CV,
	CC_37_CV,
	CC_38_CV,
	CC_39_CV,
	CC_40_CV,
	CC_41_CV,
	CC_42_CV,
	CC_43_CV,
	CC_44_CV,
	CC_45_CV,
	CC_46_CV,
	CC_47_CV,
	CC_48_CV,
	CC_49_CV,
	CC_50_CV,
	CC_51_CV,
	CC_52_CV,
	CC_53_CV,
	CC_54_CV,
	CC_55_CV,
	CC_56_CV,
	CC_57_CV,
	CC_58_CV,
	CC_59_CV,
	CC_60_CV,
	CC_61_CV,
	CC_62_CV,
	CC_63_CV,
	CC_64_CV,
	CC_65_CV,
	CC_66_CV,
	CC_67_CV,
	CC_68_CV,
	CC_69_CV,
	CC_70_CV,
	CC_71_CV,
	CC_72_CV,
	CC_73_CV,
	CC_74_CV,
	CC_75_CV,
	CC_76_CV,
	CC_77_CV,
	CC_78_CV,
	CC_79_CV,
	CC_80_CV,
	CC_81_CV,
	CC_82_CV,
	CC_83_CV,
	CC_84_CV,
	CC_85_CV,
	CC_86_CV,
	CC_87_CV,
	CC_88_CV,
	CC_89_CV,
	CC_90_CV,
	CC_91_CV,
	CC_92_CV,
	CC_93_CV,
	CC_94_CV,
	CC_95_CV,
	CC_96_CV,
	CC_97_CV,
	CC_98_CV,
	CC_99_CV,
	CC_100_CV,
	CC_101_CV,
	CC_102_CV,
	CC_103_CV,
	CC_104_CV,
	CC_105_CV,
	CC_106_CV,
	CC_107_CV,
	CC_108_CV,
	CC_109_CV,
	CC_110_CV,
	CC_111_CV,
	CC_112_CV,
	CC_113_CV,
	CC_114_CV,
	CC_115_CV,
	CC_116_CV,
	CC_117_CV,
	CC_118_CV,
	CC_119_CV,
	CC_120_CV,
	CC_121_CV,
	CC_122_CV,
	CC_123_CV,
	CC_124_CV,
	CC_125_CV,
	CC_126_CV,
	CC_127_CV,
	NOTE_ON_CV, // 128
	VELOCITY_CV,
	PRESSURE_CV,
	BEND_CV,
	TILT_CV,	// 132 - new in 2.0
	GATE_CV,	// treat CV out as a gate from a noteoff source
	NO_CV		// turn this CV off
};


enum
{
	ONE_VOLT_PER_OCTAVE,
	ONE_POINT_TWO_VOLTS_PER_OCTAVE,
	HERTZ_PER_VOLT
};

enum
{
	LED_REFRESH_MODE_NORMAL,
	LED_REFRESH_MODE_OFF,
	LED_REFRESH_MODE_SILENT,	 	
};

enum
{
	GATE_NORMAL,
	GATE_LEGATO,	
};

enum 
{
	STANDARD_GATE,
	S_TRIG,
};


//******************************
// global settings for qunexus
//******************************
struct GLOBALS_STRUCT // 22
{
	unsigned char globalGain,
	globalOffLevel,
	globalOnLevel,
	globalTiltSensitivity,
	globalKeyMode,	// 0: normal; 1: toggle; 2: toggle/clear; 3: legato
	globalMPE,	// 0: MPE off; 1: MPE on
	globalKeyboardLayerUSB1Channel,	// midi channel for usb 1 if channel rotation is off -- Deprecated in 2.0, moved to tracks
	globalKeyboardLayerUSB2Channel,	// midi channel for usb 2 (midi expander) if channel rotation is off -- Deprecated in 2.0, moved to tracks
	globalKeySelectionCriteria,	// 0 - last key pressed; 1 - first key pressed; 2 - lowest note; 3 - highest note
	globalVelocityActive,
	globalPressureActive,
	globalTiltActive,
	globalChPressureActive,
	globalPitchBendActive,
	globalPolyAftertouchActive,
	globalCCActive,
   	globalMPEOffset,
	globalLEDMode,
	globalLEDChannel,
	globalPolyphonyNumber,
	globalLEDRefreshMode;

	signed char globalTranspose;
};


//******************************
// keyboard layer structs
//******************************

struct MODLINE_CONSTANTS
{
	unsigned char curve;
	signed int gain;
	unsigned char max;
	unsigned char min;
	signed char offset;
};

//**** struct for the three cc's

struct KEYBOARD_LAYER_CC_SETTINGS
{
	unsigned char keyboardCCControlNumber;
	struct MODLINE_CONSTANTS modlineConstants;
	unsigned char keyboardCCMIDIOutputDevice;
	unsigned char keyboardCCSource;
};

//**** struct for channel pressure settings
struct KEYBOARD_LAYER_CHANNEL_PRESSURE_SETTINGS
{
	struct MODLINE_CONSTANTS modlineConstants;
	unsigned char keyboardChannelPressureMIDIOutputDevice;
	unsigned char keyboardChannelPressureSource;
};

//**** struct for pitch bend settings
struct KEYBOARD_LAYER_PITCH_BEND_SETTINGS
{
	struct MODLINE_CONSTANTS modlineConstants;
	unsigned char keyboardPitchBendMIDIOutputDevice;
	unsigned char keyboardPitchBendSource;
};

//**** struct for poly aftertouch settings
struct KEYBOARD_LAYER_POLY_AFTERTOUCH_SETTINGS
{
	struct MODLINE_CONSTANTS modlineConstants;
	unsigned char keyboardPolyAftertouchMIDIOutputDevice;
	unsigned char keyboardPolyAftertouchSource;
};

//**** struct for velocity settings
struct KEYBOARD_LAYER_VELOCITY_SETTINGS
{
	struct MODLINE_CONSTANTS modlineConstants;
	unsigned char keyboardVelocityMIDIOutputDevice;
};

//****************************************************************
//*** controller layer
//****************************************************************
struct CONTROLLER_LAYER_BEND_PAD
{
	unsigned char controllerLayerActive,
	keyboardLayerParticipate,
	noteNumber,
	pressureCCNumber,
	tiltCCNumber,
	midiChannel,
	velocityValue,
	pressureReturnValue,
	tiltReturnValue;
};

struct CONTROLLER_LAYER_KEY
{
	unsigned char controllerLayerActive,
	keyboardLayerParticipate,
	noteNumber,
	pressureCCNumber,
	tiltCCNumber,
	midiChannel,
	velocityValue,
	pressureReturnValue,
	tiltReturnValue,
	toggleCCNumber,
	toggleValue,
	toggleReturnValue,
	toggleEnabled;	
};


///*** cv layer************
//***** cv input struct
struct CV_INPUT
{
	unsigned char channel;
	struct MODLINE_CONSTANTS modlineConstants; // curve gain max min offset
	unsigned char midiOutputDevice;
	unsigned char ccNumber; 
};

struct CV_OUTPUT
{
	unsigned char channel;
	struct MODLINE_CONSTANTS modlineConstants; // curve gain max min offset
	unsigned char midiInputDevice;
	unsigned char midiInputType;
};

//*** Sequencer and Track preset Structs

typedef struct 
{
	unsigned char mode : 4;						// mode (off/arp/step)
	unsigned char polyphony : 4;				// polyphony (0 disabled, 1-4 or possible 1-8 in the future)
	
	unsigned char patternPlay : 4;				// play patterns
	unsigned char patternSeq : 4;				// pattern for step sequencer (1-8?)
	
	unsigned char division : 4; 				// division (1/1, 1/2, 1/4, 1/8, 1/16, 1/32)
	unsigned char dotTrip : 4;					// 4 doted or triplets
	
	unsigned char gatePercent;					// whole percentage for gate without decimal (50 = .5)

	unsigned char chainX;						// first 8 bits of the pattern chain
	unsigned char chainY;						// second 8 bits of the pattern chain
	unsigned char chainZ;						// third 8 bits of the pattern chain
	
	unsigned char length;    					// length of the arp/seq, 0 = empty
	
	unsigned char dest : 2;						// 2 bits destination (USB/Expander),
	unsigned char channel : 6;					// 6 bits MIDI channel (16 = MPE)
	signed char transpose;						// per track transposition
	
} TRACK_PRESET;

typedef TRACK_PRESET * TRACK_PRESET_POINTER;

typedef struct 
{
    unsigned char valid;                    // must equal a specific value, test for valid 2.0 firmwar struct
    unsigned char clock_source : 4;				// 4 bits are the sources (int/usb/exp)
	unsigned char clock_dest : 4;				// 4 bits are the destinations (usb/exp) 
	unsigned int BPM;
	unsigned char swingPercent;
	unsigned char kbSplit;					// keyboard can be split between two tracks
	unsigned char kbSplitLower : 4;			// which track is assigned to the lower half of the split
	unsigned char kbSplitUpper : 4;			// which track is assigned to the upper half of the split

	TRACK_PRESET track[3];

} SEQUENCER_PRESET;

//*** main device struct ***
struct QUNEXUS
{
	unsigned char presetNum;
    unsigned char programChangeNumber[4]; // 5
	//** globals
    struct GLOBALS_STRUCT global; // 27

	//** keyboard layer
	struct KEYBOARD_LAYER_CC_SETTINGS keyboardLayerCCSettings[NUM_KEYBOARD_CC];
	struct KEYBOARD_LAYER_CHANNEL_PRESSURE_SETTINGS keyboardLayerChannelPressureSettings;
	struct KEYBOARD_LAYER_PITCH_BEND_SETTINGS keyboardLayerPitchBendSettings;
	struct KEYBOARD_LAYER_POLY_AFTERTOUCH_SETTINGS keyboardLayerPolyAftertouchSettings;
	struct KEYBOARD_LAYER_VELOCITY_SETTINGS keyboardLayerVelocitySettings;

	//** controller layer
	unsigned char controllerLayerMidiOutputDevice;
	struct CONTROLLER_LAYER_KEY controllerLayerKey[25];
	struct CONTROLLER_LAYER_BEND_PAD controllerLayerBendPad;
	
	//** cv layer
	struct CV_INPUT cvInput[2];
	struct CV_OUTPUT cvOutput[3];
	unsigned char gateInputDevice;
	unsigned char gateInputChannel;
	unsigned char cvPitchStandard;
	unsigned char gateArticulation;
	unsigned char gateType;
	
	//** 2.0 additions, put here to maintain backwards compatibility
	SEQUENCER_PRESET seq;
};

#endif 
