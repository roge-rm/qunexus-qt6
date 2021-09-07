// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "sysexdecode.h"
#include <QDebug>

#define SIZE_OF_PRESET_HEADER 8

SysExDecode::SysExDecode(QWidget *parent) :
    QWidget(parent)
{
}

void SysExDecode::slotProcessGlobals(QByteArray sysExMessageByteArray)
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

void SysExDecode::slotDecodeGlobals(QByteArray globalsByteArray)
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

bool SysExDecode::slotIsGlobals(QByteArray ba)
{
    char globalsHeader[] = {0x7E, 0x00, 0x20, 0x00};

    if(ba.indexOf(QByteArray(globalsHeader, 4)) == 1 && ba.size() == 345)
    {
        return true;
    }

    return false;
}

bool SysExDecode::slotIsPreset(QByteArray ba)
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

// load preset sysex packet into QUNEXUS struct
void SysExDecode::slotProcessPreset(QByteArray presetPacket)
{
    //An 8 byte packet of sysex chars
    unsigned char  buffer[8];
    int             numPackets = 72; // (593-9)/8 - 1
    int             counter = 0;
    unsigned char * qnPtr = &qn.presetNum; // read the preset into the QUNEXUS struct

    unsigned char * ptrStart = qnPtr;
    qDebug() << "qnPtr init: " << qnPtr;
    //For each of the 73 packets...
    for(int thisPacket = 0; thisPacket <= numPackets; thisPacket++)
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

            if (counter == 25 || counter == 31)
            {
                qDebug() << "CHECK: " << counter;
                qDebug() << "presetnum: " << qn.presetNum;
                qDebug() << "pc0: " << qn.programChangeNumber[0];
                qDebug() << "globalLEDRefreshMode: " << qn.global.globalLEDRefreshMode;
                qDebug() << "globalTranspose: " << qn.global.globalTranspose;
                qDebug() << "mlc.gain: " << qn.keyboardLayerCCSettings[0].modlineConstants.gain;
            }
            *qnPtr = buffer[thisByte];
            qDebug() << "counter: "<< counter++ << "packet: " << thisPacket << ", byte index: " << thisByte << ", val: " << (unsigned int)buffer[thisByte] << ", qnPtr: " << qnPtr - ptrStart;
            qnPtr++;

        } // end decode packet
    } // end preset
    emit signalPresetReceived(qn.presetNum);

}

// translate QUNEXUS struct into a given preset's JSON
void SysExDecode::slotDecodePreset(QVariantMap * preset)
{
    qDebug() << "slotDecodePreset called: " << preset->value("Preset_Name");
    preset->insert("Preset_Name", "[Live]");

    // hard code this for now
    preset->insert("Keyboard_Global_Program_Change_A", qn.programChangeNumber[0]);
    qDebug() << "pc0: " << qn.programChangeNumber[0];
    preset->insert("Keyboard_Global_Program_Change_B", qn.programChangeNumber[1]);
    preset->insert("Keyboard_Global_Program_Change_C", qn.programChangeNumber[2]);
    preset->insert("Keyboard_Global_Program_Change_D", qn.programChangeNumber[3]);
    preset->insert("Globals_Gain", qn.global.globalGain);
    qDebug() << "globalGain: " << qn.global.globalGain;
    preset->insert("Globals_Off_Thresh", qn.global.globalOffLevel);
    preset->insert("Globals_On_Thresh", qn.global.globalOnLevel);
    preset->insert("Globals_Tilt_Sensitivity", qn.global.globalTiltSensitivity);
    preset->insert("Keyboard_Global_Mode", qn.global.globalKeyMode);
    preset->insert("Keyboard_Global_Channel_Rotation_Active", qn.global.globalMPE);
    preset->insert("Keyboard_Global_USB_1_Channel", qn.global.globalKeyboardLayerUSB1Channel);
    preset->insert("Keyboard_Global_USB_2_Channel", qn.global.globalKeyboardLayerUSB2Channel);
    preset->insert("Keyboard_Global_Key_Selection_Criteria", qn.global.globalKeySelectionCriteria);
    preset->insert("Keyboard_Global_Velocity_Active", qn.global.globalVelocityActive);
    preset->insert("Keyboard_Global_Pressure_Active", qn.global.globalPressureActive);
    preset->insert("Keyboard_Global_Tilt_Active", qn.global.globalTiltActive);
    preset->insert("Keyboard_Global_Channel_Pressure_Active", qn.global.globalChPressureActive);
    preset->insert("Keyboard_Global_Pitch_Bend_Active", qn.global.globalPitchBendActive);
    preset->insert("Keyboard_Global_Poly_Aftertouch_Active", qn.global.globalPolyAftertouchActive);
    preset->insert("Keyboard_Global_CC_Active", qn.global.globalCCActive);
    preset->insert("Keyboard_Global_Channel_Rotation_Offset", qn.global.globalMPEOffset);
    preset->insert("Keyboard_Global_LED_Mode", qn.global.globalLEDMode);
    preset->insert("Keyboard_Global_LED_Remote_Channel", qn.global.globalLEDChannel);
    preset->insert("Keyboard_Global_Polyphony_Number", qn.global.globalPolyphonyNumber);
    preset->insert("Globals_LED_Refresh_Style", qn.global.globalLEDRefreshMode);
    preset->insert("Keyboard_Global_Transpose", qn.global.globalTranspose);
    preset->insert("Keyboard_CC_00_Control_Number", qn.keyboardLayerCCSettings[0].keyboardCCControlNumber);
    preset->insert("Keyboard_CC_00_Curve", qn.keyboardLayerCCSettings[0].modlineConstants.curve);
    preset->insert("Keyboard_CC_00_Gain", qn.keyboardLayerCCSettings[0].modlineConstants.gain);
    preset->insert("Keyboard_CC_00_Max", qn.keyboardLayerCCSettings[0].modlineConstants.max);
    preset->insert("Keyboard_CC_00_Min", qn.keyboardLayerCCSettings[0].modlineConstants.min);
    preset->insert("Keyboard_CC_00_Offset", qn.keyboardLayerCCSettings[0].modlineConstants.offset);
    preset->insert("Keyboard_CC_00_MIDI_Output_Device", qn.keyboardLayerCCSettings[0].keyboardCCMIDIOutputDevice);
    preset->insert("Keyboard_CC_00_Source", qn.keyboardLayerCCSettings[0].keyboardCCSource);
    preset->insert("Keyboard_CC_01_Control_Number", qn.keyboardLayerCCSettings[1].keyboardCCControlNumber);
    preset->insert("Keyboard_CC_01_Curve", qn.keyboardLayerCCSettings[1].modlineConstants.curve);
    qDebug() << "orig cc1 gain: " << preset->value("Keyboard_CC_01_Gain").toUInt();
    preset->insert("Keyboard_CC_01_Gain", qn.keyboardLayerCCSettings[1].modlineConstants.gain);
    qDebug() << "cc1 gain: " << qn.keyboardLayerCCSettings[1].modlineConstants.gain;
    preset->insert("Keyboard_CC_01_Max", qn.keyboardLayerCCSettings[1].modlineConstants.max);
    preset->insert("Keyboard_CC_01_Min", qn.keyboardLayerCCSettings[1].modlineConstants.min);
    preset->insert("Keyboard_CC_01_Offset", qn.keyboardLayerCCSettings[1].modlineConstants.offset);
    preset->insert("Keyboard_CC_01_MIDI_Output_Device", qn.keyboardLayerCCSettings[1].keyboardCCMIDIOutputDevice);
    preset->insert("Keyboard_CC_01_Source", qn.keyboardLayerCCSettings[1].keyboardCCSource);
    preset->insert("Keyboard_CC_02_Control_Number", qn.keyboardLayerCCSettings[2].keyboardCCControlNumber);
    preset->insert("Keyboard_CC_02_Curve", qn.keyboardLayerCCSettings[2].modlineConstants.curve);
    preset->insert("Keyboard_CC_02_Gain", qn.keyboardLayerCCSettings[2].modlineConstants.gain);
    preset->insert("Keyboard_CC_02_Max", qn.keyboardLayerCCSettings[2].modlineConstants.max);
    preset->insert("Keyboard_CC_02_Min", qn.keyboardLayerCCSettings[2].modlineConstants.min);
    preset->insert("Keyboard_CC_02_Offset", qn.keyboardLayerCCSettings[2].modlineConstants.offset);
    preset->insert("Keyboard_CC_02_MIDI_Output_Device", qn.keyboardLayerCCSettings[2].keyboardCCMIDIOutputDevice);
    preset->insert("Keyboard_CC_02_Source", qn.keyboardLayerCCSettings[2].keyboardCCSource);
    preset->insert("Keyboard_Channel_Pressure_Curve", qn.keyboardLayerChannelPressureSettings.modlineConstants.curve);
    preset->insert("Keyboard_Channel_Pressure_Gain", qn.keyboardLayerChannelPressureSettings.modlineConstants.gain);
    preset->insert("Keyboard_Channel_Pressure_Max", qn.keyboardLayerChannelPressureSettings.modlineConstants.max);
    preset->insert("Keyboard_Channel_Pressure_Min", qn.keyboardLayerChannelPressureSettings.modlineConstants.min);
    preset->insert("Keyboard_Channel_Pressure_Offset", qn.keyboardLayerChannelPressureSettings.modlineConstants.offset);
    preset->insert("Keyboard_Channel_Pressure_MIDI_Output_Device", qn.keyboardLayerChannelPressureSettings.keyboardChannelPressureMIDIOutputDevice);
    preset->insert("Keyboard_Channel_Pressure_Source", qn.keyboardLayerChannelPressureSettings.keyboardChannelPressureSource);
    preset->insert("Keyboard_Pitch_Bend_Curve", qn.keyboardLayerPitchBendSettings.modlineConstants.curve);
    preset->insert("Keyboard_Pitch_Bend_Gain", qn.keyboardLayerPitchBendSettings.modlineConstants.gain);
    preset->insert("Keyboard_Pitch_Bend_Max", qn.keyboardLayerPitchBendSettings.modlineConstants.max);
    preset->insert("Keyboard_Pitch_Bend_Min", qn.keyboardLayerPitchBendSettings.modlineConstants.min);
    preset->insert("Keyboard_Pitch_Bend_Offset", qn.keyboardLayerPitchBendSettings.modlineConstants.offset);
    preset->insert("Keyboard_Pitch_Bend_MIDI_Output_Device", qn.keyboardLayerPitchBendSettings.keyboardPitchBendMIDIOutputDevice);
    preset->insert("Keyboard_Pitch_Bend_Source", qn.keyboardLayerPitchBendSettings.keyboardPitchBendSource);
    preset->insert("Keyboard_Poly_Aftertouch_Curve", qn.keyboardLayerPolyAftertouchSettings.modlineConstants.curve);
    preset->insert("Keyboard_Poly_Aftertouch_Gain", qn.keyboardLayerPolyAftertouchSettings.modlineConstants.gain);
    preset->insert("Keyboard_Poly_Aftertouch_Max", qn.keyboardLayerPolyAftertouchSettings.modlineConstants.max);
    preset->insert("Keyboard_Poly_Aftertouch_Min", qn.keyboardLayerPolyAftertouchSettings.modlineConstants.min);
    preset->insert("Keyboard_Poly_Aftertouch_Offset", qn.keyboardLayerPolyAftertouchSettings.modlineConstants.offset);
    preset->insert("Keyboard_Poly_Aftertouch_MIDI_Output_Device", qn.keyboardLayerPolyAftertouchSettings.keyboardPolyAftertouchMIDIOutputDevice);
    preset->insert("Keyboard_Poly_Aftertouch_Source", qn.keyboardLayerPolyAftertouchSettings.keyboardPolyAftertouchSource);
    preset->insert("Keyboard_Velocity_Curve", qn.keyboardLayerVelocitySettings.modlineConstants.curve);
    preset->insert("Keyboard_Velocity_Gain", qn.keyboardLayerVelocitySettings.modlineConstants.gain);
    preset->insert("Keyboard_Velocity_Max", qn.keyboardLayerVelocitySettings.modlineConstants.max);
    preset->insert("Keyboard_Velocity_Min", qn.keyboardLayerVelocitySettings.modlineConstants.min);
    preset->insert("Keyboard_Velocity_Offset", qn.keyboardLayerVelocitySettings.modlineConstants.offset);
    preset->insert("Keyboard_Velocity_MIDI_Output_Device", qn.keyboardLayerVelocitySettings.keyboardVelocityMIDIOutputDevice);
    preset->insert("Controller_Global_MIDI_Output_Device", qn.controllerLayerMidiOutputDevice);
    preset->insert("Controller_Key_0_Controller_Layer_Enable_Disable", qn.controllerLayerKey[0].controllerLayerActive);
    preset->insert("Controller_Key_0_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[0].keyboardLayerParticipate);
    preset->insert("Controller_Key_0_Note_Number", qn.controllerLayerKey[0].noteNumber);
    preset->insert("Controller_Key_0_Pressure_Control_Number", qn.controllerLayerKey[0].pressureCCNumber);
    preset->insert("Controller_Key_0_Tilt_Control_Number", qn.controllerLayerKey[0].tiltCCNumber);
    preset->insert("Controller_Key_0_Channel", qn.controllerLayerKey[0].midiChannel);
    preset->insert("Controller_Key_0_Note_Velocity", qn.controllerLayerKey[0].velocityValue);
    preset->insert("Controller_Key_0_Pressure_Return_Value", qn.controllerLayerKey[0].pressureReturnValue);
    preset->insert("Controller_Key_0_Tilt_Return_Value", qn.controllerLayerKey[0].tiltReturnValue);
    preset->insert("Controller_Key_0_Toggle_Control_Number", qn.controllerLayerKey[0].toggleCCNumber);
    preset->insert("Controller_Key_0_Toggle_Value", qn.controllerLayerKey[0].toggleValue);
    preset->insert("Controller_Key_0_Toggle_Return_Value", qn.controllerLayerKey[0].toggleReturnValue);
    preset->insert("Controller_Key_0_Toggle_On_Off", qn.controllerLayerKey[0].toggleEnabled);
    preset->insert("Controller_Key_1_Controller_Layer_Enable_Disable", qn.controllerLayerKey[1].controllerLayerActive);
    preset->insert("Controller_Key_1_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[1].keyboardLayerParticipate);
    preset->insert("Controller_Key_1_Note_Number", qn.controllerLayerKey[1].noteNumber);
    preset->insert("Controller_Key_1_Pressure_Control_Number", qn.controllerLayerKey[1].pressureCCNumber);
    preset->insert("Controller_Key_1_Tilt_Control_Number", qn.controllerLayerKey[1].tiltCCNumber);
    preset->insert("Controller_Key_1_Channel", qn.controllerLayerKey[1].midiChannel);
    preset->insert("Controller_Key_1_Note_Velocity", qn.controllerLayerKey[1].velocityValue);
    preset->insert("Controller_Key_1_Pressure_Return_Value", qn.controllerLayerKey[1].pressureReturnValue);
    preset->insert("Controller_Key_1_Tilt_Return_Value", qn.controllerLayerKey[1].tiltReturnValue);
    preset->insert("Controller_Key_1_Toggle_Control_Number", qn.controllerLayerKey[1].toggleCCNumber);
    preset->insert("Controller_Key_1_Toggle_Value", qn.controllerLayerKey[1].toggleValue);
    preset->insert("Controller_Key_1_Toggle_Return_Value", qn.controllerLayerKey[1].toggleReturnValue);
    preset->insert("Controller_Key_1_Toggle_On_Off", qn.controllerLayerKey[1].toggleEnabled);
    preset->insert("Controller_Key_2_Controller_Layer_Enable_Disable", qn.controllerLayerKey[2].controllerLayerActive);
    preset->insert("Controller_Key_2_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[2].keyboardLayerParticipate);
    preset->insert("Controller_Key_2_Note_Number", qn.controllerLayerKey[2].noteNumber);
    preset->insert("Controller_Key_2_Pressure_Control_Number", qn.controllerLayerKey[2].pressureCCNumber);
    preset->insert("Controller_Key_2_Tilt_Control_Number", qn.controllerLayerKey[2].tiltCCNumber);
    preset->insert("Controller_Key_2_Channel", qn.controllerLayerKey[2].midiChannel);
    preset->insert("Controller_Key_2_Note_Velocity", qn.controllerLayerKey[2].velocityValue);
    preset->insert("Controller_Key_2_Pressure_Return_Value", qn.controllerLayerKey[2].pressureReturnValue);
    preset->insert("Controller_Key_2_Tilt_Return_Value", qn.controllerLayerKey[2].tiltReturnValue);
    preset->insert("Controller_Key_2_Toggle_Control_Number", qn.controllerLayerKey[2].toggleCCNumber);
    preset->insert("Controller_Key_2_Toggle_Value", qn.controllerLayerKey[2].toggleValue);
    preset->insert("Controller_Key_2_Toggle_Return_Value", qn.controllerLayerKey[2].toggleReturnValue);
    preset->insert("Controller_Key_2_Toggle_On_Off", qn.controllerLayerKey[2].toggleEnabled);
    preset->insert("Controller_Key_3_Controller_Layer_Enable_Disable", qn.controllerLayerKey[3].controllerLayerActive);
    preset->insert("Controller_Key_3_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[3].keyboardLayerParticipate);
    preset->insert("Controller_Key_3_Note_Number", qn.controllerLayerKey[3].noteNumber);
    preset->insert("Controller_Key_3_Pressure_Control_Number", qn.controllerLayerKey[3].pressureCCNumber);
    preset->insert("Controller_Key_3_Tilt_Control_Number", qn.controllerLayerKey[3].tiltCCNumber);
    preset->insert("Controller_Key_3_Channel", qn.controllerLayerKey[3].midiChannel);
    preset->insert("Controller_Key_3_Note_Velocity", qn.controllerLayerKey[3].velocityValue);
    preset->insert("Controller_Key_3_Pressure_Return_Value", qn.controllerLayerKey[3].pressureReturnValue);
    preset->insert("Controller_Key_3_Tilt_Return_Value", qn.controllerLayerKey[3].tiltReturnValue);
    preset->insert("Controller_Key_3_Toggle_Control_Number", qn.controllerLayerKey[3].toggleCCNumber);
    preset->insert("Controller_Key_3_Toggle_Value", qn.controllerLayerKey[3].toggleValue);
    preset->insert("Controller_Key_3_Toggle_Return_Value", qn.controllerLayerKey[3].toggleReturnValue);
    preset->insert("Controller_Key_3_Toggle_On_Off", qn.controllerLayerKey[3].toggleEnabled);
    preset->insert("Controller_Key_4_Controller_Layer_Enable_Disable", qn.controllerLayerKey[4].controllerLayerActive);
    preset->insert("Controller_Key_4_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[4].keyboardLayerParticipate);
    preset->insert("Controller_Key_4_Note_Number", qn.controllerLayerKey[4].noteNumber);
    preset->insert("Controller_Key_4_Pressure_Control_Number", qn.controllerLayerKey[4].pressureCCNumber);
    preset->insert("Controller_Key_4_Tilt_Control_Number", qn.controllerLayerKey[4].tiltCCNumber);
    preset->insert("Controller_Key_4_Channel", qn.controllerLayerKey[4].midiChannel);
    preset->insert("Controller_Key_4_Note_Velocity", qn.controllerLayerKey[4].velocityValue);
    preset->insert("Controller_Key_4_Pressure_Return_Value", qn.controllerLayerKey[4].pressureReturnValue);
    preset->insert("Controller_Key_4_Tilt_Return_Value", qn.controllerLayerKey[4].tiltReturnValue);
    preset->insert("Controller_Key_4_Toggle_Control_Number", qn.controllerLayerKey[4].toggleCCNumber);
    preset->insert("Controller_Key_4_Toggle_Value", qn.controllerLayerKey[4].toggleValue);
    preset->insert("Controller_Key_4_Toggle_Return_Value", qn.controllerLayerKey[4].toggleReturnValue);
    preset->insert("Controller_Key_4_Toggle_On_Off", qn.controllerLayerKey[4].toggleEnabled);
    preset->insert("Controller_Key_5_Controller_Layer_Enable_Disable", qn.controllerLayerKey[5].controllerLayerActive);
    preset->insert("Controller_Key_5_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[5].keyboardLayerParticipate);
    preset->insert("Controller_Key_5_Note_Number", qn.controllerLayerKey[5].noteNumber);
    preset->insert("Controller_Key_5_Pressure_Control_Number", qn.controllerLayerKey[5].pressureCCNumber);
    preset->insert("Controller_Key_5_Tilt_Control_Number", qn.controllerLayerKey[5].tiltCCNumber);
    preset->insert("Controller_Key_5_Channel", qn.controllerLayerKey[5].midiChannel);
    preset->insert("Controller_Key_5_Note_Velocity", qn.controllerLayerKey[5].velocityValue);
    preset->insert("Controller_Key_5_Pressure_Return_Value", qn.controllerLayerKey[5].pressureReturnValue);
    preset->insert("Controller_Key_5_Tilt_Return_Value", qn.controllerLayerKey[5].tiltReturnValue);
    preset->insert("Controller_Key_5_Toggle_Control_Number", qn.controllerLayerKey[5].toggleCCNumber);
    preset->insert("Controller_Key_5_Toggle_Value", qn.controllerLayerKey[5].toggleValue);
    preset->insert("Controller_Key_5_Toggle_Return_Value", qn.controllerLayerKey[5].toggleReturnValue);
    preset->insert("Controller_Key_5_Toggle_On_Off", qn.controllerLayerKey[5].toggleEnabled);
    preset->insert("Controller_Key_6_Controller_Layer_Enable_Disable", qn.controllerLayerKey[6].controllerLayerActive);
    preset->insert("Controller_Key_6_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[6].keyboardLayerParticipate);
    preset->insert("Controller_Key_6_Note_Number", qn.controllerLayerKey[6].noteNumber);
    preset->insert("Controller_Key_6_Pressure_Control_Number", qn.controllerLayerKey[6].pressureCCNumber);
    preset->insert("Controller_Key_6_Tilt_Control_Number", qn.controllerLayerKey[6].tiltCCNumber);
    preset->insert("Controller_Key_6_Channel", qn.controllerLayerKey[6].midiChannel);
    preset->insert("Controller_Key_6_Note_Velocity", qn.controllerLayerKey[6].velocityValue);
    preset->insert("Controller_Key_6_Pressure_Return_Value", qn.controllerLayerKey[6].pressureReturnValue);
    preset->insert("Controller_Key_6_Tilt_Return_Value", qn.controllerLayerKey[6].tiltReturnValue);
    preset->insert("Controller_Key_6_Toggle_Control_Number", qn.controllerLayerKey[6].toggleCCNumber);
    preset->insert("Controller_Key_6_Toggle_Value", qn.controllerLayerKey[6].toggleValue);
    preset->insert("Controller_Key_6_Toggle_Return_Value", qn.controllerLayerKey[6].toggleReturnValue);
    preset->insert("Controller_Key_6_Toggle_On_Off", qn.controllerLayerKey[6].toggleEnabled);
    preset->insert("Controller_Key_7_Controller_Layer_Enable_Disable", qn.controllerLayerKey[7].controllerLayerActive);
    preset->insert("Controller_Key_7_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[7].keyboardLayerParticipate);
    preset->insert("Controller_Key_7_Note_Number", qn.controllerLayerKey[7].noteNumber);
    preset->insert("Controller_Key_7_Pressure_Control_Number", qn.controllerLayerKey[7].pressureCCNumber);
    preset->insert("Controller_Key_7_Tilt_Control_Number", qn.controllerLayerKey[7].tiltCCNumber);
    preset->insert("Controller_Key_7_Channel", qn.controllerLayerKey[7].midiChannel);
    preset->insert("Controller_Key_7_Note_Velocity", qn.controllerLayerKey[7].velocityValue);
    preset->insert("Controller_Key_7_Pressure_Return_Value", qn.controllerLayerKey[7].pressureReturnValue);
    preset->insert("Controller_Key_7_Tilt_Return_Value", qn.controllerLayerKey[7].tiltReturnValue);
    preset->insert("Controller_Key_7_Toggle_Control_Number", qn.controllerLayerKey[7].toggleCCNumber);
    preset->insert("Controller_Key_7_Toggle_Value", qn.controllerLayerKey[7].toggleValue);
    preset->insert("Controller_Key_7_Toggle_Return_Value", qn.controllerLayerKey[7].toggleReturnValue);
    preset->insert("Controller_Key_7_Toggle_On_Off", qn.controllerLayerKey[7].toggleEnabled);
    preset->insert("Controller_Key_8_Controller_Layer_Enable_Disable", qn.controllerLayerKey[8].controllerLayerActive);
    preset->insert("Controller_Key_8_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[8].keyboardLayerParticipate);
    preset->insert("Controller_Key_8_Note_Number", qn.controllerLayerKey[8].noteNumber);
    preset->insert("Controller_Key_8_Pressure_Control_Number", qn.controllerLayerKey[8].pressureCCNumber);
    preset->insert("Controller_Key_8_Tilt_Control_Number", qn.controllerLayerKey[8].tiltCCNumber);
    preset->insert("Controller_Key_8_Channel", qn.controllerLayerKey[8].midiChannel);
    preset->insert("Controller_Key_8_Note_Velocity", qn.controllerLayerKey[8].velocityValue);
    preset->insert("Controller_Key_8_Pressure_Return_Value", qn.controllerLayerKey[8].pressureReturnValue);
    preset->insert("Controller_Key_8_Tilt_Return_Value", qn.controllerLayerKey[8].tiltReturnValue);
    preset->insert("Controller_Key_8_Toggle_Control_Number", qn.controllerLayerKey[8].toggleCCNumber);
    preset->insert("Controller_Key_8_Toggle_Value", qn.controllerLayerKey[8].toggleValue);
    preset->insert("Controller_Key_8_Toggle_Return_Value", qn.controllerLayerKey[8].toggleReturnValue);
    preset->insert("Controller_Key_8_Toggle_On_Off", qn.controllerLayerKey[8].toggleEnabled);
    preset->insert("Controller_Key_9_Controller_Layer_Enable_Disable", qn.controllerLayerKey[9].controllerLayerActive);
    preset->insert("Controller_Key_9_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[9].keyboardLayerParticipate);
    preset->insert("Controller_Key_9_Note_Number", qn.controllerLayerKey[9].noteNumber);
    preset->insert("Controller_Key_9_Pressure_Control_Number", qn.controllerLayerKey[9].pressureCCNumber);
    preset->insert("Controller_Key_9_Tilt_Control_Number", qn.controllerLayerKey[9].tiltCCNumber);
    preset->insert("Controller_Key_9_Channel", qn.controllerLayerKey[9].midiChannel);
    preset->insert("Controller_Key_9_Note_Velocity", qn.controllerLayerKey[9].velocityValue);
    preset->insert("Controller_Key_9_Pressure_Return_Value", qn.controllerLayerKey[9].pressureReturnValue);
    preset->insert("Controller_Key_9_Tilt_Return_Value", qn.controllerLayerKey[9].tiltReturnValue);
    preset->insert("Controller_Key_9_Toggle_Control_Number", qn.controllerLayerKey[9].toggleCCNumber);
    preset->insert("Controller_Key_9_Toggle_Value", qn.controllerLayerKey[9].toggleValue);
    preset->insert("Controller_Key_9_Toggle_Return_Value", qn.controllerLayerKey[9].toggleReturnValue);
    preset->insert("Controller_Key_9_Toggle_On_Off", qn.controllerLayerKey[9].toggleEnabled);
    preset->insert("Controller_Key_10_Controller_Layer_Enable_Disable", qn.controllerLayerKey[10].controllerLayerActive);
    preset->insert("Controller_Key_10_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[10].keyboardLayerParticipate);
    preset->insert("Controller_Key_10_Note_Number", qn.controllerLayerKey[10].noteNumber);
    preset->insert("Controller_Key_10_Pressure_Control_Number", qn.controllerLayerKey[10].pressureCCNumber);
    preset->insert("Controller_Key_10_Tilt_Control_Number", qn.controllerLayerKey[10].tiltCCNumber);
    preset->insert("Controller_Key_10_Channel", qn.controllerLayerKey[10].midiChannel);
    preset->insert("Controller_Key_10_Note_Velocity", qn.controllerLayerKey[10].velocityValue);
    preset->insert("Controller_Key_10_Pressure_Return_Value", qn.controllerLayerKey[10].pressureReturnValue);
    preset->insert("Controller_Key_10_Tilt_Return_Value", qn.controllerLayerKey[10].tiltReturnValue);
    preset->insert("Controller_Key_10_Toggle_Control_Number", qn.controllerLayerKey[10].toggleCCNumber);
    preset->insert("Controller_Key_10_Toggle_Value", qn.controllerLayerKey[10].toggleValue);
    preset->insert("Controller_Key_10_Toggle_Return_Value", qn.controllerLayerKey[10].toggleReturnValue);
    preset->insert("Controller_Key_10_Toggle_On_Off", qn.controllerLayerKey[10].toggleEnabled);
    preset->insert("Controller_Key_11_Controller_Layer_Enable_Disable", qn.controllerLayerKey[11].controllerLayerActive);
    preset->insert("Controller_Key_11_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[11].keyboardLayerParticipate);
    preset->insert("Controller_Key_11_Note_Number", qn.controllerLayerKey[11].noteNumber);
    preset->insert("Controller_Key_11_Pressure_Control_Number", qn.controllerLayerKey[11].pressureCCNumber);
    preset->insert("Controller_Key_11_Tilt_Control_Number", qn.controllerLayerKey[11].tiltCCNumber);
    preset->insert("Controller_Key_11_Channel", qn.controllerLayerKey[11].midiChannel);
    preset->insert("Controller_Key_11_Note_Velocity", qn.controllerLayerKey[11].velocityValue);
    preset->insert("Controller_Key_11_Pressure_Return_Value", qn.controllerLayerKey[11].pressureReturnValue);
    preset->insert("Controller_Key_11_Tilt_Return_Value", qn.controllerLayerKey[11].tiltReturnValue);
    preset->insert("Controller_Key_11_Toggle_Control_Number", qn.controllerLayerKey[11].toggleCCNumber);
    preset->insert("Controller_Key_11_Toggle_Value", qn.controllerLayerKey[11].toggleValue);
    preset->insert("Controller_Key_11_Toggle_Return_Value", qn.controllerLayerKey[11].toggleReturnValue);
    preset->insert("Controller_Key_11_Toggle_On_Off", qn.controllerLayerKey[11].toggleEnabled);
    preset->insert("Controller_Key_12_Controller_Layer_Enable_Disable", qn.controllerLayerKey[12].controllerLayerActive);
    preset->insert("Controller_Key_12_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[12].keyboardLayerParticipate);
    preset->insert("Controller_Key_12_Note_Number", qn.controllerLayerKey[12].noteNumber);
    preset->insert("Controller_Key_12_Pressure_Control_Number", qn.controllerLayerKey[12].pressureCCNumber);
    preset->insert("Controller_Key_12_Tilt_Control_Number", qn.controllerLayerKey[12].tiltCCNumber);
    preset->insert("Controller_Key_12_Channel", qn.controllerLayerKey[12].midiChannel);
    preset->insert("Controller_Key_12_Note_Velocity", qn.controllerLayerKey[12].velocityValue);
    preset->insert("Controller_Key_12_Pressure_Return_Value", qn.controllerLayerKey[12].pressureReturnValue);
    preset->insert("Controller_Key_12_Tilt_Return_Value", qn.controllerLayerKey[12].tiltReturnValue);
    preset->insert("Controller_Key_12_Toggle_Control_Number", qn.controllerLayerKey[12].toggleCCNumber);
    preset->insert("Controller_Key_12_Toggle_Value", qn.controllerLayerKey[12].toggleValue);
    preset->insert("Controller_Key_12_Toggle_Return_Value", qn.controllerLayerKey[12].toggleReturnValue);
    preset->insert("Controller_Key_12_Toggle_On_Off", qn.controllerLayerKey[12].toggleEnabled);
    preset->insert("Controller_Key_13_Controller_Layer_Enable_Disable", qn.controllerLayerKey[13].controllerLayerActive);
    preset->insert("Controller_Key_13_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[13].keyboardLayerParticipate);
    preset->insert("Controller_Key_13_Note_Number", qn.controllerLayerKey[13].noteNumber);
    preset->insert("Controller_Key_13_Pressure_Control_Number", qn.controllerLayerKey[13].pressureCCNumber);
    preset->insert("Controller_Key_13_Tilt_Control_Number", qn.controllerLayerKey[13].tiltCCNumber);
    preset->insert("Controller_Key_13_Channel", qn.controllerLayerKey[13].midiChannel);
    preset->insert("Controller_Key_13_Note_Velocity", qn.controllerLayerKey[13].velocityValue);
    preset->insert("Controller_Key_13_Pressure_Return_Value", qn.controllerLayerKey[13].pressureReturnValue);
    preset->insert("Controller_Key_13_Tilt_Return_Value", qn.controllerLayerKey[13].tiltReturnValue);
    preset->insert("Controller_Key_13_Toggle_Control_Number", qn.controllerLayerKey[13].toggleCCNumber);
    preset->insert("Controller_Key_13_Toggle_Value", qn.controllerLayerKey[13].toggleValue);
    preset->insert("Controller_Key_13_Toggle_Return_Value", qn.controllerLayerKey[13].toggleReturnValue);
    preset->insert("Controller_Key_13_Toggle_On_Off", qn.controllerLayerKey[13].toggleEnabled);
    preset->insert("Controller_Key_14_Controller_Layer_Enable_Disable", qn.controllerLayerKey[14].controllerLayerActive);
    preset->insert("Controller_Key_14_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[14].keyboardLayerParticipate);
    preset->insert("Controller_Key_14_Note_Number", qn.controllerLayerKey[14].noteNumber);
    preset->insert("Controller_Key_14_Pressure_Control_Number", qn.controllerLayerKey[14].pressureCCNumber);
    preset->insert("Controller_Key_14_Tilt_Control_Number", qn.controllerLayerKey[14].tiltCCNumber);
    preset->insert("Controller_Key_14_Channel", qn.controllerLayerKey[14].midiChannel);
    preset->insert("Controller_Key_14_Note_Velocity", qn.controllerLayerKey[14].velocityValue);
    preset->insert("Controller_Key_14_Pressure_Return_Value", qn.controllerLayerKey[14].pressureReturnValue);
    preset->insert("Controller_Key_14_Tilt_Return_Value", qn.controllerLayerKey[14].tiltReturnValue);
    preset->insert("Controller_Key_14_Toggle_Control_Number", qn.controllerLayerKey[14].toggleCCNumber);
    preset->insert("Controller_Key_14_Toggle_Value", qn.controllerLayerKey[14].toggleValue);
    preset->insert("Controller_Key_14_Toggle_Return_Value", qn.controllerLayerKey[14].toggleReturnValue);
    preset->insert("Controller_Key_14_Toggle_On_Off", qn.controllerLayerKey[14].toggleEnabled);
    preset->insert("Controller_Key_15_Controller_Layer_Enable_Disable", qn.controllerLayerKey[15].controllerLayerActive);
    preset->insert("Controller_Key_15_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[15].keyboardLayerParticipate);
    preset->insert("Controller_Key_15_Note_Number", qn.controllerLayerKey[15].noteNumber);
    preset->insert("Controller_Key_15_Pressure_Control_Number", qn.controllerLayerKey[15].pressureCCNumber);
    preset->insert("Controller_Key_15_Tilt_Control_Number", qn.controllerLayerKey[15].tiltCCNumber);
    preset->insert("Controller_Key_15_Channel", qn.controllerLayerKey[15].midiChannel);
    preset->insert("Controller_Key_15_Note_Velocity", qn.controllerLayerKey[15].velocityValue);
    preset->insert("Controller_Key_15_Pressure_Return_Value", qn.controllerLayerKey[15].pressureReturnValue);
    preset->insert("Controller_Key_15_Tilt_Return_Value", qn.controllerLayerKey[15].tiltReturnValue);
    preset->insert("Controller_Key_15_Toggle_Control_Number", qn.controllerLayerKey[15].toggleCCNumber);
    preset->insert("Controller_Key_15_Toggle_Value", qn.controllerLayerKey[15].toggleValue);
    preset->insert("Controller_Key_15_Toggle_Return_Value", qn.controllerLayerKey[15].toggleReturnValue);
    preset->insert("Controller_Key_15_Toggle_On_Off", qn.controllerLayerKey[15].toggleEnabled);
    preset->insert("Controller_Key_16_Controller_Layer_Enable_Disable", qn.controllerLayerKey[16].controllerLayerActive);
    preset->insert("Controller_Key_16_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[16].keyboardLayerParticipate);
    preset->insert("Controller_Key_16_Note_Number", qn.controllerLayerKey[16].noteNumber);
    preset->insert("Controller_Key_16_Pressure_Control_Number", qn.controllerLayerKey[16].pressureCCNumber);
    preset->insert("Controller_Key_16_Tilt_Control_Number", qn.controllerLayerKey[16].tiltCCNumber);
    preset->insert("Controller_Key_16_Channel", qn.controllerLayerKey[16].midiChannel);
    preset->insert("Controller_Key_16_Note_Velocity", qn.controllerLayerKey[16].velocityValue);
    preset->insert("Controller_Key_16_Pressure_Return_Value", qn.controllerLayerKey[16].pressureReturnValue);
    preset->insert("Controller_Key_16_Tilt_Return_Value", qn.controllerLayerKey[16].tiltReturnValue);
    preset->insert("Controller_Key_16_Toggle_Control_Number", qn.controllerLayerKey[16].toggleCCNumber);
    preset->insert("Controller_Key_16_Toggle_Value", qn.controllerLayerKey[16].toggleValue);
    preset->insert("Controller_Key_16_Toggle_Return_Value", qn.controllerLayerKey[16].toggleReturnValue);
    preset->insert("Controller_Key_16_Toggle_On_Off", qn.controllerLayerKey[16].toggleEnabled);
    preset->insert("Controller_Key_17_Controller_Layer_Enable_Disable", qn.controllerLayerKey[17].controllerLayerActive);
    preset->insert("Controller_Key_17_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[17].keyboardLayerParticipate);
    preset->insert("Controller_Key_17_Note_Number", qn.controllerLayerKey[17].noteNumber);
    preset->insert("Controller_Key_17_Pressure_Control_Number", qn.controllerLayerKey[17].pressureCCNumber);
    preset->insert("Controller_Key_17_Tilt_Control_Number", qn.controllerLayerKey[17].tiltCCNumber);
    preset->insert("Controller_Key_17_Channel", qn.controllerLayerKey[17].midiChannel);
    preset->insert("Controller_Key_17_Note_Velocity", qn.controllerLayerKey[17].velocityValue);
    preset->insert("Controller_Key_17_Pressure_Return_Value", qn.controllerLayerKey[17].pressureReturnValue);
    preset->insert("Controller_Key_17_Tilt_Return_Value", qn.controllerLayerKey[17].tiltReturnValue);
    preset->insert("Controller_Key_17_Toggle_Control_Number", qn.controllerLayerKey[17].toggleCCNumber);
    preset->insert("Controller_Key_17_Toggle_Value", qn.controllerLayerKey[17].toggleValue);
    preset->insert("Controller_Key_17_Toggle_Return_Value", qn.controllerLayerKey[17].toggleReturnValue);
    preset->insert("Controller_Key_17_Toggle_On_Off", qn.controllerLayerKey[17].toggleEnabled);
    preset->insert("Controller_Key_18_Controller_Layer_Enable_Disable", qn.controllerLayerKey[18].controllerLayerActive);
    preset->insert("Controller_Key_18_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[18].keyboardLayerParticipate);
    preset->insert("Controller_Key_18_Note_Number", qn.controllerLayerKey[18].noteNumber);
    preset->insert("Controller_Key_18_Pressure_Control_Number", qn.controllerLayerKey[18].pressureCCNumber);
    preset->insert("Controller_Key_18_Tilt_Control_Number", qn.controllerLayerKey[18].tiltCCNumber);
    preset->insert("Controller_Key_18_Channel", qn.controllerLayerKey[18].midiChannel);
    preset->insert("Controller_Key_18_Note_Velocity", qn.controllerLayerKey[18].velocityValue);
    preset->insert("Controller_Key_18_Pressure_Return_Value", qn.controllerLayerKey[18].pressureReturnValue);
    preset->insert("Controller_Key_18_Tilt_Return_Value", qn.controllerLayerKey[18].tiltReturnValue);
    preset->insert("Controller_Key_18_Toggle_Control_Number", qn.controllerLayerKey[18].toggleCCNumber);
    preset->insert("Controller_Key_18_Toggle_Value", qn.controllerLayerKey[18].toggleValue);
    preset->insert("Controller_Key_18_Toggle_Return_Value", qn.controllerLayerKey[18].toggleReturnValue);
    preset->insert("Controller_Key_18_Toggle_On_Off", qn.controllerLayerKey[18].toggleEnabled);
    preset->insert("Controller_Key_19_Controller_Layer_Enable_Disable", qn.controllerLayerKey[19].controllerLayerActive);
    preset->insert("Controller_Key_19_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[19].keyboardLayerParticipate);
    preset->insert("Controller_Key_19_Note_Number", qn.controllerLayerKey[19].noteNumber);
    preset->insert("Controller_Key_19_Pressure_Control_Number", qn.controllerLayerKey[19].pressureCCNumber);
    preset->insert("Controller_Key_19_Tilt_Control_Number", qn.controllerLayerKey[19].tiltCCNumber);
    preset->insert("Controller_Key_19_Channel", qn.controllerLayerKey[19].midiChannel);
    preset->insert("Controller_Key_19_Note_Velocity", qn.controllerLayerKey[19].velocityValue);
    preset->insert("Controller_Key_19_Pressure_Return_Value", qn.controllerLayerKey[19].pressureReturnValue);
    preset->insert("Controller_Key_19_Tilt_Return_Value", qn.controllerLayerKey[19].tiltReturnValue);
    preset->insert("Controller_Key_19_Toggle_Control_Number", qn.controllerLayerKey[19].toggleCCNumber);
    preset->insert("Controller_Key_19_Toggle_Value", qn.controllerLayerKey[19].toggleValue);
    preset->insert("Controller_Key_19_Toggle_Return_Value", qn.controllerLayerKey[19].toggleReturnValue);
    preset->insert("Controller_Key_19_Toggle_On_Off", qn.controllerLayerKey[19].toggleEnabled);
    preset->insert("Controller_Key_20_Controller_Layer_Enable_Disable", qn.controllerLayerKey[20].controllerLayerActive);
    preset->insert("Controller_Key_20_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[20].keyboardLayerParticipate);
    preset->insert("Controller_Key_20_Note_Number", qn.controllerLayerKey[20].noteNumber);
    preset->insert("Controller_Key_20_Pressure_Control_Number", qn.controllerLayerKey[20].pressureCCNumber);
    preset->insert("Controller_Key_20_Tilt_Control_Number", qn.controllerLayerKey[20].tiltCCNumber);
    preset->insert("Controller_Key_20_Channel", qn.controllerLayerKey[20].midiChannel);
    preset->insert("Controller_Key_20_Note_Velocity", qn.controllerLayerKey[20].velocityValue);
    preset->insert("Controller_Key_20_Pressure_Return_Value", qn.controllerLayerKey[20].pressureReturnValue);
    preset->insert("Controller_Key_20_Tilt_Return_Value", qn.controllerLayerKey[20].tiltReturnValue);
    preset->insert("Controller_Key_20_Toggle_Control_Number", qn.controllerLayerKey[20].toggleCCNumber);
    preset->insert("Controller_Key_20_Toggle_Value", qn.controllerLayerKey[20].toggleValue);
    preset->insert("Controller_Key_20_Toggle_Return_Value", qn.controllerLayerKey[20].toggleReturnValue);
    preset->insert("Controller_Key_20_Toggle_On_Off", qn.controllerLayerKey[20].toggleEnabled);
    preset->insert("Controller_Key_21_Controller_Layer_Enable_Disable", qn.controllerLayerKey[21].controllerLayerActive);
    preset->insert("Controller_Key_21_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[21].keyboardLayerParticipate);
    preset->insert("Controller_Key_21_Note_Number", qn.controllerLayerKey[21].noteNumber);
    preset->insert("Controller_Key_21_Pressure_Control_Number", qn.controllerLayerKey[21].pressureCCNumber);
    preset->insert("Controller_Key_21_Tilt_Control_Number", qn.controllerLayerKey[21].tiltCCNumber);
    preset->insert("Controller_Key_21_Channel", qn.controllerLayerKey[21].midiChannel);
    preset->insert("Controller_Key_21_Note_Velocity", qn.controllerLayerKey[21].velocityValue);
    preset->insert("Controller_Key_21_Pressure_Return_Value", qn.controllerLayerKey[21].pressureReturnValue);
    preset->insert("Controller_Key_21_Tilt_Return_Value", qn.controllerLayerKey[21].tiltReturnValue);
    preset->insert("Controller_Key_21_Toggle_Control_Number", qn.controllerLayerKey[21].toggleCCNumber);
    preset->insert("Controller_Key_21_Toggle_Value", qn.controllerLayerKey[21].toggleValue);
    preset->insert("Controller_Key_21_Toggle_Return_Value", qn.controllerLayerKey[21].toggleReturnValue);
    preset->insert("Controller_Key_21_Toggle_On_Off", qn.controllerLayerKey[21].toggleEnabled);
    preset->insert("Controller_Key_22_Controller_Layer_Enable_Disable", qn.controllerLayerKey[22].controllerLayerActive);
    preset->insert("Controller_Key_22_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[22].keyboardLayerParticipate);
    preset->insert("Controller_Key_22_Note_Number", qn.controllerLayerKey[22].noteNumber);
    preset->insert("Controller_Key_22_Pressure_Control_Number", qn.controllerLayerKey[22].pressureCCNumber);
    preset->insert("Controller_Key_22_Tilt_Control_Number", qn.controllerLayerKey[22].tiltCCNumber);
    preset->insert("Controller_Key_22_Channel", qn.controllerLayerKey[22].midiChannel);
    preset->insert("Controller_Key_22_Note_Velocity", qn.controllerLayerKey[22].velocityValue);
    preset->insert("Controller_Key_22_Pressure_Return_Value", qn.controllerLayerKey[22].pressureReturnValue);
    preset->insert("Controller_Key_22_Tilt_Return_Value", qn.controllerLayerKey[22].tiltReturnValue);
    preset->insert("Controller_Key_22_Toggle_Control_Number", qn.controllerLayerKey[22].toggleCCNumber);
    preset->insert("Controller_Key_22_Toggle_Value", qn.controllerLayerKey[22].toggleValue);
    preset->insert("Controller_Key_22_Toggle_Return_Value", qn.controllerLayerKey[22].toggleReturnValue);
    preset->insert("Controller_Key_22_Toggle_On_Off", qn.controllerLayerKey[22].toggleEnabled);
    preset->insert("Controller_Key_23_Controller_Layer_Enable_Disable", qn.controllerLayerKey[23].controllerLayerActive);
    preset->insert("Controller_Key_23_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[23].keyboardLayerParticipate);
    preset->insert("Controller_Key_23_Note_Number", qn.controllerLayerKey[23].noteNumber);
    preset->insert("Controller_Key_23_Pressure_Control_Number", qn.controllerLayerKey[23].pressureCCNumber);
    preset->insert("Controller_Key_23_Tilt_Control_Number", qn.controllerLayerKey[23].tiltCCNumber);
    preset->insert("Controller_Key_23_Channel", qn.controllerLayerKey[23].midiChannel);
    preset->insert("Controller_Key_23_Note_Velocity", qn.controllerLayerKey[23].velocityValue);
    preset->insert("Controller_Key_23_Pressure_Return_Value", qn.controllerLayerKey[23].pressureReturnValue);
    preset->insert("Controller_Key_23_Tilt_Return_Value", qn.controllerLayerKey[23].tiltReturnValue);
    preset->insert("Controller_Key_23_Toggle_Control_Number", qn.controllerLayerKey[23].toggleCCNumber);
    preset->insert("Controller_Key_23_Toggle_Value", qn.controllerLayerKey[23].toggleValue);
    preset->insert("Controller_Key_23_Toggle_Return_Value", qn.controllerLayerKey[23].toggleReturnValue);
    preset->insert("Controller_Key_23_Toggle_On_Off", qn.controllerLayerKey[23].toggleEnabled);
    preset->insert("Controller_Key_24_Controller_Layer_Enable_Disable", qn.controllerLayerKey[24].controllerLayerActive);
    preset->insert("Controller_Key_24_Keyboard_Layer_Enable_Disable", qn.controllerLayerKey[24].keyboardLayerParticipate);
    preset->insert("Controller_Key_24_Note_Number", qn.controllerLayerKey[24].noteNumber);
    preset->insert("Controller_Key_24_Pressure_Control_Number", qn.controllerLayerKey[24].pressureCCNumber);
    preset->insert("Controller_Key_24_Tilt_Control_Number", qn.controllerLayerKey[24].tiltCCNumber);
    preset->insert("Controller_Key_24_Channel", qn.controllerLayerKey[24].midiChannel);
    preset->insert("Controller_Key_24_Note_Velocity", qn.controllerLayerKey[24].velocityValue);
    preset->insert("Controller_Key_24_Pressure_Return_Value", qn.controllerLayerKey[24].pressureReturnValue);
    preset->insert("Controller_Key_24_Tilt_Return_Value", qn.controllerLayerKey[24].tiltReturnValue);
    preset->insert("Controller_Key_24_Toggle_Control_Number", qn.controllerLayerKey[24].toggleCCNumber);
    preset->insert("Controller_Key_24_Toggle_Value", qn.controllerLayerKey[24].toggleValue);
    preset->insert("Controller_Key_24_Toggle_Return_Value", qn.controllerLayerKey[24].toggleReturnValue);
    preset->insert("Controller_Key_24_Toggle_On_Off", qn.controllerLayerKey[24].toggleEnabled);
    preset->insert("Controller_Bend_Pad_00_Controller_Layer_Enable_Disable", qn.controllerLayerBendPad.controllerLayerActive);
    preset->insert("Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable", qn.controllerLayerBendPad.keyboardLayerParticipate);
    preset->insert("Controller_Bend_Pad_00_Note_Number", qn.controllerLayerBendPad.noteNumber);
    preset->insert("Controller_Bend_Pad_00_Pressure_Control_Number", qn.controllerLayerBendPad.pressureCCNumber);
    preset->insert("Controller_Bend_Pad_00_Tilt_Control_Number", qn.controllerLayerBendPad.tiltCCNumber);
    preset->insert("Controller_Bend_Pad_00_Channel", qn.controllerLayerBendPad.midiChannel);
    preset->insert("Controller_Bend_Pad_00_Note_Velocity", qn.controllerLayerBendPad.velocityValue);
    preset->insert("Controller_Bend_Pad_00_Pressure_Return_Value", qn.controllerLayerBendPad.pressureReturnValue);
    preset->insert("Controller_Bend_Pad_00_Tilt_Return_Value", qn.controllerLayerBendPad.tiltReturnValue);
    preset->insert("CV_In_CV_1_Channel", qn.cvInput[0].channel);
    preset->insert("CV_In_CV_1_Curve", qn.cvInput[0].modlineConstants.curve);
    preset->insert("CV_In_CV_1_Gain", qn.cvInput[0].modlineConstants.gain);
    preset->insert("CV_In_CV_1_Max", qn.cvInput[0].modlineConstants.max);
    preset->insert("CV_In_CV_1_Min", qn.cvInput[0].modlineConstants.min);
    preset->insert("CV_In_CV_1_Offset", qn.cvInput[0].modlineConstants.offset);
    preset->insert("CV_In_CV_1_MIDI_Output_Device", qn.cvInput[0].midiOutputDevice);
    preset->insert("CV_In_CV_1_CC_Number", qn.cvInput[0].ccNumber);
    preset->insert("CV_In_CV_2_Channel", qn.cvInput[1].channel);
    preset->insert("CV_In_CV_2_Curve", qn.cvInput[1].modlineConstants.curve);
    preset->insert("CV_In_CV_2_Gain", qn.cvInput[1].modlineConstants.gain);
    preset->insert("CV_In_CV_2_Max", qn.cvInput[1].modlineConstants.max);
    preset->insert("CV_In_CV_2_Min", qn.cvInput[1].modlineConstants.min);
    preset->insert("CV_In_CV_2_Offset", qn.cvInput[1].modlineConstants.offset);
    preset->insert("CV_In_CV_2_MIDI_Output_Device", qn.cvInput[1].midiOutputDevice);
    preset->insert("CV_In_CV_2_CC_Number", qn.cvInput[1].ccNumber);
    preset->insert("CV_Out_CV_1_Channel", qn.cvOutput[0].channel);
    preset->insert("CV_Out_CV_1_Curve", qn.cvOutput[0].modlineConstants.curve);
    preset->insert("CV_Out_CV_1_Gain", qn.cvOutput[0].modlineConstants.gain);
    preset->insert("CV_Out_CV_1_Max", qn.cvOutput[0].modlineConstants.max);
    preset->insert("CV_Out_CV_1_Min", qn.cvOutput[0].modlineConstants.min);
    preset->insert("CV_Out_CV_1_Offset", qn.cvOutput[0].modlineConstants.offset);
    preset->insert("CV_Out_CV_1_MIDI_Input_Device", qn.cvOutput[0].midiInputDevice);
    preset->insert("CV_Out_CV_1_MIDI_Input_Type", qn.cvOutput[0].midiInputType);
    preset->insert("CV_Out_CV_2_Channel", qn.cvOutput[1].channel);
    preset->insert("CV_Out_CV_2_Curve", qn.cvOutput[1].modlineConstants.curve);
    preset->insert("CV_Out_CV_2_Gain", qn.cvOutput[1].modlineConstants.gain);
    preset->insert("CV_Out_CV_2_Max", qn.cvOutput[1].modlineConstants.max);
    preset->insert("CV_Out_CV_2_Min", qn.cvOutput[1].modlineConstants.min);
    preset->insert("CV_Out_CV_2_Offset", qn.cvOutput[1].modlineConstants.offset);
    preset->insert("CV_Out_CV_2_MIDI_Input_Device", qn.cvOutput[1].midiInputDevice);
    preset->insert("CV_Out_CV_2_MIDI_Input_Type", qn.cvOutput[1].midiInputType);
    preset->insert("CV_Out_CV_3_Channel", qn.cvOutput[2].channel);
    preset->insert("CV_Out_CV_3_Curve", qn.cvOutput[2].modlineConstants.curve);
    preset->insert("CV_Out_CV_3_Gain", qn.cvOutput[2].modlineConstants.gain);
    preset->insert("CV_Out_CV_3_Max", qn.cvOutput[2].modlineConstants.max);
    preset->insert("CV_Out_CV_3_Min", qn.cvOutput[2].modlineConstants.min);
    preset->insert("CV_Out_CV_3_Offset", qn.cvOutput[2].modlineConstants.offset);
    preset->insert("CV_Out_CV_3_MIDI_Input_Device", qn.cvOutput[2].midiInputDevice);
    preset->insert("CV_Out_CV_3_MIDI_Input_Type", qn.cvOutput[2].midiInputType);
    preset->insert("CV_Out_Gate_MIDI_Input_Device", qn.gateInputDevice);
    preset->insert("CV_Out_Gate_Channel", qn.gateInputChannel);
    preset->insert("CV_Pitch_Scaling_Scheme", qn.cvPitchStandard);
    preset->insert("CV_Gate_Articulation", qn.gateArticulation);
    preset->insert("CV_Gate_S_Trig", qn.gateType);
    preset->insert("PRESET_STRUCT_IS_V2", 36); // magic number
    preset->insert("SEQ_clock_source", qn.seq.clock_source);
    preset->insert("SEQ_clock_dest", qn.seq.clock_dest);
    qDebug() << "orig bpm: " << preset->value("SEQ_tempo").toUInt();
    preset->insert("SEQ_tempo", qn.seq.BPM);
    qDebug() << "bpm: " << qn.seq.BPM;
    preset->insert("SEQ_swing", qn.seq.swingPercent);
    preset->insert("SEQ_KB_split", qn.seq.kbSplit);
    preset->insert("SEQ_KB_lower", qn.seq.kbSplitLower);
    preset->insert("SEQ_KB_upper", qn.seq.kbSplitUpper);
    preset->insert("SEQ_track_1_mode", qn.seq.track[0].mode);                    // Special
    preset->insert("SEQ_track_1_poly", qn.seq.track[0].polyphony);               // Special
    preset->insert("SEQ_track_1_arp_pattern", qn.seq.track[0].patternPlay);      // Special
    preset->insert("SEQ_track_1_seq_pattern", qn.seq.track[0].patternSeq);       // Special
    preset->insert("SEQ_track_1_division", qn.seq.track[0].division);            // Special
    preset->insert("SEQ_track_1_dot", qn.seq.track[0].dotTrip);                  // Special
    preset->insert("SEQ_track_1_gate", qn.seq.track[0].gatePercent);
    preset->insert("SEQ_track_1_chain", qn.seq.track[0].chainX);                 // Special
    preset->insert("SEQ_track_1_chain", qn.seq.track[0].chainY);                 // Special
    preset->insert("SEQ_track_1_chain", qn.seq.track[0].chainZ);                 // Special
    preset->insert("SEQ_track_1_length", qn.seq.track[0].length);
    preset->insert("SEQ_track_1_dest", qn.seq.track[0].dest);                    // Special
    preset->insert("SEQ_track_1_midi_ch", qn.seq.track[0].channel);
    preset->insert("SEQ_track_1_transp", qn.seq.track[0].transpose);
    preset->insert("SEQ_track_2_mode", qn.seq.track[1].mode);                    // Special
    preset->insert("SEQ_track_2_poly", qn.seq.track[1].polyphony);               // Special
    preset->insert("SEQ_track_2_arp_pattern", qn.seq.track[1].patternPlay);      // Special
    preset->insert("SEQ_track_2_seq_pattern", qn.seq.track[1].patternSeq);       // Special
    preset->insert("SEQ_track_2_division", qn.seq.track[1].division);            // Special
    preset->insert("SEQ_track_2_dot", qn.seq.track[1].dotTrip);                  // Special
    preset->insert("SEQ_track_2_gate", qn.seq.track[1].gatePercent);
    preset->insert("SEQ_track_2_chain", qn.seq.track[1].chainX);                 // Special
    preset->insert("SEQ_track_2_chain", qn.seq.track[1].chainY);                 // Special
    preset->insert("SEQ_track_2_chain", qn.seq.track[1].chainZ);                 // Special
    preset->insert("SEQ_track_2_length", qn.seq.track[1].length);
    preset->insert("SEQ_track_2_dest", qn.seq.track[1].dest);                    // Special
    preset->insert("SEQ_track_2_midi_ch", qn.seq.track[1].channel);
    preset->insert("SEQ_track_2_transp", qn.seq.track[1].transpose);
    preset->insert("SEQ_track_3_mode", qn.seq.track[2].mode);                    // Special
    preset->insert("SEQ_track_3_poly", qn.seq.track[2].polyphony);               // Special
    preset->insert("SEQ_track_3_arp_pattern", qn.seq.track[2].patternPlay);      // Special
    preset->insert("SEQ_track_3_seq_pattern", qn.seq.track[2].patternSeq);       // Special
    preset->insert("SEQ_track_3_division", qn.seq.track[2].division);            // Special
    preset->insert("SEQ_track_3_dot", qn.seq.track[2].dotTrip);                  // Special
    preset->insert("SEQ_track_3_gate", qn.seq.track[2].gatePercent);
    preset->insert("SEQ_track_3_chain", qn.seq.track[2].chainX);                 // Special
    preset->insert("SEQ_track_3_chain", qn.seq.track[2].chainY);                 // Special
    preset->insert("SEQ_track_3_chain", qn.seq.track[2].chainZ);                 // Special
    preset->insert("SEQ_track_3_length", qn.seq.track[2].length);
    preset->insert("SEQ_track_3_dest", qn.seq.track[2].dest);                    // Special
    preset->insert("SEQ_track_3_midi_ch", qn.seq.track[2].channel);
    preset->insert("SEQ_track_3_transp", qn.seq.track[2].transpose);
}
