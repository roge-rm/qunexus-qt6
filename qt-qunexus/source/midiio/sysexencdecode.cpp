// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "sysexencdecode.h"
#include "mainwindow.h"

#define SIZE_OF_PRESET_HEADER 8
#define SIZE_OF_OLD_PRESET_JSON 457
#define SYX_DEBUG_PRESET_PACKET

SysExEncDecode::SysExEncDecode(QWidget *parent) :
    QWidget(parent)
{

    thisParent = parent;
    qnSeq.valid = PRESET_STRUCT_IS_V2; // magic number to prove this struct is a valid 2.0 format

    ///////////////////////////////////////
    ///////////_midi_sx_encode ////////////
    ///////////////////////////////////////

    size = 0;
    midi_hi_bits = 0;
    midi_hi_count = 0;
    crc = 0;

    SX_PACKET_START = 0x01;
    SX_ENCODE_LEN   = 0x07;

    sx_xmit_active = 0;

    sx_ident.insert("manufacturer_id1", 0x00);
    sx_ident.insert("manufacturer_id2", 0x01);
    sx_ident.insert("manufacturer_id3", 0x5F);
    sx_ident.insert("manufacturer_id4", 0x7A);
    sx_ident.insert("product", 25);
    sx_ident.insert("format", 0x00);

    sysex_start = 0xF0;
    sysex_end   = 0xF7;

    ///////////////////////////////////////
    /////////////_structList //////////////
    ///////////////////////////////////////

    keyLayerJSON        // this is the list of parameter names that get sent to the QuNexus hardware
                        // their order determines the order of the preset payload, param names are pulled from
                        // the preset map
            //******** globals *********

            <<
               "Keyboard_Global_Program_Change_A"<<
               "Keyboard_Global_Program_Change_B"<<
               "Keyboard_Global_Program_Change_C"<<
               "Keyboard_Global_Program_Change_D"<<
               "Globals_Gain"<<
               "Globals_Off_Thresh"<<
               "Globals_On_Thresh"<<
               "Globals_Tilt_Sensitivity"<<
               "Keyboard_Global_Mode"<<
               "Keyboard_Global_Channel_Rotation_Active"<<
               "Keyboard_Global_USB_1_Channel"<<
               "Keyboard_Global_USB_2_Channel"<<
               "Keyboard_Global_Key_Selection_Criteria"<<
               "Keyboard_Global_Velocity_Active"<<
               "Keyboard_Global_Pressure_Active"<<
               "Keyboard_Global_Tilt_Active"<<
               "Keyboard_Global_Channel_Pressure_Active"<<
               "Keyboard_Global_Bend_Pad_Sensitivity"<<
               "Keyboard_Global_Poly_Aftertouch_Active"<<
               "Keyboard_Global_CC_Active"<<
               "Keyboard_Global_Channel_Rotation_Offset"<<
               "Keyboard_Global_LED_Mode"<<
               "Keyboard_Global_LED_Remote_Channel"<<
               "Keyboard_Global_Polyphony_Number"<<
               "Globals_LED_Refresh_Style"<<
               "Keyboard_Global_Transpose"<<

               //********* keyboard layer *********

               // cc 00
               "Keyboard_CC_00_Control_Number"<<
               "Keyboard_CC_00_Curve"<<
               "Keyboard_CC_00_Gain"<<
               "Keyboard_CC_00_Max"<<
               "Keyboard_CC_00_Min"<<
               "Keyboard_CC_00_Offset"<<
               "Keyboard_CC_00_MIDI_Output_Device"<<
               "Keyboard_CC_00_Source"<<

               // cc 01
               "Keyboard_CC_01_Control_Number"<<
               "Keyboard_CC_01_Curve"<<
               "Keyboard_CC_01_Gain"<<
               "Keyboard_CC_01_Max"<<
               "Keyboard_CC_01_Min"<<
               "Keyboard_CC_01_Offset"<<
               "Keyboard_CC_01_MIDI_Output_Device"<<
               "Keyboard_CC_01_Source"<<

               // cc 02
               "Keyboard_CC_02_Control_Number"<<
               "Keyboard_CC_02_Curve"<<
               "Keyboard_CC_02_Gain"<<
               "Keyboard_CC_02_Max"<<
               "Keyboard_CC_02_Min"<<
               "Keyboard_CC_02_Offset"<<
               "Keyboard_CC_02_MIDI_Output_Device"<<
               "Keyboard_CC_02_Source"<<

               // channel pressure

               "Keyboard_Channel_Pressure_Curve"<<
               "Keyboard_Channel_Pressure_Gain"<<
               "Keyboard_Channel_Pressure_Max"<<
               "Keyboard_Channel_Pressure_Min"<<
               "Keyboard_Channel_Pressure_Offset"<<
               "Keyboard_Channel_Pressure_MIDI_Output_Device"<<
               "Keyboard_Channel_Pressure_Source"<<

               // pitch bend

               "Keyboard_Pitch_Bend_Curve"<<
               "Keyboard_Pitch_Bend_Gain"<<
               "Keyboard_Pitch_Bend_Max"<<
               "Keyboard_Pitch_Bend_Min"<<
               "Keyboard_Pitch_Bend_Offset"<<
               "Keyboard_Pitch_Bend_MIDI_Output_Device"<<
               "Keyboard_Pitch_Bend_Source"<<

               // poly aftertouch

               "Keyboard_Poly_Aftertouch_Curve"<<
               "Keyboard_Poly_Aftertouch_Gain"<<
               "Keyboard_Poly_Aftertouch_Max"<<
               "Keyboard_Poly_Aftertouch_Min"<<
               "Keyboard_Poly_Aftertouch_Offset"<<
               "Keyboard_Poly_Aftertouch_MIDI_Output_Device"<<
               "Keyboard_Poly_Aftertouch_Source"<<

               // velocity

               "Keyboard_Velocity_Curve"<<
               "Keyboard_Velocity_Gain"<<
               "Keyboard_Velocity_Max"<<
               "Keyboard_Velocity_Min"<<
               "Keyboard_Velocity_Offset"<<
               "Keyboard_Velocity_MIDI_Output_Device"<<

               //******** controller layer *********

               "Controller_Global_MIDI_Output_Device"<<

               //******** controller layer keys *********

               "Controller_Key_0_Controller_Layer_Enable_Disable"<<
               "Controller_Key_0_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_0_Note_Number"<<
               "Controller_Key_0_Pressure_Control_Number"<<
               "Controller_Key_0_Tilt_Control_Number"<<
               "Controller_Key_0_Channel"<<
               "Controller_Key_0_Note_Velocity"<<
               "Controller_Key_0_Pressure_Return_Value"<<
               "Controller_Key_0_Tilt_Return_Value"<<
               "Controller_Key_0_Toggle_Control_Number"<<
               "Controller_Key_0_Toggle_Value"<<
               "Controller_Key_0_Toggle_Return_Value"<<
               "Controller_Key_0_Toggle_On_Off"<<

               "Controller_Key_1_Controller_Layer_Enable_Disable"<<
               "Controller_Key_1_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_1_Note_Number"<<
               "Controller_Key_1_Pressure_Control_Number"<<
               "Controller_Key_1_Tilt_Control_Number"<<
               "Controller_Key_1_Channel"<<
               "Controller_Key_1_Note_Velocity"<<
               "Controller_Key_1_Pressure_Return_Value"<<
               "Controller_Key_1_Tilt_Return_Value"<<
               "Controller_Key_1_Toggle_Control_Number"<<
               "Controller_Key_1_Toggle_Value"<<
               "Controller_Key_1_Toggle_Return_Value"<<
               "Controller_Key_1_Toggle_On_Off"<<

               "Controller_Key_2_Controller_Layer_Enable_Disable"<<
               "Controller_Key_2_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_2_Note_Number"<<
               "Controller_Key_2_Pressure_Control_Number"<<
               "Controller_Key_2_Tilt_Control_Number"<<
               "Controller_Key_2_Channel"<<
               "Controller_Key_2_Note_Velocity"<<
               "Controller_Key_2_Pressure_Return_Value"<<
               "Controller_Key_2_Tilt_Return_Value"<<
               "Controller_Key_2_Toggle_Control_Number"<<
               "Controller_Key_2_Toggle_Value"<<
               "Controller_Key_2_Toggle_Return_Value"<<
               "Controller_Key_2_Toggle_On_Off"<<

               "Controller_Key_3_Controller_Layer_Enable_Disable"<<
               "Controller_Key_3_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_3_Note_Number"<<
               "Controller_Key_3_Pressure_Control_Number"<<
               "Controller_Key_3_Tilt_Control_Number"<<
               "Controller_Key_3_Channel"<<
               "Controller_Key_3_Note_Velocity"<<
               "Controller_Key_3_Pressure_Return_Value"<<
               "Controller_Key_3_Tilt_Return_Value"<<
               "Controller_Key_3_Toggle_Control_Number"<<
               "Controller_Key_3_Toggle_Value"<<
               "Controller_Key_3_Toggle_Return_Value"<<
               "Controller_Key_3_Toggle_On_Off"<<

               "Controller_Key_4_Controller_Layer_Enable_Disable"<<
               "Controller_Key_4_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_4_Note_Number"<<
               "Controller_Key_4_Pressure_Control_Number"<<
               "Controller_Key_4_Tilt_Control_Number"<<
               "Controller_Key_4_Channel"<<
               "Controller_Key_4_Note_Velocity"<<
               "Controller_Key_4_Pressure_Return_Value"<<
               "Controller_Key_4_Tilt_Return_Value"<<
               "Controller_Key_4_Toggle_Control_Number"<<
               "Controller_Key_4_Toggle_Value"<<
               "Controller_Key_4_Toggle_Return_Value"<<
               "Controller_Key_4_Toggle_On_Off"<<

               "Controller_Key_5_Controller_Layer_Enable_Disable"<<
               "Controller_Key_5_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_5_Note_Number"<<
               "Controller_Key_5_Pressure_Control_Number"<<
               "Controller_Key_5_Tilt_Control_Number"<<
               "Controller_Key_5_Channel"<<
               "Controller_Key_5_Note_Velocity"<<
               "Controller_Key_5_Pressure_Return_Value"<<
               "Controller_Key_5_Tilt_Return_Value"<<
               "Controller_Key_5_Toggle_Control_Number"<<
               "Controller_Key_5_Toggle_Value"<<
               "Controller_Key_5_Toggle_Return_Value"<<
               "Controller_Key_5_Toggle_On_Off"<<

               "Controller_Key_6_Controller_Layer_Enable_Disable"<<
               "Controller_Key_6_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_6_Note_Number"<<
               "Controller_Key_6_Pressure_Control_Number"<<
               "Controller_Key_6_Tilt_Control_Number"<<
               "Controller_Key_6_Channel"<<
               "Controller_Key_6_Note_Velocity"<<
               "Controller_Key_6_Pressure_Return_Value"<<
               "Controller_Key_6_Tilt_Return_Value"<<
               "Controller_Key_6_Toggle_Control_Number"<<
               "Controller_Key_6_Toggle_Value"<<
               "Controller_Key_6_Toggle_Return_Value"<<
               "Controller_Key_6_Toggle_On_Off"<<

               "Controller_Key_7_Controller_Layer_Enable_Disable"<<
               "Controller_Key_7_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_7_Note_Number"<<
               "Controller_Key_7_Pressure_Control_Number"<<
               "Controller_Key_7_Tilt_Control_Number"<<
               "Controller_Key_7_Channel"<<
               "Controller_Key_7_Note_Velocity"<<
               "Controller_Key_7_Pressure_Return_Value"<<
               "Controller_Key_7_Tilt_Return_Value"<<
               "Controller_Key_7_Toggle_Control_Number"<<
               "Controller_Key_7_Toggle_Value"<<
               "Controller_Key_7_Toggle_Return_Value"<<
               "Controller_Key_7_Toggle_On_Off"<<

               "Controller_Key_8_Controller_Layer_Enable_Disable"<<
               "Controller_Key_8_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_8_Note_Number"<<
               "Controller_Key_8_Pressure_Control_Number"<<
               "Controller_Key_8_Tilt_Control_Number"<<
               "Controller_Key_8_Channel"<<
               "Controller_Key_8_Note_Velocity"<<
               "Controller_Key_8_Pressure_Return_Value"<<
               "Controller_Key_8_Tilt_Return_Value"<<
               "Controller_Key_8_Toggle_Control_Number"<<
               "Controller_Key_8_Toggle_Value"<<
               "Controller_Key_8_Toggle_Return_Value"<<
               "Controller_Key_8_Toggle_On_Off"<<

               "Controller_Key_9_Controller_Layer_Enable_Disable"<<
               "Controller_Key_9_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_9_Note_Number"<<
               "Controller_Key_9_Pressure_Control_Number"<<
               "Controller_Key_9_Tilt_Control_Number"<<
               "Controller_Key_9_Channel"<<
               "Controller_Key_9_Note_Velocity"<<
               "Controller_Key_9_Pressure_Return_Value"<<
               "Controller_Key_9_Tilt_Return_Value"<<
               "Controller_Key_9_Toggle_Control_Number"<<
               "Controller_Key_9_Toggle_Value"<<
               "Controller_Key_9_Toggle_Return_Value"<<
               "Controller_Key_9_Toggle_On_Off"<<

               "Controller_Key_10_Controller_Layer_Enable_Disable"<<
               "Controller_Key_10_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_10_Note_Number"<<
               "Controller_Key_10_Pressure_Control_Number"<<
               "Controller_Key_10_Tilt_Control_Number"<<
               "Controller_Key_10_Channel"<<
               "Controller_Key_10_Note_Velocity"<<
               "Controller_Key_10_Pressure_Return_Value"<<
               "Controller_Key_10_Tilt_Return_Value"<<
               "Controller_Key_10_Toggle_Control_Number"<<
               "Controller_Key_10_Toggle_Value"<<
               "Controller_Key_10_Toggle_Return_Value"<<
               "Controller_Key_10_Toggle_On_Off"<<

               "Controller_Key_11_Controller_Layer_Enable_Disable"<<
               "Controller_Key_11_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_11_Note_Number"<<
               "Controller_Key_11_Pressure_Control_Number"<<
               "Controller_Key_11_Tilt_Control_Number"<<
               "Controller_Key_11_Channel"<<
               "Controller_Key_11_Note_Velocity"<<
               "Controller_Key_11_Pressure_Return_Value"<<
               "Controller_Key_11_Tilt_Return_Value"<<
               "Controller_Key_11_Toggle_Control_Number"<<
               "Controller_Key_11_Toggle_Value"<<
               "Controller_Key_11_Toggle_Return_Value"<<
               "Controller_Key_11_Toggle_On_Off"<<

               "Controller_Key_12_Controller_Layer_Enable_Disable"<<
               "Controller_Key_12_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_12_Note_Number"<<
               "Controller_Key_12_Pressure_Control_Number"<<
               "Controller_Key_12_Tilt_Control_Number"<<
               "Controller_Key_12_Channel"<<
               "Controller_Key_12_Note_Velocity"<<
               "Controller_Key_12_Pressure_Return_Value"<<
               "Controller_Key_12_Tilt_Return_Value"<<
               "Controller_Key_12_Toggle_Control_Number"<<
               "Controller_Key_12_Toggle_Value"<<
               "Controller_Key_12_Toggle_Return_Value"<<
               "Controller_Key_12_Toggle_On_Off"<<

               "Controller_Key_13_Controller_Layer_Enable_Disable"<<
               "Controller_Key_13_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_13_Note_Number"<<
               "Controller_Key_13_Pressure_Control_Number"<<
               "Controller_Key_13_Tilt_Control_Number"<<
               "Controller_Key_13_Channel"<<
               "Controller_Key_13_Note_Velocity"<<
               "Controller_Key_13_Pressure_Return_Value"<<
               "Controller_Key_13_Tilt_Return_Value"<<
               "Controller_Key_13_Toggle_Control_Number"<<
               "Controller_Key_13_Toggle_Value"<<
               "Controller_Key_13_Toggle_Return_Value"<<
               "Controller_Key_13_Toggle_On_Off"<<

               "Controller_Key_14_Controller_Layer_Enable_Disable"<<
               "Controller_Key_14_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_14_Note_Number"<<
               "Controller_Key_14_Pressure_Control_Number"<<
               "Controller_Key_14_Tilt_Control_Number"<<
               "Controller_Key_14_Channel"<<
               "Controller_Key_14_Note_Velocity"<<
               "Controller_Key_14_Pressure_Return_Value"<<
               "Controller_Key_14_Tilt_Return_Value"<<
               "Controller_Key_14_Toggle_Control_Number"<<
               "Controller_Key_14_Toggle_Value"<<
               "Controller_Key_14_Toggle_Return_Value"<<
               "Controller_Key_14_Toggle_On_Off"<<

               "Controller_Key_15_Controller_Layer_Enable_Disable"<<
               "Controller_Key_15_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_15_Note_Number"<<
               "Controller_Key_15_Pressure_Control_Number"<<
               "Controller_Key_15_Tilt_Control_Number"<<
               "Controller_Key_15_Channel"<<
               "Controller_Key_15_Note_Velocity"<<
               "Controller_Key_15_Pressure_Return_Value"<<
               "Controller_Key_15_Tilt_Return_Value"<<
               "Controller_Key_15_Toggle_Control_Number"<<
               "Controller_Key_15_Toggle_Value"<<
               "Controller_Key_15_Toggle_Return_Value"<<
               "Controller_Key_15_Toggle_On_Off"<<

               "Controller_Key_16_Controller_Layer_Enable_Disable"<<
               "Controller_Key_16_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_16_Note_Number"<<
               "Controller_Key_16_Pressure_Control_Number"<<
               "Controller_Key_16_Tilt_Control_Number"<<
               "Controller_Key_16_Channel"<<
               "Controller_Key_16_Note_Velocity"<<
               "Controller_Key_16_Pressure_Return_Value"<<
               "Controller_Key_16_Tilt_Return_Value"<<
               "Controller_Key_16_Toggle_Control_Number"<<
               "Controller_Key_16_Toggle_Value"<<
               "Controller_Key_16_Toggle_Return_Value"<<
               "Controller_Key_16_Toggle_On_Off"<<

               "Controller_Key_17_Controller_Layer_Enable_Disable"<<
               "Controller_Key_17_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_17_Note_Number"<<
               "Controller_Key_17_Pressure_Control_Number"<<
               "Controller_Key_17_Tilt_Control_Number"<<
               "Controller_Key_17_Channel"<<
               "Controller_Key_17_Note_Velocity"<<
               "Controller_Key_17_Pressure_Return_Value"<<
               "Controller_Key_17_Tilt_Return_Value"<<
               "Controller_Key_17_Toggle_Control_Number"<<
               "Controller_Key_17_Toggle_Value"<<
               "Controller_Key_17_Toggle_Return_Value"<<
               "Controller_Key_17_Toggle_On_Off"<<

               "Controller_Key_18_Controller_Layer_Enable_Disable"<<
               "Controller_Key_18_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_18_Note_Number"<<
               "Controller_Key_18_Pressure_Control_Number"<<
               "Controller_Key_18_Tilt_Control_Number"<<
               "Controller_Key_18_Channel"<<
               "Controller_Key_18_Note_Velocity"<<
               "Controller_Key_18_Pressure_Return_Value"<<
               "Controller_Key_18_Tilt_Return_Value"<<
               "Controller_Key_18_Toggle_Control_Number"<<
               "Controller_Key_18_Toggle_Value"<<
               "Controller_Key_18_Toggle_Return_Value"<<
               "Controller_Key_18_Toggle_On_Off"<<

               "Controller_Key_19_Controller_Layer_Enable_Disable"<<
               "Controller_Key_19_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_19_Note_Number"<<
               "Controller_Key_19_Pressure_Control_Number"<<
               "Controller_Key_19_Tilt_Control_Number"<<
               "Controller_Key_19_Channel"<<
               "Controller_Key_19_Note_Velocity"<<
               "Controller_Key_19_Pressure_Return_Value"<<
               "Controller_Key_19_Tilt_Return_Value"<<
               "Controller_Key_19_Toggle_Control_Number"<<
               "Controller_Key_19_Toggle_Value"<<
               "Controller_Key_19_Toggle_Return_Value"<<
               "Controller_Key_19_Toggle_On_Off"<<

               "Controller_Key_20_Controller_Layer_Enable_Disable"<<
               "Controller_Key_20_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_20_Note_Number"<<
               "Controller_Key_20_Pressure_Control_Number"<<
               "Controller_Key_20_Tilt_Control_Number"<<
               "Controller_Key_20_Channel"<<
               "Controller_Key_20_Note_Velocity"<<
               "Controller_Key_20_Pressure_Return_Value"<<
               "Controller_Key_20_Tilt_Return_Value"<<
               "Controller_Key_20_Toggle_Control_Number"<<
               "Controller_Key_20_Toggle_Value"<<
               "Controller_Key_20_Toggle_Return_Value"<<
               "Controller_Key_20_Toggle_On_Off"<<

               "Controller_Key_21_Controller_Layer_Enable_Disable"<<
               "Controller_Key_21_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_21_Note_Number"<<
               "Controller_Key_21_Pressure_Control_Number"<<
               "Controller_Key_21_Tilt_Control_Number"<<
               "Controller_Key_21_Channel"<<
               "Controller_Key_21_Note_Velocity"<<
               "Controller_Key_21_Pressure_Return_Value"<<
               "Controller_Key_21_Tilt_Return_Value"<<
               "Controller_Key_21_Toggle_Control_Number"<<
               "Controller_Key_21_Toggle_Value"<<
               "Controller_Key_21_Toggle_Return_Value"<<
               "Controller_Key_21_Toggle_On_Off"<<

               "Controller_Key_22_Controller_Layer_Enable_Disable"<<
               "Controller_Key_22_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_22_Note_Number"<<
               "Controller_Key_22_Pressure_Control_Number"<<
               "Controller_Key_22_Tilt_Control_Number"<<
               "Controller_Key_22_Channel"<<
               "Controller_Key_22_Note_Velocity"<<
               "Controller_Key_22_Pressure_Return_Value"<<
               "Controller_Key_22_Tilt_Return_Value"<<
               "Controller_Key_22_Toggle_Control_Number"<<
               "Controller_Key_22_Toggle_Value"<<
               "Controller_Key_22_Toggle_Return_Value"<<
               "Controller_Key_22_Toggle_On_Off"<<

               "Controller_Key_23_Controller_Layer_Enable_Disable"<<
               "Controller_Key_23_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_23_Note_Number"<<
               "Controller_Key_23_Pressure_Control_Number"<<
               "Controller_Key_23_Tilt_Control_Number"<<
               "Controller_Key_23_Channel"<<
               "Controller_Key_23_Note_Velocity"<<
               "Controller_Key_23_Pressure_Return_Value"<<
               "Controller_Key_23_Tilt_Return_Value"<<
               "Controller_Key_23_Toggle_Control_Number"<<
               "Controller_Key_23_Toggle_Value"<<
               "Controller_Key_23_Toggle_Return_Value"<<
               "Controller_Key_23_Toggle_On_Off"<<

               "Controller_Key_24_Controller_Layer_Enable_Disable"<<
               "Controller_Key_24_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_24_Note_Number"<<
               "Controller_Key_24_Pressure_Control_Number"<<
               "Controller_Key_24_Tilt_Control_Number"<<
               "Controller_Key_24_Channel"<<
               "Controller_Key_24_Note_Velocity"<<
               "Controller_Key_24_Pressure_Return_Value"<<
               "Controller_Key_24_Tilt_Return_Value"<<
               "Controller_Key_24_Toggle_Control_Number"<<
               "Controller_Key_24_Toggle_Value"<<
               "Controller_Key_24_Toggle_Return_Value"<<
               "Controller_Key_24_Toggle_On_Off"<<


               //******** controller layer bend pad *********

               "Controller_Bend_Pad_00_Controller_Layer_Enable_Disable"<<
               "Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable"<<
               "Controller_Bend_Pad_00_Note_Number"<<
               "Controller_Bend_Pad_00_Pressure_Control_Number"<<
               "Controller_Bend_Pad_00_Tilt_Control_Number"<<
               "Controller_Bend_Pad_00_Channel"<<
               "Controller_Bend_Pad_00_Note_Velocity"<<
               "Controller_Bend_Pad_00_Pressure_Return_Value"<<
               "Controller_Bend_Pad_00_Tilt_Return_Value"<<

               //******** cv layer *********

               //******** cv inputs *********

               "CV_In_CV_1_Channel"<<	// default 1
               "CV_In_CV_1_Curve"<<		// default 0
               "CV_In_CV_1_Gain"<<		// default 100
               "CV_In_CV_1_Max"<< // default 127
               "CV_In_CV_1_Min"<< // default 0
               "CV_In_CV_1_Offset"<< // default 0
               "CV_In_CV_1_MIDI_Output_Device"<< // default USB
               "CV_In_CV_1_CC_Number"<<	// default 126

               "CV_In_CV_2_Channel"<<	// default 1
               "CV_In_CV_2_Curve"<<		// default 0
               "CV_In_CV_2_Gain"<<		// default 100
               "CV_In_CV_2_Max"<< // default 127
               "CV_In_CV_2_Min"<< // default 0
               "CV_In_CV_2_Offset"<< // default 0
               "CV_In_CV_2_MIDI_Output_Device"<< // default USB
               "CV_In_CV_2_CC_Number"<<	// default 127

               //******** cv outputs *********

               "CV_Out_CV_1_Channel"<< // default 1
               "CV_Out_CV_1_Curve"<<	// default 0
               "CV_Out_CV_1_Gain"<< // default 100
               "CV_Out_CV_1_Max"<< // default 127
               "CV_Out_CV_1_Min"<< // default 0
               "CV_Out_CV_1_Offset"<< // default 0
               "CV_Out_CV_1_MIDI_Input_Device"<< //default All
               "CV_Out_CV_1_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_CV_2_Channel"<< // default 1
               "CV_Out_CV_2_Curve"<<	// default 0
               "CV_Out_CV_2_Gain"<< // default 100
               "CV_Out_CV_2_Max"<< // default 127
               "CV_Out_CV_2_Min"<< // default 0
               "CV_Out_CV_2_Offset"<< // default 0
               "CV_Out_CV_2_MIDI_Input_Device"<< //default All
               "CV_Out_CV_2_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_CV_3_Channel"<< // default 1
               "CV_Out_CV_3_Curve"<<	// default 0
               "CV_Out_CV_3_Gain"<< // default 100
               "CV_Out_CV_3_Max"<< // default 127
               "CV_Out_CV_3_Min"<< // default 0
               "CV_Out_CV_3_Offset"<< // default 0
               "CV_Out_CV_3_MIDI_Input_Device"<< //default All
               "CV_Out_CV_3_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_Gate_MIDI_Input_Device"<< // default All
               "CV_Out_Gate_Channel"<< // default All
               "CV_Pitch_Scaling_Scheme"<<
               "CV_Gate_Articulation"<<
               "CV_Gate_S_Trig"<<

                // 2.0 variables
               "Preset_Struct_is_V2"<<
               "SEQ_clock_source"<<
               "SEQ_clock_dest"<<
               "SEQ_tempo"<<
               "SEQ_swing"<<
               "SEQ_KB_split"<<
               "SEQ_KB_lower"<<
               "SEQ_KB_upper"<<
               "SEQ_track_1_mode"<<
               "SEQ_track_1_poly"<<
               "SEQ_track_1_arp_pattern"<<
               "SEQ_track_1_seq_pattern"<<
               "SEQ_track_1_division"<<
               "SEQ_track_1_dot"<<
               "SEQ_track_1_gate"<<
               "SEQ_track_1_chain_x"<<
               "SEQ_track_1_chain_y"<<
               "SEQ_track_1_chain_z"<<
               "SEQ_track_1_length"<<
               "SEQ_track_1_dest"<<
               "SEQ_track_1_midi_ch"<<
               "SEQ_track_1_transp"<<
               "SEQ_track_2_mode"<<
               "SEQ_track_2_poly"<<
               "SEQ_track_2_arp_pattern"<<
               "SEQ_track_2_seq_pattern"<<
               "SEQ_track_2_division"<<
               "SEQ_track_2_dot"<<
               "SEQ_track_2_gate"<<
               "SEQ_track_2_chain_x"<<
               "SEQ_track_2_chain_y"<<
               "SEQ_track_2_chain_z"<<
               "SEQ_track_2_length"<<
               "SEQ_track_2_dest"<<
               "SEQ_track_2_midi_ch"<<
               "SEQ_track_2_transp"<<
               "SEQ_track_3_mode"<<
               "SEQ_track_3_poly"<<
               "SEQ_track_3_arp_pattern"<<
               "SEQ_track_3_seq_pattern"<<
               "SEQ_track_3_division"<<
               "SEQ_track_3_dot"<<
               "SEQ_track_3_gate"<<
               "SEQ_track_3_chain_x"<<
               "SEQ_track_3_chain_y"<<
               "SEQ_track_3_chain_z"<<
               "SEQ_track_3_length"<<
               "SEQ_track_3_dest"<<
               "SEQ_track_3_midi_ch"<<
               "SEQ_track_3_transp";

    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//

    // lookup tables to convert qn enums to qt comboboxes
    Curves
            <<
               "Linear"<<
               "Logarithmic"<<
               "Sine"<<
               "Cosine"<<
               "Exponential"<<
               "Invert"<<
               "Custom 1"<<
               "Custom 2";
    /*
    CV_Out_Source
            <<
               "Off"<<
               "Track 1"<<
               "Track 2"<<
               "Track 3"<<
               "All Tracks"<<
               "Expander"<<
               "USB Port 3"<<
               "USB 3 + Expander"<<
               "Track 1 + USB 3 + Exp."<<
               "Track 2 + USB 3 + Exp."<<
               "Track 3 + USB 3 + Exp."<<
               "All Tracks + USB 3 + Exp."<<
               "Clock";
               */
    CV_Out_Type
            <<
               "CC#000"<<
               "CC#001"<<
               "CC#002"<<
               "CC#003"<<
               "CC#004"<<
               "CC#005"<<
               "CC#006"<<
               "CC#007"<<
               "CC#008"<<
               "CC#009"<<
               "CC#010"<<
               "CC#011"<<
               "CC#012"<<
               "CC#013"<<
               "CC#014"<<
               "CC#015"<<
               "CC#016"<<
               "CC#017"<<
               "CC#018"<<
               "CC#019"<<
               "CC#020"<<
               "CC#021"<<
               "CC#022"<<
               "CC#023"<<
               "CC#024"<<
               "CC#025"<<
               "CC#026"<<
               "CC#027"<<
               "CC#028"<<
               "CC#029"<<
               "CC#030"<<
               "CC#031"<<
               "CC#032"<<
               "CC#033"<<
               "CC#034"<<
               "CC#035"<<
               "CC#036"<<
               "CC#037"<<
               "CC#038"<<
               "CC#039"<<
               "CC#040"<<
               "CC#041"<<
               "CC#042"<<
               "CC#043"<<
               "CC#044"<<
               "CC#045"<<
               "CC#046"<<
               "CC#047"<<
               "CC#048"<<
               "CC#049"<<
               "CC#050"<<
               "CC#051"<<
               "CC#052"<<
               "CC#053"<<
               "CC#054"<<
               "CC#055"<<
               "CC#056"<<
               "CC#057"<<
               "CC#058"<<
               "CC#059"<<
               "CC#060"<<
               "CC#061"<<
               "CC#062"<<
               "CC#063"<<
               "CC#064"<<
               "CC#065"<<
               "CC#066"<<
               "CC#067"<<
               "CC#068"<<
               "CC#069"<<
               "CC#070"<<
               "CC#071"<<
               "CC#072"<<
               "CC#073"<<
               "CC#074"<<
               "CC#075"<<
               "CC#076"<<
               "CC#077"<<
               "CC#078"<<
               "CC#079"<<
               "CC#080"<<
               "CC#081"<<
               "CC#082"<<
               "CC#083"<<
               "CC#084"<<
               "CC#085"<<
               "CC#086"<<
               "CC#087"<<
               "CC#088"<<
               "CC#089"<<
               "CC#090"<<
               "CC#091"<<
               "CC#092"<<
               "CC#093"<<
               "CC#094"<<
               "CC#095"<<
               "CC#096"<<
               "CC#097"<<
               "CC#098"<<
               "CC#099"<<
               "CC#100"<<
               "CC#101"<<
               "CC#102"<<
               "CC#103"<<
               "CC#104"<<
               "CC#105"<<
               "CC#106"<<
               "CC#107"<<
               "CC#108"<<
               "CC#109"<<
               "CC#110"<<
               "CC#111"<<
               "CC#112"<<
               "CC#113"<<
               "CC#114"<<
               "CC#115"<<
               "CC#116"<<
               "CC#117"<<
               "CC#118"<<
               "CC#119"<<
               "CC#120"<<
               "CC#121"<<
               "CC#122"<<
               "CC#123"<<
               "CC#124"<<
               "CC#125"<<
               "CC#126"<<
               "CC#127"<<
               "Pitch CV"<<
               "Velocity CV"<<
               "Pressure CV"<<
               "Pitch Bend CV"<<
               "Tilt CV"<<
               "Gate CV"<<
               "Off";
    CV_Scaling
            <<
               "1 Volt/Octave"<<
               "1.2 Volts/Octave"<<
               "Hz/Volt";
    KB_Source
            <<
               "None"<<
               "Key Velocity"<<
               "Key Pitch"<<
               "Key Number"<<
               "Note On"<<
               "Pressure"<<
               "Tilt"<<
               "Bend Pad"<<
               "Expression Pedal";
    LED_Refresh
            <<
               "Normal"<<
               "Control Only"<<
               "All Off";
    Key_Priority
            <<
               "Latest"<<
               "Earliest"<<
               "Highest"<<
               "Lowest";
    MIDI_Out
            <<
               "USB 1"<<
               "Expander"<<
               "USB 1 + Expander"<<
               "Disabled";
    CV_MIDI_Out
            <<
               "None"<<
               "Expander Port"<<
               "USB 3"<<
               "None"<<
               "None"<<
               "USB 3 + Expander";
    MIDI_CH
            <<
               "ch. 1"<<
               "ch. 2"<<
               "ch. 3"<<
               "ch. 4"<<
               "ch. 5"<<
               "ch. 6"<<
               "ch. 7"<<
               "ch. 8"<<
               "ch. 9"<<
               "ch. 10"<<
               "ch. 11"<<
               "ch. 12"<<
               "ch. 13"<<
               "ch. 14"<<
               "ch. 15"<<
               "ch. 16"<<
               "MPE";
    Toggle_Modes
            <<
               "Off"<<
               "On"<<
               "Legato";
    SEQ_clock_source
            <<
               "Auto"<<
               "Internal"<<
               "USB Port 1"<<
               "Expander In"<<
               "CV In 1"<<
               "CV In 2";
    SEQ_track_mode
            <<
               "Off"<<
               "Arpeggiate"<<
               "Step Sequencer"<<
               "Step Sequencer"<<
               "Step Sequencer"<<
               "Step Sequencer";
    SEQ_track_polyphony
            <<
               "Disabled"<<
               "Mono"<<
               "2 Voices"<<
               "3 Voices"<<
               "4 Voices";
    SEQ_track_arp_pattern
            <<
               "None"<< // skip 0 for logic purposes
               "Up"<<
               "Down"<<
               "Up/Down Exc"<<
               "Up/Down Inc"<<
               "Order"<<
               "Reverse"<<
               "Pend Exc"<<
               "Pend Inc"<<
               "Random"<<
               "Drunk";

    SEQ_track_seq_division
            <<
               "Empty"<<
               "Pat. #1"<<
               "Pat. #2"<<
               "Pat. #3"<<
               "Pat. #4"<<
               "Pat. #5"<<
               "Pat. #6"<<
               "Pat. #7"<<
               "Pat. #8";

    SEQ_track_division
            <<
               "Whole"<<
               "Half"<<
               "Quarter"<<
               "8th"<<
               "16th"<<
               "32nd";
    SEQ_track_dot
            <<
               "No Dot"<<
               "Dot"<<
               "Triplet"<<
               "No Dot"<<
               "No Dot"<<
               "No Dot";


    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//


    //----- Create Dictionary for Translating Strings
    //*** key modes
    dictionary.insert("Off" , 0);
    dictionary.insert("On" , 1);
    dictionary.insert("Legato" , 2);

    //*** channel rotation
    //"CHANNEL_ROTATION_DISABLED" , 0);
    //"CHANNEL_ROTATION_ACTIVE" , 1);

    //*** key selection criteria
    dictionary.insert("Latest" , 0);
    dictionary.insert("Earliest" , 1);
    dictionary.insert("Highest" , 2);
    dictionary.insert("Lowest" , 3);

    //*** led modes
    //"REMOTE_LED_MODE" , 0);
    //"LOCAL_LED_MODE": 1);

    //*** curves
    dictionary.insert("Linear" , 0);
    dictionary.insert("Logarithmic" , 1);
    dictionary.insert("Sine" , 2);
    dictionary.insert("Cosine" , 3);
    dictionary.insert("Exponential" , 4);
    dictionary.insert("Invert" , 5);
    dictionary.insert("Custom 1" , 6);
    dictionary.insert("Custom 2" , 7);
    /*dictionary.insert("Linear" , 0);
    dictionary.insert("Exponential" , 1);
    dictionary.insert("Sin" , 2);
    dictionary.insert("Cos" , 3);
    dictionary.insert("Log" , 4);
    dictionary.insert("Compress" , 5);
    dictionary.insert("Mid" , 6);
    dictionary.insert("Expand" , 7);
    dictionary.insert("Expand 2" , 8);
    dictionary.insert("Shift Up 1" , 9);
    dictionary.insert("Shift Up 2" , 10);
    dictionary.insert("Shift Up 3" , 11);
    dictionary.insert("Shift Up 4" , 12);
    dictionary.insert("Xtreme Comp 1" , 13);
    dictionary.insert("Xtreme Comp 2" , 14);
    dictionary.insert("Xtreme Comp 3" , 15);
    dictionary.insert("Hard Comp" , 16);
    dictionary.insert("Xtreme Expand" , 17);
    dictionary.insert("Invert" , 18); // added 4/3/2013); nyw*/

    //*** output devices
    dictionary.insert("USB 1" , 0);
    dictionary.insert("Expander" , 1);
    dictionary.insert("USB 1 + Expander" , 2);
    dictionary.insert("Disabled", 3);

    //*** clock inputs
    dictionary.insert("Auto" , 0);
    dictionary.insert("Internal" , 1);
    dictionary.insert("USB Port 1" , 2);
    dictionary.insert("Expander In" , 3);
    dictionary.insert("CV In 1" , 4);
    dictionary.insert("CV In 2" , 5);

    //*** sources
    dictionary.insert("None" , 0);
    dictionary.insert("Velocity" , 1);
    dictionary.insert("Key Pitch" , 2);
    dictionary.insert("Key Number" , 3);
    dictionary.insert("Note On" , 4);
    dictionary.insert("Pressure" , 5);
    dictionary.insert("Tilt" , 6);
    dictionary.insert("Bend Pad" , 7);
    dictionary.insert("Expression Pedal" , 8);

    //*** output device sources for the CV
    //dictionary.insert("Off" , 0); // already inserted above
    dictionary.insert("Track 1" , 1);
    dictionary.insert("Track 2" , 2);
    dictionary.insert("Track 3" , 4);
    dictionary.insert("All Tracks" , 7);
    dictionary.insert("Expander Out" , 8);
    dictionary.insert("USB Port 3" , 16);
    dictionary.insert("USB 3 + Exp.", 24);
    dictionary.insert("Track 1 + USB 3 + Exp." , 25);
    dictionary.insert("Track 2 + USB 3 + Exp." , 26);
    dictionary.insert("Track 3 + USB 3 + Exp." , 28);
    dictionary.insert("All Tracks + USB 3 + Exp." , 31);
    dictionary.insert("Clock" , 128);

    //*** CV Input MIDI destinations
    dictionary.insert("Expander Port", 1);
    dictionary.insert("USB 3", 2);
    dictionary.insert("USB 3 + Expander", 5);

    //*** CV midi types
    dictionary.insert("CC#000" , 0);
    dictionary.insert("CC#001" , 1);
    dictionary.insert("CC#002" , 2);
    dictionary.insert("CC#003" , 3);
    dictionary.insert("CC#004" , 4);
    dictionary.insert("CC#005" , 5);
    dictionary.insert("CC#006" , 6);
    dictionary.insert("CC#007" , 7);
    dictionary.insert("CC#008" , 8);
    dictionary.insert("CC#009" , 9);
    dictionary.insert("CC#010" , 10);
    dictionary.insert("CC#011" , 11);
    dictionary.insert("CC#012" , 12);
    dictionary.insert("CC#013" , 13);
    dictionary.insert("CC#014" , 14);
    dictionary.insert("CC#015" , 15);
    dictionary.insert("CC#016" , 16);
    dictionary.insert("CC#017" , 17);
    dictionary.insert("CC#018" , 18);
    dictionary.insert("CC#019" , 19);
    dictionary.insert("CC#020" , 20);
    dictionary.insert("CC#021" , 21);
    dictionary.insert("CC#022" , 22);
    dictionary.insert("CC#023" , 23);
    dictionary.insert("CC#024" , 24);
    dictionary.insert("CC#025" , 25);
    dictionary.insert("CC#026" , 26);
    dictionary.insert("CC#027" , 27);
    dictionary.insert("CC#028" , 28);
    dictionary.insert("CC#029" , 29);
    dictionary.insert("CC#030" , 30);
    dictionary.insert("CC#031" , 31);
    dictionary.insert("CC#032" , 32);
    dictionary.insert("CC#033" , 33);
    dictionary.insert("CC#034" , 34);
    dictionary.insert("CC#035" , 35);
    dictionary.insert("CC#036" , 36);
    dictionary.insert("CC#037" , 37);
    dictionary.insert("CC#038" , 38);
    dictionary.insert("CC#039" , 39);
    dictionary.insert("CC#040" , 40);
    dictionary.insert("CC#041" , 41);
    dictionary.insert("CC#042" , 42);
    dictionary.insert("CC#043" , 43);
    dictionary.insert("CC#044" , 44);
    dictionary.insert("CC#045" , 45);
    dictionary.insert("CC#046" , 46);
    dictionary.insert("CC#047" , 47);
    dictionary.insert("CC#048" , 48);
    dictionary.insert("CC#049" , 49);
    dictionary.insert("CC#050" , 50);
    dictionary.insert("CC#051" , 51);
    dictionary.insert("CC#052" , 52);
    dictionary.insert("CC#053" , 53);
    dictionary.insert("CC#054" , 54);
    dictionary.insert("CC#055" , 55);
    dictionary.insert("CC#056" , 56);
    dictionary.insert("CC#057" , 57);
    dictionary.insert("CC#058" , 58);
    dictionary.insert("CC#059" , 59);
    dictionary.insert("CC#060" , 60);
    dictionary.insert("CC#061" , 61);
    dictionary.insert("CC#062" , 62);
    dictionary.insert("CC#063" , 63);
    dictionary.insert("CC#064" , 64);
    dictionary.insert("CC#065" , 65);
    dictionary.insert("CC#066" , 66);
    dictionary.insert("CC#067" , 67);
    dictionary.insert("CC#068" , 68);
    dictionary.insert("CC#069" , 69);
    dictionary.insert("CC#070" , 70);
    dictionary.insert("CC#071" , 71);
    dictionary.insert("CC#072" , 72);
    dictionary.insert("CC#073" , 73);
    dictionary.insert("CC#074" , 74);
    dictionary.insert("CC#075" , 75);
    dictionary.insert("CC#076" , 76);
    dictionary.insert("CC#077" , 77);
    dictionary.insert("CC#078" , 78);
    dictionary.insert("CC#079" , 79);
    dictionary.insert("CC#080" , 80);
    dictionary.insert("CC#081" , 81);
    dictionary.insert("CC#082" , 82);
    dictionary.insert("CC#083" , 83);
    dictionary.insert("CC#084" , 84);
    dictionary.insert("CC#085" , 85);
    dictionary.insert("CC#086" , 86);
    dictionary.insert("CC#087" , 87);
    dictionary.insert("CC#088" , 88);
    dictionary.insert("CC#089" , 89);
    dictionary.insert("CC#090" , 90);
    dictionary.insert("CC#091" , 91);
    dictionary.insert("CC#092" , 92);
    dictionary.insert("CC#093" , 93);
    dictionary.insert("CC#094" , 94);
    dictionary.insert("CC#095" , 95);
    dictionary.insert("CC#096" , 96);
    dictionary.insert("CC#097" , 97);
    dictionary.insert("CC#098" , 98);
    dictionary.insert("CC#099" , 99);
    dictionary.insert("CC#100" , 100);
    dictionary.insert("CC#101" , 101);
    dictionary.insert("CC#102" , 102);
    dictionary.insert("CC#103" , 103);
    dictionary.insert("CC#104" , 104);
    dictionary.insert("CC#105" , 105);
    dictionary.insert("CC#106" , 106);
    dictionary.insert("CC#107" , 107);
    dictionary.insert("CC#108" , 108);
    dictionary.insert("CC#109" , 109);
    dictionary.insert("CC#110" , 110);
    dictionary.insert("CC#111" , 111);
    dictionary.insert("CC#112" , 112);
    dictionary.insert("CC#113" , 113);
    dictionary.insert("CC#114" , 114);
    dictionary.insert("CC#115" , 115);
    dictionary.insert("CC#116" , 116);
    dictionary.insert("CC#117" , 117);
    dictionary.insert("CC#118" , 118);
    dictionary.insert("CC#119" , 119);
    dictionary.insert("CC#120" , 120);
    dictionary.insert("CC#121" , 121);
    dictionary.insert("CC#122" , 122);
    dictionary.insert("CC#123" , 123);
    dictionary.insert("CC#124" , 124);
    dictionary.insert("CC#125" , 125);
    dictionary.insert("CC#126" , 126);
    dictionary.insert("CC#127" , 127);
    dictionary.insert("Pitch CV" , 128);
    dictionary.insert("Velocity CV" , 129);
    dictionary.insert("Pressure CV" , 130);
    dictionary.insert("Pitch Bend CV" , 131);
    dictionary.insert("Tilt CV", 132);
    dictionary.insert("Gate CV", 133);
    dictionary.insert("OFF", 134);

    dictionary.insert("1 Volt/Octave" , 0);
    dictionary.insert("1.2 Volts/Octave" , 1);
    dictionary.insert("Hz/Volt" , 2);

    //Led Refresh Style
    dictionary.insert("Normal", 0); //Handled in Key modes sections
    dictionary.insert("Control Only", 2);
    dictionary.insert("All Off", 1);

    dictionary.insert("true" , 1);
    dictionary.insert("false" , 0);


    //** Sequencer Track Values
    dictionary.insert("Arpeggiator", 1);
    dictionary.insert("Step Sequencer", 3);

    dictionary.insert("Whole", 0);
    dictionary.insert("Half", 1);
    dictionary.insert("Quarter", 2);
    dictionary.insert("8th", 3);
    dictionary.insert("16th", 4);
    dictionary.insert("32nd", 5);

    dictionary.insert("No Dot", 0);
    dictionary.insert("Dot", 1);
    dictionary.insert("Triplet", 2);

    dictionary.insert("Up", 1);
    dictionary.insert("Down", 2);
    dictionary.insert("Up/Down Exc", 3);
    dictionary.insert("Up/Down Inc", 4);
    dictionary.insert("Order", 5);
    dictionary.insert("Reverse", 6);
    dictionary.insert("Pend Exc", 7);
    dictionary.insert("Pend Inc", 8);
    dictionary.insert("Random", 9);
    dictionary.insert("Drunk", 10);
    dictionary.insert("Strum", 11);

    dictionary.insert("Mono", 1);
    dictionary.insert("4 voices", 4);


    //** Sequencer MIDI ch.
    dictionary.insert("ch. 1" , 0);
    dictionary.insert("ch. 2" , 1);
    dictionary.insert("ch. 3" , 2);
    dictionary.insert("ch. 4" , 3);
    dictionary.insert("ch. 5" , 4);
    dictionary.insert("ch. 6" , 5);
    dictionary.insert("ch. 7" , 6);
    dictionary.insert("ch. 8" , 7);
    dictionary.insert("ch. 9" , 8);
    dictionary.insert("ch. 10" , 9);
    dictionary.insert("ch. 11" , 10);
    dictionary.insert("ch. 12" , 11);
    dictionary.insert("ch. 13" , 12);
    dictionary.insert("ch. 14" , 13);
    dictionary.insert("ch. 15" , 14);
    dictionary.insert("ch. 16" , 15);
    dictionary.insert("MPE" , 16);

    //** Patterns
    dictionary.insert("Empty", 0);
    dictionary.insert("Pat. #1", 1);
    dictionary.insert("Pat. #2", 2);
    dictionary.insert("Pat. #3", 3);
    dictionary.insert("Pat. #4", 4);
    dictionary.insert("Pat. #5", 5);
    dictionary.insert("Pat. #6", 6);
    dictionary.insert("Pat. #7", 7);
    dictionary.insert("Pat. #8", 8);


    ///////////////////////////////////////
    /////////////QuNexusSysex//////////////
    ///////////////////////////////////////
    //----- QuNexusSysEx Vars
    PRESET_LENGTH = 507;  //<<------------- was 470 for 1.1.9 - DON'T FORGET TO CHANGE THIS WHEN PRESETS GET CHANGED
    PRESET_START = 0xA1;
    PRESET_LENGTH_MSB = (PRESET_LENGTH >> 8);
    PRESET_LENGTH_LSB = (PRESET_LENGTH & 0xFF);

    ////////////////////////////////////////////
    ///////////// parameter types //////////////
    ////////////////////////////////////////////

    //paramTypeMap.insert()

    paramTypeMap.insert("Keyboard_Global_Program_Change_A", "char");
    paramTypeMap.insert("Keyboard_Global_Program_Change_B", "char");
    paramTypeMap.insert("Keyboard_Global_Program_Change_C", "char");
    paramTypeMap.insert("Keyboard_Global_Program_Change_D", "char");
    paramTypeMap.insert("Globals_Gain", "char");
    paramTypeMap.insert("Globals_Off_Thresh", "char");
    paramTypeMap.insert("Globals_On_Thresh", "char");
    paramTypeMap.insert("Globals_Tilt_Sensitivity", "char");
    paramTypeMap.insert("Keyboard_Global_Mode", "Toggle_Modes");
    paramTypeMap.insert("Keyboard_Global_Channel_Rotation_Active", "char");
    paramTypeMap.insert("Keyboard_Global_USB_1_Channel", "char");
    paramTypeMap.insert("Keyboard_Global_USB_2_Channel", "char");
    paramTypeMap.insert("Keyboard_Global_Key_Selection_Criteria", "Key_Priority");
    paramTypeMap.insert("Keyboard_Global_Velocity_Active", "char");
    paramTypeMap.insert("Keyboard_Global_Pressure_Active", "char");
    paramTypeMap.insert("Keyboard_Global_Tilt_Active", "char");
    paramTypeMap.insert("Keyboard_Global_Channel_Pressure_Active", "char");
    paramTypeMap.insert("Keyboard_Global_Bend_Pad_Sensitivity", "char");
    paramTypeMap.insert("Keyboard_Global_Poly_Aftertouch_Active", "char");
    paramTypeMap.insert("Keyboard_Global_CC_Active", "char");
    paramTypeMap.insert("Keyboard_Global_Channel_Rotation_Offset", "char");
    paramTypeMap.insert("Keyboard_Global_LED_Mode", "Checkbox");
    paramTypeMap.insert("Keyboard_Global_LED_Remote_Channel", "char");
    paramTypeMap.insert("Keyboard_Global_Polyphony_Number", "char");
    paramTypeMap.insert("Globals_LED_Refresh_Style", "LED_Refresh");
    paramTypeMap.insert("Keyboard_Global_Transpose", "signed char");
    paramTypeMap.insert("Keyboard_CC_00_Control_Number", "char");
    paramTypeMap.insert("Keyboard_CC_00_Curve", "Curve");
    paramTypeMap.insert("Keyboard_CC_00_Gain", "signed int");
    paramTypeMap.insert("Keyboard_CC_00_Max", "char");
    paramTypeMap.insert("Keyboard_CC_00_Min", "char");
    paramTypeMap.insert("Keyboard_CC_00_Offset", "signed char");
    paramTypeMap.insert("Keyboard_CC_00_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_CC_00_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_CC_01_Control_Number", "char");
    paramTypeMap.insert("Keyboard_CC_01_Curve", "Curve");
    paramTypeMap.insert("Keyboard_CC_01_Gain", "signed int");
    paramTypeMap.insert("Keyboard_CC_01_Max", "char");
    paramTypeMap.insert("Keyboard_CC_01_Min", "char");
    paramTypeMap.insert("Keyboard_CC_01_Offset", "signed char");
    paramTypeMap.insert("Keyboard_CC_01_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_CC_01_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_CC_02_Control_Number", "char");
    paramTypeMap.insert("Keyboard_CC_02_Curve", "Curve");
    paramTypeMap.insert("Keyboard_CC_02_Gain", "signed int");
    paramTypeMap.insert("Keyboard_CC_02_Max", "char");
    paramTypeMap.insert("Keyboard_CC_02_Min", "char");
    paramTypeMap.insert("Keyboard_CC_02_Offset", "signed char");
    paramTypeMap.insert("Keyboard_CC_02_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_CC_02_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Curve", "Curve");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Gain", "signed int");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Max", "char");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Min", "char");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Offset", "signed char");
    paramTypeMap.insert("Keyboard_Channel_Pressure_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_Channel_Pressure_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Curve", "Curve");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Gain", "signed int");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Max", "char");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Min", "char");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Offset", "signed char");
    paramTypeMap.insert("Keyboard_Pitch_Bend_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_Pitch_Bend_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Curve", "Curve");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Gain", "signed int");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Max", "char");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Min", "char");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Offset", "signed char");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Keyboard_Poly_Aftertouch_Source", "KB_Source");
    paramTypeMap.insert("Keyboard_Velocity_Curve", "Curve");
    paramTypeMap.insert("Keyboard_Velocity_Gain", "signed int");
    paramTypeMap.insert("Keyboard_Velocity_Max", "char");
    paramTypeMap.insert("Keyboard_Velocity_Min", "char");
    paramTypeMap.insert("Keyboard_Velocity_Offset", "signed char");
    paramTypeMap.insert("Keyboard_Velocity_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Controller_Global_MIDI_Output_Device", "MIDI_Out");
    paramTypeMap.insert("Controller_Key_0_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_0_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_0_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_0_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_0_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_0_Channel", "char");
    paramTypeMap.insert("Controller_Key_0_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_0_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_0_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_0_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_0_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_0_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_0_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_1_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_1_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_1_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_1_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_1_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_1_Channel", "char");
    paramTypeMap.insert("Controller_Key_1_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_1_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_1_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_1_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_1_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_1_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_1_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_2_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_2_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_2_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_2_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_2_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_2_Channel", "char");
    paramTypeMap.insert("Controller_Key_2_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_2_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_2_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_2_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_2_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_2_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_2_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_3_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_3_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_3_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_3_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_3_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_3_Channel", "char");
    paramTypeMap.insert("Controller_Key_3_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_3_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_3_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_3_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_3_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_3_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_3_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_4_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_4_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_4_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_4_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_4_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_4_Channel", "char");
    paramTypeMap.insert("Controller_Key_4_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_4_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_4_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_4_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_4_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_4_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_4_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_5_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_5_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_5_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_5_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_5_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_5_Channel", "char");
    paramTypeMap.insert("Controller_Key_5_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_5_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_5_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_5_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_5_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_5_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_5_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_6_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_6_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_6_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_6_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_6_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_6_Channel", "char");
    paramTypeMap.insert("Controller_Key_6_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_6_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_6_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_6_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_6_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_6_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_6_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_7_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_7_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_7_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_7_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_7_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_7_Channel", "char");
    paramTypeMap.insert("Controller_Key_7_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_7_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_7_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_7_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_7_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_7_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_7_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_8_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_8_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_8_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_8_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_8_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_8_Channel", "char");
    paramTypeMap.insert("Controller_Key_8_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_8_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_8_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_8_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_8_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_8_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_8_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_9_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_9_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_9_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_9_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_9_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_9_Channel", "char");
    paramTypeMap.insert("Controller_Key_9_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_9_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_9_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_9_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_9_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_9_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_9_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_10_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_10_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_10_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_10_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_10_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_10_Channel", "char");
    paramTypeMap.insert("Controller_Key_10_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_10_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_10_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_10_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_10_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_10_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_10_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_11_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_11_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_11_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_11_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_11_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_11_Channel", "char");
    paramTypeMap.insert("Controller_Key_11_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_11_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_11_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_11_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_11_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_11_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_11_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_12_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_12_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_12_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_12_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_12_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_12_Channel", "char");
    paramTypeMap.insert("Controller_Key_12_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_12_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_12_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_12_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_12_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_12_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_12_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_13_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_13_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_13_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_13_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_13_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_13_Channel", "char");
    paramTypeMap.insert("Controller_Key_13_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_13_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_13_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_13_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_13_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_13_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_13_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_14_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_14_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_14_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_14_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_14_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_14_Channel", "char");
    paramTypeMap.insert("Controller_Key_14_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_14_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_14_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_14_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_14_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_14_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_14_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_15_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_15_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_15_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_15_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_15_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_15_Channel", "char");
    paramTypeMap.insert("Controller_Key_15_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_15_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_15_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_15_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_15_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_15_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_15_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_16_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_16_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_16_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_16_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_16_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_16_Channel", "char");
    paramTypeMap.insert("Controller_Key_16_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_16_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_16_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_16_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_16_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_16_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_16_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_17_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_17_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_17_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_17_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_17_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_17_Channel", "char");
    paramTypeMap.insert("Controller_Key_17_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_17_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_17_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_17_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_17_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_17_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_17_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_18_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_18_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_18_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_18_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_18_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_18_Channel", "char");
    paramTypeMap.insert("Controller_Key_18_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_18_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_18_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_18_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_18_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_18_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_18_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_19_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_19_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_19_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_19_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_19_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_19_Channel", "char");
    paramTypeMap.insert("Controller_Key_19_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_19_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_19_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_19_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_19_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_19_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_19_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_20_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_20_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_20_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_20_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_20_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_20_Channel", "char");
    paramTypeMap.insert("Controller_Key_20_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_20_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_20_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_20_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_20_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_20_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_20_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_21_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_21_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_21_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_21_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_21_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_21_Channel", "char");
    paramTypeMap.insert("Controller_Key_21_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_21_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_21_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_21_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_21_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_21_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_21_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_22_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_22_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_22_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_22_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_22_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_22_Channel", "char");
    paramTypeMap.insert("Controller_Key_22_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_22_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_22_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_22_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_22_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_22_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_22_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_23_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_23_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_23_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_23_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_23_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_23_Channel", "char");
    paramTypeMap.insert("Controller_Key_23_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_23_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_23_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_23_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_23_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_23_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_23_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Key_24_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_24_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Key_24_Note_Number", "char");
    paramTypeMap.insert("Controller_Key_24_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_24_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_24_Channel", "char");
    paramTypeMap.insert("Controller_Key_24_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Key_24_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_24_Tilt_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_24_Toggle_Control_Number", "char");
    paramTypeMap.insert("Controller_Key_24_Toggle_Value", "char");
    paramTypeMap.insert("Controller_Key_24_Toggle_Return_Value", "char");
    paramTypeMap.insert("Controller_Key_24_Toggle_On_Off", "Checkbox");
    paramTypeMap.insert("Controller_Bend_Pad_00_Controller_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable", "Checkbox");
    paramTypeMap.insert("Controller_Bend_Pad_00_Note_Number", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Pressure_Control_Number", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Tilt_Control_Number", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Channel", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Note_Velocity", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Pressure_Return_Value", "char");
    paramTypeMap.insert("Controller_Bend_Pad_00_Tilt_Return_Value", "char");
    paramTypeMap.insert("CV_In_CV_1_Channel", "char");
    paramTypeMap.insert("CV_In_CV_1_Curve", "Curve");
    paramTypeMap.insert("CV_In_CV_1_Gain", "signed int");
    paramTypeMap.insert("CV_In_CV_1_Max", "char");
    paramTypeMap.insert("CV_In_CV_1_Min", "char");
    paramTypeMap.insert("CV_In_CV_1_Offset", "signed char");
    paramTypeMap.insert("CV_In_CV_1_MIDI_Output_Device", "CV_MIDI_Out");
    paramTypeMap.insert("CV_In_CV_1_CC_Number", "char");
    paramTypeMap.insert("CV_In_CV_2_Channel", "char");
    paramTypeMap.insert("CV_In_CV_2_Curve", "Curve");
    paramTypeMap.insert("CV_In_CV_2_Gain", "signed int");
    paramTypeMap.insert("CV_In_CV_2_Max", "char");
    paramTypeMap.insert("CV_In_CV_2_Min", "char");
    paramTypeMap.insert("CV_In_CV_2_Offset", "signed char");
    paramTypeMap.insert("CV_In_CV_2_MIDI_Output_Device", "CV_MIDI_Out");
    paramTypeMap.insert("CV_In_CV_2_CC_Number", "char");
    paramTypeMap.insert("CV_Out_CV_1_Channel", "char");
    paramTypeMap.insert("CV_Out_CV_1_Curve", "Curve");
    paramTypeMap.insert("CV_Out_CV_1_Gain", "signed int");
    paramTypeMap.insert("CV_Out_CV_1_Max", "char");
    paramTypeMap.insert("CV_Out_CV_1_Min", "char");
    paramTypeMap.insert("CV_Out_CV_1_Offset", "signed char");
    paramTypeMap.insert("CV_Out_CV_1_MIDI_Input_Device", "CV_Out_Source");
    paramTypeMap.insert("CV_Out_CV_1_MIDI_Input_Type", "CV_Out_Type");
    paramTypeMap.insert("CV_Out_CV_2_Channel", "char");
    paramTypeMap.insert("CV_Out_CV_2_Curve", "Curve");
    paramTypeMap.insert("CV_Out_CV_2_Gain", "signed int");
    paramTypeMap.insert("CV_Out_CV_2_Max", "char");
    paramTypeMap.insert("CV_Out_CV_2_Min", "char");
    paramTypeMap.insert("CV_Out_CV_2_Offset", "signed char");
    paramTypeMap.insert("CV_Out_CV_2_MIDI_Input_Device", "CV_Out_Source");
    paramTypeMap.insert("CV_Out_CV_2_MIDI_Input_Type", "CV_Out_Type");
    paramTypeMap.insert("CV_Out_CV_3_Channel", "char");
    paramTypeMap.insert("CV_Out_CV_3_Curve", "Curve");
    paramTypeMap.insert("CV_Out_CV_3_Gain", "signed int");
    paramTypeMap.insert("CV_Out_CV_3_Max", "char");
    paramTypeMap.insert("CV_Out_CV_3_Min", "char");
    paramTypeMap.insert("CV_Out_CV_3_Offset", "signed char");
    paramTypeMap.insert("CV_Out_CV_3_MIDI_Input_Device", "CV_Out_Source");
    paramTypeMap.insert("CV_Out_CV_3_MIDI_Input_Type", "CV_Out_Type");
    paramTypeMap.insert("CV_Out_Gate_MIDI_Input_Device", "CV_Out_Source");
    paramTypeMap.insert("CV_Out_Gate_Channel", "char");
    paramTypeMap.insert("CV_Pitch_Scaling_Scheme", "CV_Scaling");
    paramTypeMap.insert("CV_Gate_Articulation", "Checkbox");
    paramTypeMap.insert("CV_Gate_S_Trig", "Checkbox");
    paramTypeMap.insert("Preset_Struct_is_V2", "char");
    paramTypeMap.insert("SEQ_clock_source", "bs44right");
    paramTypeMap.insert("SEQ_clock_dest", "bs44left");
    paramTypeMap.insert("SEQ_tempo", "16b_int");
    paramTypeMap.insert("SEQ_swing", "char");
    paramTypeMap.insert("SEQ_KB_split", "char");
    paramTypeMap.insert("SEQ_KB_lower", "bs44right");
    paramTypeMap.insert("SEQ_KB_upper", "bs44left");
    paramTypeMap.insert("SEQ_track_1_mode", "bs44right");
    paramTypeMap.insert("SEQ_track_1_poly", "bs44left");
    paramTypeMap.insert("SEQ_track_1_arp_pattern", "bs44right");
    paramTypeMap.insert("SEQ_track_1_seq_pattern", "bs44left");
    paramTypeMap.insert("SEQ_track_1_division", "bs44right");
    paramTypeMap.insert("SEQ_track_1_dot", "bs44left");
    paramTypeMap.insert("SEQ_track_1_gate", "char");
    paramTypeMap.insert("SEQ_track_1_chain_x", "char");
    paramTypeMap.insert("SEQ_track_1_chain_y", "char");
    paramTypeMap.insert("SEQ_track_1_chain_z", "char");
    paramTypeMap.insert("SEQ_track_1_length", "char");
    paramTypeMap.insert("SEQ_track_1_dest", "bs26right");
    paramTypeMap.insert("SEQ_track_1_midi_ch", "bs26left");
    paramTypeMap.insert("SEQ_track_1_transp", "char");
    paramTypeMap.insert("SEQ_track_2_mode", "bs44right");
    paramTypeMap.insert("SEQ_track_2_poly", "bs44left");
    paramTypeMap.insert("SEQ_track_2_arp_pattern", "bs44right");
    paramTypeMap.insert("SEQ_track_2_seq_pattern", "bs44left");
    paramTypeMap.insert("SEQ_track_2_division", "bs44right");
    paramTypeMap.insert("SEQ_track_2_dot", "bs44left");
    paramTypeMap.insert("SEQ_track_2_gate", "char");
    paramTypeMap.insert("SEQ_track_2_chain_x", "char");
    paramTypeMap.insert("SEQ_track_2_chain_y", "char");
    paramTypeMap.insert("SEQ_track_2_chain_z", "char");
    paramTypeMap.insert("SEQ_track_2_length", "char");
    paramTypeMap.insert("SEQ_track_2_dest", "bs26right");
    paramTypeMap.insert("SEQ_track_2_midi_ch", "bs26left");
    paramTypeMap.insert("SEQ_track_2_transp", "char");
    paramTypeMap.insert("SEQ_track_3_mode", "bs44right");
    paramTypeMap.insert("SEQ_track_3_poly", "bs44left");
    paramTypeMap.insert("SEQ_track_3_arp_pattern", "bs44right");
    paramTypeMap.insert("SEQ_track_3_seq_pattern", "bs44left");
    paramTypeMap.insert("SEQ_track_3_division", "bs44right");
    paramTypeMap.insert("SEQ_track_3_dot", "bs44left");
    paramTypeMap.insert("SEQ_track_3_gate", "char");
    paramTypeMap.insert("SEQ_track_3_chain_x", "char");
    paramTypeMap.insert("SEQ_track_3_chain_y", "char");
    paramTypeMap.insert("SEQ_track_3_chain_z", "char");
    paramTypeMap.insert("SEQ_track_3_length", "char");
    paramTypeMap.insert("SEQ_track_3_dest", "bs26right");
    paramTypeMap.insert("SEQ_track_3_midi_ch", "bs26left");
    paramTypeMap.insert("SEQ_track_3_transp", "char");

}


///////////////////////////////////////
///////////_midi_sx_encode ////////////
///////////////////////////////////////
void SysExEncDecode::sysex_out(unsigned char ch)
{
    // flag transmit so that we don't interrupt this with activeSense
    if (ch == 240)
    {
        sx_xmit_active = 1;
    }
    else if (ch == 247)
    {
        sx_xmit_active = 0;
    }

    QString chr;
    chr.setNum(ch, 16);
    //qDebug() << encodedSysexByteArray.count() << "-" << chr << "       crc - " << crc;
    encodedSysexByteArray.append(ch);
}

void SysExEncDecode::midi_buffer_put_core(unsigned char ch)
{
    sysex_out(ch);
}

void SysExEncDecode::midi_chunk_init()
{
    midi_hi_bits = midi_hi_count = 0;
}

void SysExEncDecode::midi_sx_encode_char(unsigned char ch)
{
    midi_hi_bits |= (ch & 0x80);
    midi_hi_bits >>=1;
    midi_buffer_put_core(ch & 0x7f);
    if (++midi_hi_count == SX_ENCODE_LEN)
    {
        midi_hi_count = 0;
        //itemName = "High Bit Decode Byte";
        midi_buffer_put_core(midi_hi_bits);
        //itemName = ""
    }
}

void SysExEncDecode::midi_sx_encode_int(int i)
{
    midi_sx_encode_crc_char((unsigned char)((i>>8) & 0xFF)); // encode the MSB
    midi_sx_encode_crc_char((unsigned char)(i & 0xFF)); // encode the LSB
}

void SysExEncDecode::midi_sx_encode_crc_char(unsigned char ch)
{
    crc_byte(ch);
    midi_sx_encode_char(ch);
}

void SysExEncDecode::midi_sx_encode_crc_int(int i)
{
    midi_sx_encode_crc_char((unsigned char)((i>>8) & 0xFF)); // encode the MSB
    //qDebug() << "ENCODE MSB" << QString("0x%1").arg((unsigned char)((i>>8) & 0xFF), 0, 16);

    midi_sx_encode_crc_char((unsigned char)(i & 0xFF)); // encode the LSB
    //qDebug() << "ENCODE LSB" << QString("0x%1").arg((unsigned char)(i & 0xFF), 0, 16);
}

void SysExEncDecode::midi_sx_flush()
{
    while(midi_hi_count)
    {
        midi_sx_encode_crc_char(0);
    }
}

void SysExEncDecode::send_sx_stop()
{
    midi_buffer_put_core(sysex_end);
}

void SysExEncDecode::crc_byte(unsigned char ch)
{
    unsigned int temp = 0;
    unsigned int quick = 0;
    //if we represent crc at start as 0xHHLL
    temp = (crc >> 8) ^ ch;    //xor 8 bit val with upper byte of crc (0x00HH ^ val) = 0x00XX
    crc &= 0xFFFF;
    crc <<= 8;				    // left shift crc now 0xLL00
    crc &= 0xFFFF;
    quick = ((temp ^ (temp >> 4)) & 0xFFFF);	//0x00XX ^ 0x000X = 0x00XY
    crc ^= quick;                               // 0xLL00 ^ 0x00XY = 0xLLXY
    crc &= 0xFFFF;                              //effect of all this is to preserve the information in
    //LSB (LL) intact, while mixing the new data and the old MSB thoroughly

    quick <<= 5;				//hash	(0x00XY << 5) = 0xNNN0	 (quick * 2 to the fifth)
    quick &= 0xFFFF;
    crc ^= quick;				//hash
    crc &= 0xFFFF;
    quick <<= 7;				//hash	(0xNNN0 << 7) = 0xN000
    quick &= 0xFFFF;			//	 (quick * 2 to the seventh)
    crc ^= quick; 				//hash
    crc &= 0xFFFF;
}

void SysExEncDecode::crc_init()
{
    crc = 0xffff;
}

void SysExEncDecode::lineInit()
{
    crc_init();
    midi_chunk_init();
}

///////////////////////////////////////
///////////////// 2.0 /////////////////
///////////////////////////////////////
void SysExEncDecode::encodeSequencerPreset(QVariantMap preset) // encodes a preset into the qnSeq struct
{
    QVariant tempPresetValue;

    // sequencer
    tempPresetValue = dictionary.value( preset.value("SEQ_clock_source").toString() );
    qnSeq.clock_source = tempPresetValue.toUInt();
    qDebug() << "SEQ_clock_source: " << qnSeq.clock_source;

    tempPresetValue = dictionary.value( preset.value("SEQ_clock_dest").toString() );
    qnSeq.clock_dest = tempPresetValue.toUInt();
    qDebug() << "SEQ_clock_dest: " << qnSeq.clock_dest;


    qnSeq.BPM = preset.value("SEQ_tempo").toUInt();
    qnSeq.swingPercent = preset.value("SEQ_swing").toUInt();
    qnSeq.kbSplit = preset.value("SEQ_KB_split").toUInt();
    qnSeq.kbSplitLower = preset.value("SEQ_KB_lower").toUInt();
    qnSeq.kbSplitUpper = preset.value("SEQ_KB_upper").toUInt();

    // track 1
    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_mode").toString() );
    qnSeq.track[0].mode = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_poly").toString() );
    qnSeq.track[0].polyphony = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_arp_pattern").toString() );
    qnSeq.track[0].patternPlay = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_seq_pattern").toString() );
    qnSeq.track[0].patternSeq = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_division").toString() );
    qnSeq.track[0].division = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_dot").toString() );
    qnSeq.track[0].dotTrip = tempPresetValue.toUInt();

    qnSeq.track[0].gatePercent = preset.value("SEQ_track_1_gate").toUInt();
    qnSeq.track[0].chainX = 44; // hard coded at ABCD, this form may change in 2.1
    qnSeq.track[0].chainY = 224;
    qnSeq.track[0].chainZ = 0;
    qnSeq.track[0].length = preset.value("SEQ_track_1_length").toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_dest").toString() );
    qnSeq.track[0].dest = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_1_midi_ch").toString() );
    qnSeq.track[0].channel = tempPresetValue.toUInt();
    //qDebug() << "channel: " << qnSeq.track[0].channel;

    qnSeq.track[0].transpose = preset.value("SEQ_track_1_transp").toUInt();

    // track 2
    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_mode").toString() );
    qnSeq.track[1].mode = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_poly").toString() );
    qnSeq.track[1].polyphony = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_arp_pattern").toString() );
    qnSeq.track[1].patternPlay = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_seq_pattern").toString() );
    qnSeq.track[1].patternSeq = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_division").toString() );
    qnSeq.track[1].division = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_dot").toString() );
    qnSeq.track[1].dotTrip = tempPresetValue.toUInt();

    qnSeq.track[1].gatePercent = preset.value("SEQ_track_2_gate").toUInt();
    qnSeq.track[1].chainX = 44; // hard coded at ABCD, this form may change in 2.1
    qnSeq.track[1].chainY = 224;
    qnSeq.track[1].chainZ = 0;
    qnSeq.track[1].length = preset.value("SEQ_track_2_length").toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_dest").toString() );
    qnSeq.track[1].dest = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_2_midi_ch").toString() );
    qnSeq.track[1].channel = tempPresetValue.toUInt();
    //qDebug() << "channel: " << qnSeq.track[1].channel;

    qnSeq.track[1].transpose = preset.value("SEQ_track_2_transp").toUInt();

    // track 3
    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_mode").toString() );
    qnSeq.track[2].mode = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_poly").toString() );
    qnSeq.track[2].polyphony = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_arp_pattern").toString() );
    qnSeq.track[2].patternPlay = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_seq_pattern").toString() );
    qnSeq.track[2].patternSeq = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_division").toString() );
    qnSeq.track[2].division = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_dot").toString() );
    qnSeq.track[2].dotTrip = tempPresetValue.toUInt();

    qnSeq.track[2].gatePercent = preset.value("SEQ_track_3_gate").toUInt();
    qnSeq.track[2].chainX = 44; // hard coded at ABCD, this form may change in 2.1
    qnSeq.track[2].chainY = 224;
    qnSeq.track[2].chainZ = 0;
    qnSeq.track[2].length = preset.value("SEQ_track_3_length").toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_dest").toString() );
    qnSeq.track[2].dest = tempPresetValue.toUInt();

    tempPresetValue = dictionary.value( preset.value("SEQ_track_3_midi_ch").toString() );
    qnSeq.track[2].channel = tempPresetValue.toUInt();
    //qDebug() << "channel: " << qnSeq.track[2].channel;

    qnSeq.track[2].transpose = preset.value("SEQ_track_3_transp").toUInt();
}

///////////////////////////////////////
/////////////QuNexusSysex//////////////
///////////////////////////////////////

void SysExEncDecode::encode(unsigned char ch)
{
    //qDebug() << "size: " << size << " char:" << ch;
    size--;
    midi_sx_encode_crc_char(ch);

    if (ch < 0)			//write text file for use in explicitly initializing variables
    {
        ch += 256;
    }

    //	print('\t#', count++);
    //count++;
    sumByte += ch;
    sumByte &= 0xFF;
}

void SysExEncDecode::encodeInt(int i)
{
    //qDebug() << "size: " << size << " int:" << i;
    size--;
    midi_sx_encode_crc_int(i);

    if (i < 0)  //write text file for use in explicitly initializing variables
    {
        i += 256;
    }

    //count++;
    sumByte += i;
    sumByte &= 0xFF;
}

///////////////////////////////////////
/////////////  Encoding  //////////////
///////////////////////////////////////

void SysExEncDecode::slotSysExEncodePreset(QVariantMap preset, unsigned char presetNumChar)
{
    qDebug() << "***[ BEGIN slotSysExEncodePreset ] ********************************************************************";
    encodedSysexByteArray.clear();

    //----- SysEx Header
    sysex_out(sysex_start);
    sysex_out(sx_ident.value("manufacturer_id1"));
    sysex_out(sx_ident.value("manufacturer_id2"));
    sysex_out(sx_ident.value("manufacturer_id3"));
    sysex_out(sx_ident.value("manufacturer_id4"));
    sysex_out(sx_ident.value("product"));
    sysex_out(sx_ident.value("format"));
    //itemName = "SX_PACKET_START"
    sysex_out(SX_PACKET_START);
    //itemName = ""

    //----- Packet Preamble
    crc_init(); // sends nothing
    midi_chunk_init(); //sends nothing

    //itemName = "\tstart text"
    midi_sx_encode_crc_char(0x00);
    midi_sx_encode_crc_char(0x02);

    //itemName = "preset start"
    midi_sx_encode_crc_char(0x22);
    midi_sx_encode_crc_char(0x20);

    //itemName = "CRC"
    midi_sx_encode_crc_int(crc);
    //itemName = "flush"
    midi_sx_flush();

    //----- Preset Body
    //print('\nPRESET STARTS');
    midi_sx_encode_char(PRESET_START);          // begin preset_packet_handle() in firmware
    midi_sx_encode_char(PRESET_LENGTH_MSB);
    midi_sx_encode_char(PRESET_LENGTH_LSB);
    crc_init();

    //count = 1 ;//PRESET_LENGTH//1;  //preset size
    //hash = "c"
    sumByte = 0;

    size = PRESET_LENGTH - 13;

    encode(presetNumChar); //0 == Prest Number

    // encode 1.0 parameters
    for (int i=0; i < SIZE_OF_OLD_PRESET_JSON; i++)
    {

        QString tempParamName = keyLayerJSON[i];
        QVariant tempPresetValue = preset.value(tempParamName);

        qDebug() << "JSON Param: " << tempParamName << "Value: " << tempPresetValue;


        if(tempPresetValue.typeName() == QString("QString"))
        {
            tempPresetValue = dictionary.value(preset.value(tempParamName).toString());
            //qDebug() << "String Param:" << tempParamName << "Dictionary Value" << tempPresetValue;
        }
        else if(tempPresetValue.typeName() == QString("int"))
        {
            //tempPresetValue.toInt();
        }
        else if(tempPresetValue.typeName() == QString("double"))
        {
            //tempPresetValue.toDouble();
        }

        //qDebug() << "encode this val:" << tempParamName << tempPresetValue.toInt();

        //If multiple byte param
        if(tempParamName.contains("Gain") && tempParamName != "Globals_Gain")
        {
            encodeInt(tempPresetValue.toInt());
        }
        else if (tempParamName == "Keyboard_Global_Bend_Pad_Sensitivity")
        {
            float thisVal = tempPresetValue.toFloat();
            thisVal = thisVal * 100.0; // (convert 1.0 to 100)
            qDebug() << "Encode pad sensitivity - val: " << thisVal << " int: " << tempPresetValue.toInt() << " float: " << tempPresetValue.toFloat() << " string: " << tempPresetValue.toString();
            encode(tempPresetValue.toInt());
        }
        else
        {
            encode(tempPresetValue.toInt());
        }

        //encode(tempPresetValue.toInt());
    }

    // ------------------- 2.0 parameters -----------------------

    encodeSequencerPreset(preset); // encode this preset into the qnSeq struct

    qDebug() << "------------ begin 2.0 sequencer encoding --------";
    encode(qnSeq.valid); // magic number to confirm that this is the valid 2.0 format

    // qnSeq uses bitfields < 8, so once it's encoded we then combine and encode/transmit the bit fields as 8 bit chars

    // COMBINE macros are (MSb , LSb), LSb comes first in the struct order

    // sequencer data
    qDebug() << "source/dest: " << qnSeq.clock_dest << "/" << qnSeq.clock_source << " combine: " << COMBINE44(qnSeq.clock_dest, qnSeq.clock_source);
    encode( COMBINE44(qnSeq.clock_dest, qnSeq.clock_source) );
    qDebug() << "bpm: " << qnSeq.BPM;
    encodeInt(qnSeq.BPM); // the one lonely integer
    //qDebug() << "Swing: ";
    encode(qnSeq.swingPercent);
    encode(qnSeq.kbSplit);
    //qDebug() << "split lower/upper: ";
    encode( COMBINE44(qnSeq.kbSplitUpper, qnSeq.kbSplitLower) );

    for (int i = 0; i < 3; i++)
    {
        //qDebug() << "-------- Encode Track " << i << " ----------";
        encode( COMBINE44( qnSeq.track[i].polyphony, qnSeq.track[i].mode ) );
        encode( COMBINE44( qnSeq.track[i].patternSeq, qnSeq.track[i].patternPlay ) );
        encode( COMBINE44( qnSeq.track[i].dotTrip, qnSeq.track[i].division ) );
        encode( qnSeq.track[i].gatePercent );
        encode( qnSeq.track[i].chainX) ;
        encode( qnSeq.track[i].chainY );
        encode( qnSeq.track[i].chainZ );
        encode( qnSeq.track[i].length );
        //qDebug() << "Dest/Channel: " << COMBINE62( qnSeq.track[i].channel, qnSeq.track[i].dest );
        encode( COMBINE62( qnSeq.track[i].channel, qnSeq.track[i].dest ) );
        encode( qnSeq.track[i].transpose );
    }
    //qDebug() << "------------ end 2.0 sequencer encoding --------";

    // ------------------- end 2.0 parameters -------------------

    // finish
    encode(sumByte);
    midi_sx_flush();
    sysex_out(sysex_end);

    emit signalEncodedPreset(encodedSysexByteArray);
    qDebug() << "***[ END slotSysExEncodePreset ] ********************************************************************";
}



void SysExEncDecode::slotSysExEncodeGlobal(QString global, QList<int> vals)
{
    qDebug() << "slotSysExEncodeGlobal called - size of globals:" << vals.size() << "global: " << global;

    unsigned int GLOBAL_LENGTH = 292; // was 290, now includes 2 bytes for sequencer LEDs
    unsigned char GLOBAL_START = 0xA1;
    unsigned int GLOBAL_LENGTH_MSB = GLOBAL_LENGTH >> 8;
    unsigned int GLOBAL_LENGTH_LSB = GLOBAL_LENGTH & 0xFF;

    encodedSysexByteArray.clear();

    sysex_out(sysex_start);

    sysex_out(0x00);
    sysex_out(0x01);
    sysex_out(0x5F);
    sysex_out(0x7A);
    sysex_out(25);      //PID = 25 for qunexus
    sysex_out(0x00);

    sysex_out(SX_PACKET_START);

    crc_init();
    midi_chunk_init();

    // ***** PREAMBLE *****
    //preamble is 6 bytes long
    // -type
    //  - ID
    //  -crc

    //Type is unsigned int, use 0x02 in lsb
    midi_sx_encode_crc_char(0x00);
    midi_sx_encode_crc_char(0x02);  //0x02 is START_OF_TEXT

    //ID unsigned int, 0x5000 is update globals
    midi_sx_encode_crc_char(0x50);  //HIGH BYTE
    midi_sx_encode_crc_char(0x00);  //LOW BYTE

    //CRC
    midi_sx_encode_crc_int(crc);
    midi_sx_flush();

    midi_sx_encode_char(GLOBAL_START);
    midi_sx_encode_char(GLOBAL_LENGTH_MSB);
    midi_sx_encode_char(GLOBAL_LENGTH_LSB);
    crc_init();

    //Encode sensitivity bytes here
    for(int i = 0; i < vals.count(); i++)
    {
        if (i > 284 && i < 288) // handle cv offsets which are signed ints, two bytes each
        {
            encodeInt(vals.at(i));
        }
        else
        {
            encode(vals.at(i));
        }
    }

    midi_sx_encode_crc_char(sumByte);

    midi_sx_flush();

    sysex_out(sysex_end);

    emit signalEncodedGlobal(encodedSysexByteArray);

}

void SysExEncDecode::slotProcessSysEx(QByteArray sysExMessageByteArray)
{
    qDebug() << "slotProcessSysEx called - size:" << sysExMessageByteArray.size();

    if(slotIsGlobals(sysExMessageByteArray))
    {

        qDebug() << "globals sysex received - fwupdateRequested: " << QuNexus_MDM->fwUpdateRequested << " globals byte count:" << sysExMessageByteArray.size();

        if(QuNexus_MDM->fwUpdateRequested)
        {
            globals.clear();
            globals = sysExMessageByteArray;
            emit signalGlobalsReceivedDoFwUd();
        }
        else
        {
            //Sends to GUI
            //emit signalSendPerKeySensitivities(sysExMessageByteArray);
            emit signalGlobalsReceived();
            slotProcessGlobals(sysExMessageByteArray);
        }
    }
    else if (slotIsPreset(sysExMessageByteArray))
    {
        qDebug() << "RX preset packet - size: " << sysExMessageByteArray.size();
        //SysExEncDecode->slotProcessPreset(sysExMessageByteArray);
        presetManager->presetLoading = true;
        slotProcessPreset(sysExMessageByteArray);
    }
}

void SysExEncDecode::slotProcessGlobals(QByteArray sysExMessageByteArray)
{
    char globalsHeader[] = {0x7E, 0x00, 0x20, 0x00};

    //----------------------------- Get header
    QByteArray header;

    for(int i = 1; i < 5; i++)
    {
        header.append(sysExMessageByteArray.at(i));
    }

    if(sysExMessageByteArray.indexOf(QByteArray(globalsHeader, 4)) == 1)
    {
        slotDecodeGlobals(sysExMessageByteArray);
    }
}

void SysExEncDecode::slotDecodeGlobals(QByteArray globalsByteArray)
{
    //qDebug() << "decode globals" << globalsByteArray;

    QByteArray perKeySensitivities;
    QByteArray table1;
    QByteArray table2;
    QByteArray cvTrims;
    QByteArray cvOffsets;

    //for(int i = 0; i < globalsByteArray.length(); i++)
    //{
        //qDebug() << "byte" << (unsigned int)globalsByteArray.at(i);
    //}

    //An 8 byte packet of sysex chars
    unsigned char  buffer[8];
    int         numPackets = 42;


    //For each of the 42 packets...
    for(int i = 1; i <= numPackets; i++)
    {
        //Clear buffer
        //buffer.clear();

        //Load Buffer, starting with first packet, index 8
        for(int j =0; j < 8; j++)
        {

            //qDebug() << "index:" << j + (i*8) << (unsigned char)globalsByteArray.at(j + (i*8));
            //Use packet num i to get index into global Byte array
            buffer[j] = globalsByteArray.at(j + (i*8));
        }

        //Decode packet
        for(int j =0; j < 7; j++)
        {
            //If decode byte bit 0 is high
            if(buffer[7] & 0x01)
            {
                buffer[j] |= 0x80; // set bit 7 high
            }

            buffer[7] >>=1; // shift decode buffer right by 1

            //qDebug() << "packet " << i << "global index" << j + (i*8) << "packet index" << j << "val" << (unsigned int)buffer[j];

            int globalIndex = j + (i*8);
            //-------------------------------------------- Per Key Sensitivities
            if(globalIndex >= 8 && globalIndex <= 35)
            {
                perKeySensitivities.append(buffer[j]);
            }
            else if(globalIndex >= 36 && globalIndex <= 181)
            {
                table1.append(buffer[j]);
            }
            else if(globalIndex >= 182 && globalIndex <= 328)
            {
                table2.append(buffer[j]);
            }
            else if(globalIndex >= 329 && globalIndex <= 331)
            {
                cvTrims.append(buffer[j]);
            }
            else if(globalIndex >= 332 && globalIndex <= 338)
            {
                cvOffsets.append(buffer[j]);
                //qDebug() << "DECODE" << QString("0x%1").arg(buffer[j],0,16);

            }
        }
    }

    //Set Respective Globals UIs
    emit signalSendGlobals(QString("Per Key Sensitivities"), perKeySensitivities);
    emit signalSendGlobals("Table 1", table1);
    emit signalSendGlobals("Table 2", table2);
    emit signalSendGlobals("CV Trims", cvTrims);
    emit signalSendGlobals("CV Offsets", cvOffsets);

}

bool SysExEncDecode::slotIsGlobals(QByteArray ba)
{
    char globalsHeader[] = {0x7E, 0x00, 0x20, 0x00};

    if(ba.indexOf(QByteArray(globalsHeader, 4)) == 1 && ba.size() == 345)
    {
        return true;
    }

    return false;
}

void SysExEncDecode::slotRestoreGlobals(QByteArray globalsByteArray)
{
    qDebug() << "RESTORE globals size" << globalsByteArray;

    QList<int> globalsList;
    QByteArray cvOffsets;

    //An 8 byte packet of sysex chars
    unsigned char  buffer[8];
    int         numPackets = 42;

    //For each of the 42 packets...
    for(int i = 1; i <= numPackets; i++)
    {
        //Clear buffer
        //buffer.clear();

        //Load Buffer, starting with first packet, index 8
        for(int j =0; j < 8; j++)
        {

            //qDebug() << "index:" << j + (i*8) << (unsigned char)globalsByteArray.at(j + (i*8));
            //Use packet num i to get index into global Byte array
            buffer[j] = globalsByteArray.at(j + (i*8));
        }

        //Decode packet
        for(int j =0; j < 7; j++)
        {
            //If decode byte is 1
            if(buffer[7] & 0x01)
            {
                buffer[j] |= 0x80;
            }

            buffer[7] >>=1;

            //qDebug() << "packet " << i << "global index" << j + (i*8) << "packet index" << j << "val" << (unsigned int)buffer[j];

            int globalIndex = j + (i*8);
            //-------------------------------------------- Per Key Sensitivities
            if(globalIndex >= 8 && globalIndex <= 35)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 36 && globalIndex <= 181)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 182 && globalIndex <= 328)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 329 && globalIndex <= 331)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 332 && globalIndex <= 338)
            {
                cvOffsets.append(buffer[j]);
                qDebug() << "restore this value" << buffer[j];
            }
        }
    }

    //--------------------------------- Format Offsets
    int16_t voltPerOctaveMSB = cvOffsets.at(0) & 0xFF;
    int16_t voltPerOctaveLSB = cvOffsets.at(1) & 0xFF;

    voltPerOctaveMSB =  voltPerOctaveMSB << 8;
    voltPerOctaveMSB = voltPerOctaveMSB | voltPerOctaveLSB;

    int16_t _1_2_voltPerOctaveMSB = cvOffsets.at(2) & 0xFF;
    int16_t _1_2_voltPerOctaveLSB = cvOffsets.at(3) & 0xFF;

    _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB << 8;
    _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB | _1_2_voltPerOctaveLSB;

    int16_t hzPerVolteMSB = cvOffsets.at(4) & 0xFF;
    int16_t hzPerVolteLSB = cvOffsets.at(5) & 0xFF;

    hzPerVolteMSB = hzPerVolteMSB << 8;
    hzPerVolteMSB = hzPerVolteMSB | hzPerVolteLSB;

    //-------------------------------- Append to globals list

    globalsList.append(voltPerOctaveMSB);
    globalsList.append(_1_2_voltPerOctaveMSB);
    globalsList.append(hzPerVolteMSB);


    slotSysExEncodeGlobal("Encode Globals", globalsList);


}

bool SysExEncDecode::slotIsPreset(QByteArray ba)
{
    char presetRxHeader[] = {0x00, 0x01, 0x5F, 0x19, 0x00, 0x22, 0x30};
    //F0 00 01 5F 19 00 22 30

    qDebug() << "Preset check- index: " << ba.indexOf(QByteArray(presetRxHeader, 1));
    if(ba.indexOf(QByteArray(presetRxHeader, 1)) == 1)
    {
        return true;
    }

    return false;
}

// load preset sysex packet into QUNEXUS JSON
void SysExEncDecode::slotProcessPreset(QByteArray presetPacket)
{
    qDebug() << "---[ BEGIN slotProccessPreset ] --------------------------------------------------------------------";

    //An 8 byte packet of sysex chars
    uchar   buffer[8];
    uint    numPackets = 72; // (593-9)/8 - 1
    uint    counter = 0;
    int     JSONCounter = -1;
    bool    presetIsV2;
    uchar   presetNum;

    unsigned char lastByte; // store the last byte for ints and other special chars
    bool skipFlag = 0;;

    //For each of the 73 packets...
    for(uint thisPacket = 0; thisPacket <= numPackets; thisPacket++)
    {
        //Load Buffer, starting with first packet, index 8
        for(int thisByte = 0; thisByte < 8; thisByte++)
        {
            int thisIndex = SIZE_OF_PRESET_HEADER + (thisByte + (thisPacket * 8));
            //qDebug() << "index:" << j + (i*8) << (unsigned char)globalsByteArray.at(j + (i*8));
            //Use packet num i to get index into preset Byte array
            buffer[thisByte] = presetPacket.at(thisIndex);
        }

        //Decode packet
        for(int thisByte = 0; thisByte < 7; thisByte++)
        {
            //If decode byte bit 0 is high
            if(buffer[7] & 0x01)
            {
                buffer[thisByte] |= 0x80; // set bit 7 high
            }

            buffer[7] >>=1; // shift decode buffer right by 1

            //-------------------------------------------------
            // bytes are decoded, now load them into the editor
            //-------------------------------------------------

            switch (counter)
            {
            case 0: // presetNum
                presetNum = buffer[thisByte]; // store preset number
                JSONCounter++;
#ifdef SYX_DEBUG_PRESET_PACKET
                qDebug() << "PresetNum: " << presetNum;
#endif
                break;

            default:
//                if (JSONCounter == 494 ||
//                    JSONCounter == 495)
//                {
//                    qDebug() << "xxx Counter: " << counter << " JSONCounter: " << JSONCounter  << keyLayerJSON[JSONCounter] << "ThisByte: " << buffer[thisByte] << " param type: " << paramTypeMap[keyLayerJSON[JSONCounter]];
//                    qDebug() << "xxx Counter: " << counter << " JSONCounter: " << JSONCounter+1 << keyLayerJSON[JSONCounter+1] << "ThisByte: " << buffer[thisByte] << " param type: " << paramTypeMap[keyLayerJSON[JSONCounter+1]];
//                }
                if (JSONCounter == -1)
                {
                    JSONCounter++; // skip this, avoid out of range error
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "skip JSONCounter = -1";
#endif
                }

                // handle 2 : 6 bitspace values
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "bs26right")
                {
                    unsigned char thisLeft, thisRight = 0;
                    QString thisParam = keyLayerJSON[JSONCounter];

                    // bytes in bit space are MSB/LSB left/right, so ordering is reversed
                    thisLeft = LEFT6BITS(buffer[thisByte]);
                    thisRight = RIGHT2BITS(buffer[thisByte]);

                    if (thisParam == "SEQ_track_1_dest" ||
                        thisParam == "SEQ_track_2_dest" ||
                        thisParam == "SEQ_track_3_dest")
                    {
                        // test if ranges are valid
                        if (slotTestQListRange(&MIDI_Out, thisRight, JSONCounter))
                        {
                            // source
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], MIDI_Out[thisRight]); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                            qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << " RIGHT2: " << thisRight << " -- " << keyLayerJSON[JSONCounter] << "ThisByte: " << buffer[thisByte] << "MIDI_Out: " << MIDI_Out[thisRight];
#endif
                        }
                        JSONCounter++;

                        if (slotTestQListRange(&MIDI_CH, thisLeft, JSONCounter))
                        {
                            // destination
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], MIDI_CH[thisLeft]); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                            qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << " LEFT6: " << thisLeft << " -- " << keyLayerJSON[JSONCounter+1] << "ThisByte: " << buffer[thisByte] << "MIDI_CH: " << MIDI_CH[thisLeft];
#endif
                        }
                        JSONCounter++;
                    }
                }
                // handle 4 : 4 bitspace values
                else if (paramTypeMap[keyLayerJSON[JSONCounter] ] == "bs44right")
                {
                    unsigned char thisLeft, thisRight = 0;
                    QString thisParam = keyLayerJSON[JSONCounter];

                    thisLeft = LEFT4BITS(buffer[thisByte]);
                    thisRight = RIGHT4BITS(buffer[thisByte]);

                    // bytes in bit space are MSB/LSB left/right, so ordering is reversed
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << " RIGHT4: " << thisRight << " -- " << keyLayerJSON[JSONCounter] << "ThisByte: " << buffer[thisByte];
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter+1 << " LEFT4: " << thisLeft << " -- " << keyLayerJSON[JSONCounter+1] << "ThisByte: " << buffer[thisByte];
#endif
                    if (thisParam == "SEQ_clock_source")

                    {
                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_clock_source, thisRight, JSONCounter))
                        {
#ifdef SYX_DEBUG_PRESET_PACKET
                            qDebug() << "SEQ_clock_source[thisRight]: " << SEQ_clock_source[thisRight];
#endif
                            // source
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_clock_source[thisRight]); // write value to edit buffer
                        }
                        JSONCounter++;

                        if (slotTestQListRange(&MIDI_Out, thisLeft, JSONCounter))
                        {
                            // destination
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], MIDI_Out[thisLeft]); // write value to edit buffer
                        }
                        JSONCounter++;
                    }
                    else if (thisParam == "SEQ_KB_lower")
                    {
                        // lower
                        editBufferPresetMap->insert(keyLayerJSON[JSONCounter++], thisRight); // write value to edit buffer
                        // upper
                        editBufferPresetMap->insert(keyLayerJSON[JSONCounter++], thisLeft); // write value to edit buffer

                    }
                    else if (  thisParam == "SEQ_track_1_mode" ||
                               thisParam == "SEQ_track_2_mode" ||
                               thisParam == "SEQ_track_3_mode"  )
                    {
                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_mode, thisRight, JSONCounter))
                        {
                            // mode
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_mode[thisRight]); // write value to edit buffer
                        }
                        JSONCounter++;

                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_polyphony, thisLeft, JSONCounter))
                        {
                            // polyphony
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_polyphony[thisLeft]); // write value to edit buffer
                        }
                        JSONCounter++;
                    }
                    else if (thisParam == "SEQ_track_1_arp_pattern" ||
                             thisParam == "SEQ_track_2_arp_pattern" ||
                             thisParam == "SEQ_track_3_arp_pattern")
                    {
                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_arp_pattern, thisRight, JSONCounter))
                        {
                            // arp play pattern
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_arp_pattern[thisRight]); // write value to edit buffer
                        }
                        JSONCounter++;

                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_seq_division, thisLeft, JSONCounter))
                        {
                            // sequencer pattern #
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_seq_division[thisLeft]); // write value to edit buffer
                        }
                        JSONCounter++;

                    }
                    else if (thisParam == "SEQ_track_1_division" ||
                             thisParam == "SEQ_track_2_division" ||
                             thisParam == "SEQ_track_3_division")
                    {
                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_division, thisRight, JSONCounter))
                        {
                            // division
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_division[thisRight]); // write value to edit buffer
                        }
                        JSONCounter++;

                        // test if ranges are valid
                        if (slotTestQListRange(&SEQ_track_dot, thisLeft, JSONCounter))
                        {
                            // dot/triplet
                            editBufferPresetMap->insert(keyLayerJSON[JSONCounter], SEQ_track_dot[thisLeft]); // write value to edit buffer
                        }
                        JSONCounter++;
                    }
                }

                // handle 16 bit integers
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "16b_int" ||
                         paramTypeMap[ keyLayerJSON[JSONCounter] ] == "signed int")
                {

                    if (skipFlag)
                    {
                        int thisInt = ( lastByte << 8 ) + buffer[thisByte];

                        editBufferPresetMap->insert(keyLayerJSON[JSONCounter], thisInt); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << " Integer: " << thisInt << " -- " << keyLayerJSON[JSONCounter] << "ThisByte: " << buffer[thisByte] << "LastByte: " << lastByte;
#endif
                        JSONCounter++;
                        skipFlag = 0;
                        lastByte = 0; // clear
                    }
                    else
                    {
                        skipFlag = 1; // flag
                        lastByte = buffer[thisByte]; // integer, store MSB for next
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "skip byte: " << lastByte;
#endif
                    }

                }

                // 8bit chars
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "char" ||
                         paramTypeMap[ keyLayerJSON[JSONCounter] ] == "Checkbox")
                {
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "Char/Checkbox: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    // correct program changes that are unsigned chars yet firmware sends "-1" for off
                    if (keyLayerJSON[JSONCounter].contains("Keyboard_Global_Program_Change") &&
                            buffer[thisByte] == 255)
                    {
                         editBufferPresetMap->insert(keyLayerJSON[JSONCounter], -1); // write value to edit buffer
                    }
                    else
                    {
                         editBufferPresetMap->insert(keyLayerJSON[JSONCounter], buffer[thisByte]); // write value to edit buffer
                    }

                    JSONCounter++;
                }

                // signed char
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "signed char")
                {

                    // EB TODO - implement signed chars
                    int thisFixedChar = (signed char)buffer[thisByte];
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "signed char: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter] << " fixed char" << thisFixedChar;
#endif
                    editBufferPresetMap->insert(keyLayerJSON[JSONCounter], thisFixedChar); // write value to edit buffer
                    JSONCounter++;
                }

                // lookup data types

                // Curves
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "Curve")
                {
                    editBufferPresetMap->insert( keyLayerJSON[JSONCounter], Curves[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "Curve: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    JSONCounter++;
                }

                // CV_Out_Source
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "CV_Out_Source")
                {
                    // translate bit flipped byte into enum
                    QString thisEnum;
                    switch (buffer[thisByte])
                    {
                    case CV_OFF:
                        thisEnum = "Off";
                        break;
                    case TRACK1:
                        thisEnum = "Track 1";
                        break;
                    case TRACK2:
                        thisEnum = "Track 2";
                        break;
                    case TRACK3:
                        thisEnum = "Track 3";
                        break;
                    case ALL_TRACKS:
                        thisEnum = "All Tracks";
                        break;
                    case MIDI_EXPANDER:
                        thisEnum = "Expander";
                        break;
                    case USB3:
                        thisEnum = "USB Port 3";
                        break;
                    case USB3_AND_EXPANDER:
                        thisEnum = "USB 3 + Expander";
                        break;
                    case TRACK1_USB3_EXPANDER:
                        thisEnum = "Track 1 + USB 3 + Exp.";
                        break;
                    case TRACK2_USB3_EXPANDER:
                        thisEnum = "Track 2 + USB 3 + Exp.";
                        break;
                    case TRACK3_USB3_EXPANDER:
                        thisEnum = "Track 3 + USB 3 + Exp.";
                        break;
                    case ALLTRACKS_USB3_EXPANDER:
                        thisEnum = "All Tracks + USB 3 + Exp.";
                        break;
                    case CLOCK:
                        thisEnum = "Clock";
                        break;
                    default: // catch bad values
                        thisEnum = "Off";
                        break;
                    }
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "CV_Out_Source: " << buffer[thisByte] << "thisEnum: " << thisEnum << " -- " << keyLayerJSON[JSONCounter];
#endif
                    editBufferPresetMap->insert( keyLayerJSON[JSONCounter], thisEnum ); // write value to edit buffer
                    JSONCounter++;
                }

                // CV_Out_Type
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "CV_Out_Type")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&CV_Out_Type, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], CV_Out_Type[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "CV_Out_Type: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // CV_Scaling
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "CV_Scaling")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&CV_Scaling, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], CV_Scaling[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "CV_Scaling: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // KB_Source
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "KB_Source")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&KB_Source, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], KB_Source[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "KB_Source: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // LED_Refresh
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "LED_Refresh")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&LED_Refresh, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], LED_Refresh[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "LED_Refresh: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // Key_Priority
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "Key_Priority")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&Key_Priority, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], Key_Priority[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "Key_Priority: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // Preset_Struct_is_V2
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "Preset_Struct_is_V2")
                {
                    editBufferPresetMap->insert( keyLayerJSON[JSONCounter], (buffer[thisByte] == 36) ? 36 : 0);
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "Preset_Struct_is_V2: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    JSONCounter++;
                    presetIsV2 = true;
                }

                // MIDI_Out
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "MIDI_Out")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&MIDI_Out, buffer[thisByte], JSONCounter))
                    {
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "MIDI_Out: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter] << "MIDI_Out translated: " << MIDI_Out[buffer[thisByte]];
#endif
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], MIDI_Out[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "New editBufferPresetMap value: " << editBufferPresetMap->value(keyLayerJSON[JSONCounter]);
#endif
                    }
                    JSONCounter++;

                }

                // CV_MIDI_Out
                else if (paramTypeMap[ keyLayerJSON[JSONCounter] ] == "CV_MIDI_Out")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&CV_MIDI_Out, buffer[thisByte], JSONCounter))
                    {
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "CV_MIDI_Out: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], CV_MIDI_Out[buffer[thisByte]] ); // write value to edit buffer
                    }
                    JSONCounter++;
                }

                // Toggle_Modes
                else if (paramTypeMap[keyLayerJSON[JSONCounter] ] == "Toggle_Modes")
                {
                    // test if ranges are valid
                    if (slotTestQListRange(&Toggle_Modes, buffer[thisByte], JSONCounter))
                    {
                        editBufferPresetMap->insert( keyLayerJSON[JSONCounter], Toggle_Modes[buffer[thisByte]] ); // write value to edit buffer
#ifdef SYX_DEBUG_PRESET_PACKET
                        qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "Toggle_Modes: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter];
#endif
                    }
                    JSONCounter++;
                }

                // no match
                else
                {
#ifdef SYX_DEBUG_PRESET_PACKET
                    qDebug() << "Counter: " << counter << " JSONCounter: " << JSONCounter << "no match: " << buffer[thisByte] << " -- " << keyLayerJSON[JSONCounter] << " param type: " << paramTypeMap[keyLayerJSON[JSONCounter]];
#endif
                    JSONCounter++;
                }

                break;
            }
            counter++; // advance
            if (counter >= PRESET_LENGTH) thisByte = 8; // exit for loop

        } // end decode packet
        if (counter >= PRESET_LENGTH) thisPacket = numPackets + 1; // exit for loop
    } // end preset
    emit signalPresetProcessed(presetNum);
    if (outOfRangeFlag)
    {
#ifdef SYX_DEBUG_PRESET_PACKET
        qDebug() << "End preset packet, params out of range: ";
        qDebug().noquote() << OORParams;
#endif
        int boxWidth = 500;
        int boxHeight = 200;

        QDialog *msgBox = new QDialog(this);
        msgBox->setMinimumSize(boxWidth, boxHeight);
        msgBox->setFixedSize(boxWidth, boxHeight);

        QPoint centerparent(
                    this->x() + ((APP_WIDTH - boxWidth) /2),
                    this->y() + ((APP_HEIGHT - boxHeight) /2));

        msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        msgBox->move(centerparent);

        msgBox->setStyleSheet("QWidget{background: rgba(120,120,120, 255);}");

        QString dialogText;

        if (presetIsV2)
        {
            dialogText = QString("Error: Corrupt or Invalid QuNexus 2.0 Preset Data!\nWe recommend you load the factory default\n presets and send them to your QuNexus preset slots.");
        }
        else
        {
            dialogText = QString("Error: Corrupt or Incompatible QuNexus 1.0 Preset Data!\nWe recommend you load the factory default\n presets and send them to your QuNexus preset slots.");
        }

        QLabel* text = new QLabel(dialogText, msgBox, Qt::WindowFlags());
        text->setAlignment(Qt::AlignCenter);
        text->setMinimumSize(boxWidth - 20, 30);
        text->setFixedSize(boxWidth - 20, 30);
        text->move(10, 10);

        QLabel* errorList = new QLabel(OORParams, msgBox, Qt::WindowFlags());
        errorList->setText(OORParams);
        errorList->setMinimumSize(280, 70);
        errorList->setFixedSize(280, 70);
        errorList->move(10, 60);

        int buttonWidth = 70;
        int buttonHeight = 28;

        QPushButton* okButton = new QPushButton(msgBox);
        okButton->setText("Continue");
        okButton->setGeometry(QRect((boxWidth - buttonWidth) / 2, (boxHeight - buttonHeight - 10), buttonWidth, buttonHeight));
        connect(okButton, SIGNAL(clicked()), msgBox, SLOT(accept()));

        msgBox->exec();


        // reset for next param sync
        outOfRangeFlag = false;
        OORParams = "";
    }
    qDebug() << "---[ END slotProccessPreset ] --------------------------------------------------------------------";

}

// test our QStringLists and catch out of range errors before they happen
bool SysExEncDecode::slotTestQListRange(QStringList *thisList, int thisIndex, int thisJSONCounter)
{
    // test if ranges are valid
    if (thisIndex < thisList->size() && thisIndex >= 0)
    {
       return true;
    }
    else
    {
        qDebug() << "******* Out of Range Value ******* keyLayerJSON: " << keyLayerJSON[thisJSONCounter] << " JSONCounter:" << thisJSONCounter << "index/value: " << thisIndex;
        OORParams = OORParams.append(QString(keyLayerJSON[thisJSONCounter]).append("\n"));
        outOfRangeFlag = true;
        return false;
    }
}




