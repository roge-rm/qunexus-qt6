// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
/*
    -------------------------------------------------------------------
    -------------------------------------------------------------------

    Written by Conner Lacy - www.connerlacy.com | conner@connerlacy.com

    Copyright (c) 2013 Keith McMillen Instruments

    Description:
        - Updates preset parameter changes from UI
        - Manages librarian preset saving, deleting, renaming, etc.
        - Outputs edit buffers for syncing to device
        - Interfaces with ui, mainwindow.cpp and sysexencode.cpp

    -------------------------------------------------------------------
    -------------------------------------------------------------------
*/

#include "presetmanager.h"

///////////////////////////////////////////////////////////////
///////////// Initialization & Control Fucntions //////////////
///////////////////////////////////////////////////////////////

//-------- ******Constructor****** -------//
PresetManager::PresetManager(QWidget *parent) :
    QWidget(parent)
{  
    settings = new QSettings();

    //StyleSheets for Preset Library
    selectedStyleSheetFile = new QFile(":stylesheets/resources/stylesheets/LibrarySelectedModifiedStyleSheet.qss");
    selectedStyleSheetFile->open(QFile::ReadOnly);
    selectedStyleSheetString = QLatin1String(selectedStyleSheetFile->readAll());

    unselectedStyleSheetFile = new QFile(":stylesheets/resources/stylesheets/LibrarySelectedCleanStyleSheet.qss");
    unselectedStyleSheetFile->open(QFile::ReadOnly);
    unselectedStyleSheetString = QLatin1String(unselectedStyleSheetFile->readAll());

    //StyleSheets for Preset Library
    blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    blueStyleString = QLatin1String(blueStyleFile->readAll());

    //StyleSheets for Preset Library
    grayStyleFile = new QFile(":stylesheets/resources/stylesheets/GrayButtonStyleSheet.qss");
    grayStyleFile->open(QFile::ReadOnly);
    grayStyleString = QLatin1String(grayStyleFile->readAll());


    slotCreateDefaultParamMap();
    slotLoadJSON();

    //-------------------------------------------------------- Initialize Selected Preset on App. Load --------------------------------------------------------//
    if(settings->contains("selectedPreset") && (settings->value("selectedPreset").toInt() < jsonMasterMapEDITABLE.size()) && (settings->value("selectedPreset").toInt() > -1))
    {
        selectedLibraryPreset = settings->value("selectedPreset").toInt();
        highlightedLibraryPreset = selectedLibraryPreset;

        //Get preset name highlighted index
        QString tempPresetName = slotGetPresetNameFromIndex(selectedLibraryPreset);

        //Load preset into edit buffer
        qDebug() << "Load preset into edit buffer";
        editBufferPresetMap = jsonMasterMapEDITABLE.value(tempPresetName).toMap();
//        qDebug() << "editBufferPresetMap" << editBufferPresetMap;
    }
    else if(jsonMasterMapEDITABLE.size())
    {
        selectedLibraryPreset = 0;
        highlightedLibraryPreset = -1;
        settings->setValue("selectedPreset", selectedLibraryPreset);

        //Initialize Edit Buffer
        editBufferPresetMap = defaultParamMap;
    }
    else
    {
        selectedLibraryPreset = -1;
        highlightedLibraryPreset = -1;
        settings->setValue("selectedPreset", selectedLibraryPreset);

        //Initialize Edit Buffer
        editBufferPresetMap = defaultParamMap;
    }

    // --- loading flag for edit buffer sync
    presetLoading = false;
    JSONLoading = false;
    syncFlag = false;

    //-------------------------------------------------------- Initialize Controller Key Selections -----------------------------------------------------------//
    shiftDown = false;
    newControllerKeySelected = false;
    controllerLayerSelectedComponents.append("Key_Highlight_0");


    //----------------------------------------------------------------- Signal GUI To Update ------------------------------------------------------------------//
//    slotLoadLibraryPresetIntoEditBuffer(0);

    //emit signalUpdateParameterUi(editBufferPresetMap);
    //emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
    //emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_SELECTED, 0);
    //emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_HIGHLIGHT, 0);
    //emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_MODIFIED, 0);

    qDebug() << "emit signalLibraryReady";
    emit signalLibraryReady();
}

//-------- Populates Default Map, used to check JSON presets against --------//
void PresetManager::slotCreateDefaultParamMap()
{

    //----------------------------------------------------------------------------------------//
    //----------------------------------Default Param List------------------------------------//
    //----------------------------------------------------------------------------------------//


    defaultParamMap["Preset_Name"] = "Default Preset";

    //*********** Globals  **********//
    //**********************************//
    defaultParamMap["Globals_Gain"] = 100;
    defaultParamMap["Globals_Off_Thresh"] = 5;
    defaultParamMap["Globals_On_Thresh"] = 10;

    defaultParamMap["Globals_Tilt_Sensitivity"] = 40; // Tilt Activation Sensitivity
    defaultParamMap["Globals_LED_Refresh_Style"] = "Normal";


    //*********** Keyboard Map **********//
    //**********************************//
    defaultParamMap["Keyboard"] = 1;
    defaultParamMap["Keyboard_Global_USB_1_Channel"] =  0;
    defaultParamMap["Keyboard_Global_USB_2_Channel"] = 0;
    defaultParamMap["Keyboard_Global_Mode"] =  "Off";
    defaultParamMap["Keyboard_Global_Channel_Rotation_Active"] = 0;
    defaultParamMap["Keyboard_Global_Key_Selection_Criteria"] = "Latest";
    defaultParamMap["Keyboard_Global_Velocity_Active"] = 1;
    defaultParamMap["Keyboard_Global_Pressure_Active"] = 1;
    defaultParamMap["Keyboard_Global_Tilt_Active"] = 1;

    defaultParamMap["Keyboard_Global_Pressure_Sensitivity"] = 1;
    defaultParamMap["Keyboard_Global_Bend_Pad_Sensitivity"] = 1;
    defaultParamMap["Keyboard_Global_Pitch_Bend_Range"] = 1;
    defaultParamMap["Keyboard_Global_Poly_Aftertouch_Active"] = 1;
    defaultParamMap["Keyboard_Global_CC_Active"] = 1;
    defaultParamMap["Keyboard_Global_Transpose"] = 0;
    defaultParamMap["Keyboard_Global_Channel_Rotation_Offset"] = 0;
    defaultParamMap["Keyboard_Global_LED_Mode"] = 1;
    defaultParamMap["Keyboard_Global_LED_Remote_Channel"] = 0;
    defaultParamMap["Keyboard_Global_Polyphony_Number"] = 10;

    defaultParamMap["Keyboard_Global_Program_Change_A"] = -1;
    defaultParamMap["Keyboard_Global_Program_Change_B"] = -1;
    defaultParamMap["Keyboard_Global_Program_Change_C"] = -1;
    defaultParamMap["Keyboard_Global_Program_Change_D"] = -1;

    defaultParamMap["Keyboard_Velocity_Gain"] = 100;
    defaultParamMap["Keyboard_Velocity_Offset"] = 0;
    defaultParamMap["Keyboard_Velocity_Curve"] =  "Linear";
    defaultParamMap["Keyboard_Velocity_Min"] = 0;
    defaultParamMap["Keyboard_Velocity_Max"] = 127;
    defaultParamMap["Keyboard_Velocity_MIDI_Output_Device"] =   "USB 1";

    defaultParamMap["Keyboard_Pitch_Bend_Source"] =  "None";
    defaultParamMap["Keyboard_Pitch_Bend_Gain"] = 100;
    defaultParamMap["Keyboard_Pitch_Bend_Offset"] = 0;
    defaultParamMap["Keyboard_Pitch_Bend_Min"] =  0;
    defaultParamMap["Keyboard_Pitch_Bend_Max"] =  127;
    defaultParamMap["Keyboard_Pitch_Bend_Curve"] =  "Linear";
    defaultParamMap["Keyboard_Pitch_Bend_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_Channel_Pressure_Source"] =  "None";
    defaultParamMap["Keyboard_Channel_Pressure_Gain"] = 100;
    defaultParamMap["Keyboard_Channel_Pressure_Offset"] = 0;
    defaultParamMap["Keyboard_Channel_Pressure_Min"] =  0;
    defaultParamMap["Keyboard_Channel_Pressure_Max"] =  127;
    defaultParamMap["Keyboard_Channel_Pressure_Curve"] =  "Linear";
    defaultParamMap["Keyboard_Channel_Pressure_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_Poly_Aftertouch_Source"] =  "None";
    defaultParamMap["Keyboard_Poly_Aftertouch_Gain"] = 100;
    defaultParamMap["Keyboard_Poly_Aftertouch_Offset"] = 0;
    defaultParamMap["Keyboard_Poly_Aftertouch_Min" ] =  0;
    defaultParamMap["Keyboard_Poly_Aftertouch_Max"] =  127;
    defaultParamMap["Keyboard_Poly_Aftertouch_Curve"] =  "Linear";
    defaultParamMap["Keyboard_Poly_Aftertouch_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_CC_00_Control_Number"] =  1;
    defaultParamMap["Keyboard_CC_00_Source"] =  "None";
    defaultParamMap["Keyboard_CC_00_Gain"] = 100;
    defaultParamMap["Keyboard_CC_00_Offset"] = 0;
    defaultParamMap["Keyboard_CC_00_Min"] =  0;
    defaultParamMap["Keyboard_CC_00_Max"] =  127;
    defaultParamMap["Keyboard_CC_00_Curve"] =  "Linear";
    defaultParamMap["Keyboard_CC_00_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_CC_01_Control_Number"] =  1;
    defaultParamMap["Keyboard_CC_01_Source"] =  "None";
    defaultParamMap["Keyboard_CC_01_Gain"] = 100;
    defaultParamMap["Keyboard_CC_01_Offset"] = 0;
    defaultParamMap["Keyboard_CC_01_Min"] =  0;
    defaultParamMap["Keyboard_CC_01_Max"] =  127;
    defaultParamMap["Keyboard_CC_01_Curve"] =  "Linear";
    defaultParamMap["Keyboard_CC_01_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_CC_02_Control_Number"] =  1;
    defaultParamMap["Keyboard_CC_02_Source"] =  "None";
    defaultParamMap["Keyboard_CC_02_Gain"] = 100;
    defaultParamMap["Keyboard_CC_02_Offset"] = 0;
    defaultParamMap["Keyboard_CC_02_Min"] =  0;
    defaultParamMap["Keyboard_CC_02_Max"] =  127;
    defaultParamMap["Keyboard_CC_02_Curve"] =  "Linear";
    defaultParamMap["Keyboard_CC_02_MIDI_Output_Device"] =  "USB 1";

    defaultParamMap["Keyboard_CC_03_Control_Number"] =  1;
    defaultParamMap["Keyboard_CC_03_Source"] =  "None";
    defaultParamMap["Keyboard_CC_03_Gain"] = 100;
    defaultParamMap["Keyboard_CC_03_Offset"] = 0;
    defaultParamMap["Keyboard_CC_03_Min"] =  0;
    defaultParamMap["Keyboard_CC_03_Max"] =  127;
    defaultParamMap["Keyboard_CC_03_Curve"] =  "Linear";
    defaultParamMap["Keyboard_CC_03_MIDI_Output_Device"] =  "USB 1";


    //******Controller Layer*********//
    //*******************************//
    defaultParamMap["Controller_Global_MIDI_Output_Device"] = "USB 1";

    defaultParamMap["Controller_Key_0_Channel"] = 0;
    defaultParamMap["Controller_Key_0_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_0_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_0_Mode_Pressure_Note"] = "Normal";  //Not included in sysex yet
    defaultParamMap["Controller_Key_0_Note_Number"] = 60;
    defaultParamMap["Controller_Key_0_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_0_Pressure_Control_Number"] = 60;
    defaultParamMap["Controller_Key_0_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_0_Tilt_Control_Number"] = 61;
    defaultParamMap["Controller_Key_0_Tilt_Return_Value"] = 63;         //Not included in sysex yet
    defaultParamMap["Controller_Key_0_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_0_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_0_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_0_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_1_Channel"] = 0;
    defaultParamMap["Controller_Key_1_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_1_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_1_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_1_Note_Number"] = 61;
    defaultParamMap["Controller_Key_1_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_1_Pressure_Control_Number"] = 62;
    defaultParamMap["Controller_Key_1_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_1_Tilt_Control_Number"] = 63;
    defaultParamMap["Controller_Key_1_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_1_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_1_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_1_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_1_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_2_Channel"] = 0;
    defaultParamMap["Controller_Key_2_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_2_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_2_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_2_Note_Number"] = 62;
    defaultParamMap["Controller_Key_2_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_2_Pressure_Control_Number"] = 64;
    defaultParamMap["Controller_Key_2_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_2_Tilt_Control_Number"] = 65;
    defaultParamMap["Controller_Key_2_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_2_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_2_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_2_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_2_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_3_Channel"] = 0;
    defaultParamMap["Controller_Key_3_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_3_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_3_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_3_Note_Number"] = 63;
    defaultParamMap["Controller_Key_3_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_3_Pressure_Control_Number"] = 66;
    defaultParamMap["Controller_Key_3_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_3_Tilt_Control_Number"] = 67;
    defaultParamMap["Controller_Key_3_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_3_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_3_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_3_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_3_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_4_Channel"] = 0;
    defaultParamMap["Controller_Key_4_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_4_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_4_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_4_Note_Number"] = 64;
    defaultParamMap["Controller_Key_4_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_4_Pressure_Control_Number"] = 68;
    defaultParamMap["Controller_Key_4_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_4_Tilt_Control_Number"] = 69;
    defaultParamMap["Controller_Key_4_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_4_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_4_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_4_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_4_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_5_Channel"] = 0;
    defaultParamMap["Controller_Key_5_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_5_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_5_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_5_Note_Number"] = 65;
    defaultParamMap["Controller_Key_5_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_5_Pressure_Control_Number"] = 70;
    defaultParamMap["Controller_Key_5_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_5_Tilt_Control_Number"] = 71;
    defaultParamMap["Controller_Key_5_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_5_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_5_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_5_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_5_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_6_Channel"] = 0;
    defaultParamMap["Controller_Key_6_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_6_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_6_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_6_Note_Number"] = 66;
    defaultParamMap["Controller_Key_6_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_6_Pressure_Control_Number"] = 72;
    defaultParamMap["Controller_Key_6_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_6_Tilt_Control_Number"] = 73;
    defaultParamMap["Controller_Key_6_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_6_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_6_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_6_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_6_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_7_Channel"] = 0;
    defaultParamMap["Controller_Key_7_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_7_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_7_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_7_Note_Number"] = 67;
    defaultParamMap["Controller_Key_7_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_7_Pressure_Control_Number"] = 74;
    defaultParamMap["Controller_Key_7_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_7_Tilt_Control_Number"] = 75;
    defaultParamMap["Controller_Key_7_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_7_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_7_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_7_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_7_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_8_Channel"] = 0;
    defaultParamMap["Controller_Key_8_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_8_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_8_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_8_Note_Number"] = 68;
    defaultParamMap["Controller_Key_8_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_8_Pressure_Control_Number"] = 76;
    defaultParamMap["Controller_Key_8_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_8_Tilt_Control_Number"] = 77;
    defaultParamMap["Controller_Key_8_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_8_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_8_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_8_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_8_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_9_Channel"] = 0;
    defaultParamMap["Controller_Key_9_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_9_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_9_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_9_Note_Number"] = 69;
    defaultParamMap["Controller_Key_9_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_9_Pressure_Control_Number"] = 78;
    defaultParamMap["Controller_Key_9_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_9_Tilt_Control_Number"] = 79;
    defaultParamMap["Controller_Key_9_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_9_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_9_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_9_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_9_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_10_Channel"] = 0;
    defaultParamMap["Controller_Key_10_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_10_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_10_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_10_Note_Number"] = 70;
    defaultParamMap["Controller_Key_10_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_10_Pressure_Control_Number"] = 80;
    defaultParamMap["Controller_Key_10_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_10_Tilt_Control_Number"] = 81;
    defaultParamMap["Controller_Key_10_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_10_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_10_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_10_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_10_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_11_Channel"] = 0;
    defaultParamMap["Controller_Key_11_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_11_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_11_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_11_Note_Number"] = 71;
    defaultParamMap["Controller_Key_11_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_11_Pressure_Control_Number"] = 82;
    defaultParamMap["Controller_Key_11_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_11_Tilt_Control_Number"] = 83;
    defaultParamMap["Controller_Key_11_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_11_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_11_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_11_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_11_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_12_Channel"] = 0;
    defaultParamMap["Controller_Key_12_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_12_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_12_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_12_Note_Number"] = 72;
    defaultParamMap["Controller_Key_12_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_12_Pressure_Control_Number"] = 84;
    defaultParamMap["Controller_Key_12_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_12_Tilt_Control_Number"] = 85;
    defaultParamMap["Controller_Key_12_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_12_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_12_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_12_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_12_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_13_Channel"] = 0;
    defaultParamMap["Controller_Key_13_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_13_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_13_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_13_Note_Number"] = 73;
    defaultParamMap["Controller_Key_13_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_13_Pressure_Control_Number"] = 86;
    defaultParamMap["Controller_Key_13_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_13_Tilt_Control_Number"] = 87;
    defaultParamMap["Controller_Key_13_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_13_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_13_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_13_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_13_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_14_Channel"] = 0;
    defaultParamMap["Controller_Key_14_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_14_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_14_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_14_Note_Number"] = 74;
    defaultParamMap["Controller_Key_14_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_14_Pressure_Control_Number"] = 88;
    defaultParamMap["Controller_Key_14_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_14_Tilt_Control_Number"] = 89;
    defaultParamMap["Controller_Key_14_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_14_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_14_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_14_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_14_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_15_Channel"] = 0;
    defaultParamMap["Controller_Key_15_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_15_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_15_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_15_Note_Number"] = 75;
    defaultParamMap["Controller_Key_15_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_15_Pressure_Control_Number"] = 90;
    defaultParamMap["Controller_Key_15_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_15_Tilt_Control_Number"] = 91;
    defaultParamMap["Controller_Key_15_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_15_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_15_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_15_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_15_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_16_Channel"] = 0;
    defaultParamMap["Controller_Key_16_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_16_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_16_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_16_Note_Number"] = 76;
    defaultParamMap["Controller_Key_16_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_16_Pressure_Control_Number"] = 92;
    defaultParamMap["Controller_Key_16_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_16_Tilt_Control_Number"] = 93;
    defaultParamMap["Controller_Key_16_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_16_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_16_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_16_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_16_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_17_Channel"] = 0;
    defaultParamMap["Controller_Key_17_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_17_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_17_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_17_Note_Number"] = 77;
    defaultParamMap["Controller_Key_17_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_17_Pressure_Control_Number"] = 94;
    defaultParamMap["Controller_Key_17_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_17_Tilt_Control_Number"] = 95;
    defaultParamMap["Controller_Key_17_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_17_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_17_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_17_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_17_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_18_Channel"] = 0;
    defaultParamMap["Controller_Key_18_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_18_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_18_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_18_Note_Number"] = 78;
    defaultParamMap["Controller_Key_18_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_18_Pressure_Control_Number"] = 96;
    defaultParamMap["Controller_Key_18_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_18_Tilt_Control_Number"] = 97;
    defaultParamMap["Controller_Key_18_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_18_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_18_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_18_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_18_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_19_Channel"] = 0;
    defaultParamMap["Controller_Key_19_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_19_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_19_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_19_Note_Number"] = 79;
    defaultParamMap["Controller_Key_19_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_19_Pressure_Control_Number"] = 98;
    defaultParamMap["Controller_Key_19_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_19_Tilt_Control_Number"] = 99;
    defaultParamMap["Controller_Key_19_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_19_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_19_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_19_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_19_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_20_Channel"] = 0;
    defaultParamMap["Controller_Key_20_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_20_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_20_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_20_Note_Number"] = 80;
    defaultParamMap["Controller_Key_20_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_20_Pressure_Control_Number"] = 100;
    defaultParamMap["Controller_Key_20_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_20_Tilt_Control_Number"] = 101;
    defaultParamMap["Controller_Key_20_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_20_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_20_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_20_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_20_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_21_Channel"] = 0;
    defaultParamMap["Controller_Key_21_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_21_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_21_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_21_Note_Number"] = 81;
    defaultParamMap["Controller_Key_21_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_21_Pressure_Control_Number"] = 102;
    defaultParamMap["Controller_Key_21_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_21_Tilt_Control_Number"] = 103;
    defaultParamMap["Controller_Key_21_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_21_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_21_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_21_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_21_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_22_Channel"] = 0;
    defaultParamMap["Controller_Key_22_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_22_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_22_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_22_Note_Number"] = 82;
    defaultParamMap["Controller_Key_22_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_22_Pressure_Control_Number"] = 104;
    defaultParamMap["Controller_Key_22_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_22_Tilt_Control_Number"] = 105;
    defaultParamMap["Controller_Key_22_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_22_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_22_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_22_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_22_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_23_Channel"] = 0;
    defaultParamMap["Controller_Key_23_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_23_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_23_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_23_Note_Number"] = 83;
    defaultParamMap["Controller_Key_23_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_23_Pressure_Control_Number"] = 106;
    defaultParamMap["Controller_Key_23_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_23_Tilt_Control_Number"] = 107;
    defaultParamMap["Controller_Key_23_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_23_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_23_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_23_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_23_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Key_24_Channel"] = 0;
    defaultParamMap["Controller_Key_24_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Key_24_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Key_24_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Key_24_Note_Number"] = 84;
    defaultParamMap["Controller_Key_24_Note_Velocity"] = -1;
    defaultParamMap["Controller_Key_24_Pressure_Control_Number"] = 108;
    defaultParamMap["Controller_Key_24_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Key_24_Tilt_Control_Number"] = 109;
    defaultParamMap["Controller_Key_24_Tilt_Return_Value"] = 63;
    defaultParamMap["Controller_Key_24_Toggle_Control_Number"] = 62;
    defaultParamMap["Controller_Key_24_Toggle_Value"] = 127;
    defaultParamMap["Controller_Key_24_Toggle_Return_Value"] = 127;
    defaultParamMap["Controller_Key_24_Toggle_On_Off"] = 0;

    defaultParamMap["Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable"] = 1;
    defaultParamMap["Controller_Bend_Pad_00_Controller_Layer_Enable_Disable"] = 0;
    defaultParamMap["Controller_Bend_Pad_00_Channel"] = 0;
    defaultParamMap["Controller_Bend_Pad_00_Mode_Pressure_Note"] = "Normal";
    defaultParamMap["Controller_Bend_Pad_00_Note_Number"] = 127;
    defaultParamMap["Controller_Bend_Pad_00_Note_Velocity"] = -1;
    defaultParamMap["Controller_Bend_Pad_00_Pressure_Control_Number"] = 110;
    defaultParamMap["Controller_Bend_Pad_00_Pressure_Return_Value"] = 0;
    defaultParamMap["Controller_Bend_Pad_00_Tilt_Control_Number"] = 111;
    defaultParamMap["Controller_Bend_Pad_00_Tilt_Return_Value"] = 63;

    //*********** CV Map ************//
    //*******************************//
    defaultParamMap["CV_Out_Gate_MIDI_Input_Device"] =  "All";
    defaultParamMap["CV_Out_Gate_Channel"] = 0;

    defaultParamMap["CV_Out_CV_1_MIDI_Input_Device"] =  "All";
    defaultParamMap["CV_Out_CV_1_Channel"] =  0;
    defaultParamMap["CV_Out_CV_1_MIDI_Input_Type"] =  "Pitch CV";
    defaultParamMap["CV_Out_CV_1_Gain"] = 100;
    defaultParamMap["CV_Out_CV_1_Offset"] = 0;
    defaultParamMap["CV_Out_CV_1_Min"] =  0;
    defaultParamMap["CV_Out_CV_1_Max"] =  127;
    defaultParamMap["CV_Out_CV_1_Curve"] =  "Linear";

    defaultParamMap["CV_Out_CV_2_MIDI_Input_Device"] =  "All";
    defaultParamMap["CV_Out_CV_2_Channel"] =  0;
    defaultParamMap["CV_Out_CV_2_MIDI_Input_Type"] =  "Pitch CV";
    defaultParamMap["CV_Out_CV_2_Gain"] = 100;
    defaultParamMap["CV_Out_CV_2_Offset"] = 0;
    defaultParamMap["CV_Out_CV_2_Min"] =  0;
    defaultParamMap["CV_Out_CV_2_Max"] =  127;
    defaultParamMap["CV_Out_CV_2_Curve"] =  "Linear";

    defaultParamMap["CV_Out_CV_3_MIDI_Input_Device"] =  "All";
    defaultParamMap["CV_Out_CV_3_Channel"] =  0;
    defaultParamMap["CV_Out_CV_3_MIDI_Input_Type"] =  "Pitch CV";
    defaultParamMap["CV_Out_CV_3_Gain"] = 100;
    defaultParamMap["CV_Out_CV_3_Offset"] = 0;
    defaultParamMap["CV_Out_CV_3_Min"] =  0;
    defaultParamMap["CV_Out_CV_3_Max"] =  127;
    defaultParamMap["CV_Out_CV_3_Curve"] =  "Linear";

    defaultParamMap["CV_In_CV_1_Channel"] =  0;
    defaultParamMap["CV_In_CV_1_CC_Number"] =  126;
    defaultParamMap["CV_In_CV_1_Gain"] = 100;
    defaultParamMap["CV_In_CV_1_Offset"] = 0;
    defaultParamMap["CV_In_CV_1_Min"] =  0;
    defaultParamMap["CV_In_CV_1_Max"] =  127;
    defaultParamMap["CV_In_CV_1_Curve"] =  "Linear";
    defaultParamMap["CV_In_CV_1_MIDI_Output_Device"] =  "USB 3";

    defaultParamMap["CV_In_CV_2_Channel"] =  0;
    defaultParamMap["CV_In_CV_2_CC_Number"] =  127;
    defaultParamMap["CV_In_CV_2_Gain"] = 100;
    defaultParamMap["CV_In_CV_2_Offset"] = 0;
    defaultParamMap["CV_In_CV_2_Min"] =  0;
    defaultParamMap["CV_In_CV_2_Max"] =  127;
    defaultParamMap["CV_In_CV_2_Curve"] =  "Linear";
    defaultParamMap["CV_In_CV_2_MIDI_Output_Device"] =  "USB 3";

    defaultParamMap["CV_Pitch_Scaling_Scheme"] = "1 Volt/Octave";
    defaultParamMap["CV_Gate_Articulation"] = 1;
    defaultParamMap["CV_Gate_S_Trig"] = 0;

    // new 2.0 params - mirrors Qn_preset_struct.h in firmware
    defaultParamMap["Preset_Struct_is_V2"] = 36; // structValidV2 - magic number to verify that this is a 2.0 preset

    defaultParamMap["SEQ_clock_source"] = "Auto";        // 4 bits are the sources (int/usb/exp/gate/CV)
    defaultParamMap["SEQ_clock_dest"] = "USB 1 + Expander";            // 4 bits are the destinations (usb/exp)
    defaultParamMap["SEQ_tempo"] = 126;
    defaultParamMap["SEQ_swing"] = 50;
    defaultParamMap["SEQ_KB_split"] = 0;            // keyboard can be split between two tracks
    defaultParamMap["SEQ_KB_lower"] = 0;			// which track is assigned to the lower half of the split
    defaultParamMap["SEQ_KB_upper"] = 1;			// which track is assigned to the upper half of the split

    // ************* track 1 ******************************************************************************

    defaultParamMap["SEQ_track_1_mode"] = "Step Sequencer";     // left 4 bits mode (off/arp/step) (2 = step sequencer, stopped)
    defaultParamMap["SEQ_track_1_poly"] = "Mono";               // right 4 polyphony (0 disabled, 1-4)
    defaultParamMap["SEQ_track_1_arp_pattern"] = "Order";		// left 4 bits arp play patterns, 5 = order
    defaultParamMap["SEQ_track_1_seq_pattern"] = "Empty";		// right 4 bits step pattern pointer for step sequencer
    defaultParamMap["SEQ_track_1_division"] = "8th";            // left 4 bits division (1/1, 1/2, 1/4, 1/8, 1/16, 1/32),  (3 = 8th)
    defaultParamMap["SEQ_track_1_dot"] = "No Dot";				// right 4 doted or triplets
    defaultParamMap["SEQ_track_1_gate"] = 50;                   // whole percentage for gate without decimal (50 = .5)
    defaultParamMap["SEQ_track_1_chain"] = "ABCD";              // CHAIN struct has (8) 3 bit links. 41/225/156 is ABCD0000.
    defaultParamMap["SEQ_track_1_length"] = 16;                 // length of the arp/seq, 0 = empty
    defaultParamMap["SEQ_track_1_dest"] = "USB 1 + Expander";   // left 2 bits destination (2 = USB & Expander),
    defaultParamMap["SEQ_track_1_midi_ch"] = 0;                 // right 6 bits MIDI channel (17 = MPE)
    defaultParamMap["SEQ_track_1_transp"] = 0;                  // per track transposition

    // ************* track 2 ******************************************************************************

    defaultParamMap["SEQ_track_2_mode"] = "Step Sequencer";     // left 4 bits mode (off/arp/step) (2 = step sequencer, stopped)
    defaultParamMap["SEQ_track_2_poly"] = "Mono";               // right 4 polyphony (0 disabled, 1-4)
    defaultParamMap["SEQ_track_2_arp_pattern"] = "Order";		// left 4 bits arp play patterns, 5 = order
    defaultParamMap["SEQ_track_2_seq_pattern"] = "Empty";		// right 4 bits step pattern pointer for step sequencer
    defaultParamMap["SEQ_track_2_division"] = "8th";            // left 4 bits division (1/1, 1/2, 1/4, 1/8, 1/16, 1/32),  (3 = 8th)
    defaultParamMap["SEQ_track_2_dot"] = "No Dot";				// right 4 doted or triplets
    defaultParamMap["SEQ_track_2_gate"] = 50;                   // whole percentage for gate without decimal (50 = .5)
    defaultParamMap["SEQ_track_2_chain"] = "ABCD";              // CHAIN struct has (8) 3 bit links. 41/225/156 is ABCD0000.
    defaultParamMap["SEQ_track_2_length"] = 16;                 // length of the arp/seq, 0 = empty
    defaultParamMap["SEQ_track_2_dest"] = "USB 1 + Expander";   // left 2 bits destination (2 = USB & Expander),
    defaultParamMap["SEQ_track_2_midi_ch"] = 1;                 // right 6 bits MIDI channel (17 = MPE)
    defaultParamMap["SEQ_track_2_transp"] = 0;                  // per track transposition

    // ************* track 3 ******************************************************************************

    defaultParamMap["SEQ_track_3_mode"] = "Step Sequencer";     // left 4 bits mode (off/arp/step) (2 = step sequencer, stopped)
    defaultParamMap["SEQ_track_3_poly"] = "Mono";               // right 4 polyphony (0 disabled, 1-4)
    defaultParamMap["SEQ_track_3_arp_pattern"] = "Order";		// left 4 bits arp play patterns, 5 = order
    defaultParamMap["SEQ_track_3_seq_pattern"] = "Empty";		// right 4 bits step pattern pointer for step sequencer
    defaultParamMap["SEQ_track_3_division"] = "8th";            // left 4 bits division (1/1, 1/2, 1/4, 1/8, 1/16, 1/32),  (3 = 8th)
    defaultParamMap["SEQ_track_3_dot"] = "No Dot";				// right 4 doted or triplets
    defaultParamMap["SEQ_track_3_gate"] = 50;                   // whole percentage for gate without decimal (50 = .5)
    defaultParamMap["SEQ_track_3_chain"] = "ABCD";              // CHAIN struct has (8) 3 bit links. 41/225/156 is ABCD0000.
    defaultParamMap["SEQ_track_3_length"] = 16;                 // length of the arp/seq, 0 = empty
    defaultParamMap["SEQ_track_3_dest"] = "USB 1 + Expander";   // left 2 bits destination (2 = USB & Expander),
    defaultParamMap["SEQ_track_3_midi_ch"] = 2;                 // right 6 bits MIDI channel (17 = MPE)
    defaultParamMap["SEQ_track_3_transp"] = 0;                  // per track transposition

    //------------------------------------------------------------------//
    //--------Populate List with Params for Special Processing----------//
    //------------------------------------------------------------------//

    //  These params are either in the JSON and need special saving procedures,
    //  or are in the UI and translate into a JSON param.

    //Gain Params
    specialProcessingParamList.append("Globals_Gain");
    specialProcessingParamList.append("Globals_LED_Refresh_Style");
    specialProcessingParamList.append("Keyboard_Global_Bend_Pad_Sensitivity"); // should covert from 100 to 1.0
    specialProcessingParamList.append("Keyboard_Global_Pressure_Sensitivity");
    specialProcessingParamList.append("Keyboard_Velocity_Gain");
    specialProcessingParamList.append("Keyboard_Pitch_Bend_Gain");
    specialProcessingParamList.append("Keyboard_Channel_Pressure_Gain");
    specialProcessingParamList.append("Keyboard_CC_00_Gain");
    specialProcessingParamList.append("Keyboard_CC_01_Gain");
    specialProcessingParamList.append("Keyboard_CC_02_Gain");
    specialProcessingParamList.append("Keyboard_CC_03_Gain");
    specialProcessingParamList.append("Keyboard_Poly_Aftertouch_Gain");
    specialProcessingParamList.append("CV_Out_Gate_Gain");
    specialProcessingParamList.append("CV_Out_CV_1_Gain");
    specialProcessingParamList.append("CV_Out_CV_2_Gain");
    specialProcessingParamList.append("CV_Out_CV_3_Gain");
    specialProcessingParamList.append("CV_In_CV_1_Gain");
    specialProcessingParamList.append("CV_In_CV_2_Gain");

    //LED Local CheckBox
    specialProcessingParamList.append("Keyboard_Global_LED_Mode");

    //Channel Rotation Check Box
    specialProcessingParamList.append("Keyboard_Global_Channel_Rotation_Active");

    //CV MIDI Input Type Menus
    specialProcessingParamList.append("CV_Out_CV_1_MIDI_Input_Type");
    specialProcessingParamList.append("CV_Out_CV_2_MIDI_Input_Type");
    specialProcessingParamList.append("CV_Out_CV_3_MIDI_Input_Type");

    //USB 1 & 2 Channels
    specialProcessingParamList.append("Keyboard_Global_USB_1_Channel");
    specialProcessingParamList.append("Keyboard_Global_USB_2_Channel");

    //Controller Channesl (Handles OBOE offsets)
    specialProcessingParamList.append("Controller_Key_0_Channel");
    specialProcessingParamList.append("Controller_Key_1_Channel");
    specialProcessingParamList.append("Controller_Key_2_Channel");
    specialProcessingParamList.append("Controller_Key_3_Channel");
    specialProcessingParamList.append("Controller_Key_4_Channel");
    specialProcessingParamList.append("Controller_Key_5_Channel");
    specialProcessingParamList.append("Controller_Key_6_Channel");
    specialProcessingParamList.append("Controller_Key_7_Channel");
    specialProcessingParamList.append("Controller_Key_8_Channel");
    specialProcessingParamList.append("Controller_Key_9_Channel");
    specialProcessingParamList.append("Controller_Key_10_Channel");
    specialProcessingParamList.append("Controller_Key_11_Channel");
    specialProcessingParamList.append("Controller_Key_12_Channel");
    specialProcessingParamList.append("Controller_Key_13_Channel");
    specialProcessingParamList.append("Controller_Key_14_Channel");
    specialProcessingParamList.append("Controller_Key_15_Channel");
    specialProcessingParamList.append("Controller_Key_16_Channel");
    specialProcessingParamList.append("Controller_Key_17_Channel");
    specialProcessingParamList.append("Controller_Key_18_Channel");
    specialProcessingParamList.append("Controller_Key_19_Channel");
    specialProcessingParamList.append("Controller_Key_20_Channel");
    specialProcessingParamList.append("Controller_Key_21_Channel");
    specialProcessingParamList.append("Controller_Key_22_Channel");
    specialProcessingParamList.append("Controller_Key_23_Channel");
    specialProcessingParamList.append("Controller_Key_24_Channel");
    specialProcessingParamList.append("Controller_Key_25_Channel");
    specialProcessingParamList.append("Controller_Bend_Pad_00_Channel");

    specialProcessingParamList.append("CV_Out_Gate_Channel");
    specialProcessingParamList.append("CV_Out_CV_1_Channel");
    specialProcessingParamList.append("CV_Out_CV_2_Channel");
    specialProcessingParamList.append("CV_Out_CV_3_Channel");
    specialProcessingParamList.append("CV_In_CV_1_Channel");
    specialProcessingParamList.append("CV_In_CV_2_Channel");

    specialProcessingParamList.append("Keyboard_Global_LED_Remote_Channel");

    specialProcessingParamList.append("Keyboard_Global_Pitch_Bend_Range");
    specialProcessingParamList.append("CV_Gate_Articulation");
    specialProcessingParamList.append("CV_Gate_S_Trig");

    specialProcessingParamList.append("Globals_Tilt_Sensitivity");

    /*/---- Source/Desitnation Menus
    //Pitch Bend
    specialProcessingParamList.append("Keyboard_Pitch_Bend_Source");
    specialProcessingParamList.append("Keyboard_Pitch_Bend_MIDI_Output_Device");

    //Channel Pressure
    specialProcessingParamList.append("Keyboard_Channel_Pressure_Source");
    specialProcessingParamList.append("Keyboard_Channel_Pressure_MIDI_Output_Device");

    //Poly Aftertouch
    specialProcessingParamList.append("Keyboard_Poly_Aftertouch_Source");
    specialProcessingParamList.append("Keyboard_Poly_Aftertouch_MIDI_Output_Device");

    //CC 0
    specialProcessingParamList.append("Keyboard_CC_00_Source");
    specialProcessingParamList.append("Keyboard_CC_00_MIDI_Output_Device");

    //CC 1
    specialProcessingParamList.append("Keyboard_CC_01_Source");
    specialProcessingParamList.append("Keyboard_CC_01_MIDI_Output_Device");

    //CC 2
    specialProcessingParamList.append("Keyboard_CC_02_Source");
    specialProcessingParamList.append("Keyboard_CC_02_MIDI_Output_Device");*/


    //------------------------------------------------------------------/
    //---------------------- Initialize Edit Buffer --------------------/
    //------------------------------------------------------------------/
    //Initialize Edit Buffer
    //editBufferPresetMap = defaultParamMap;

}

//-------- Loads JSON from file and checks validity --------//
void PresetManager::slotLoadJSON()
{

    QString jsonPath = QCoreApplication::applicationDirPath(); //get bundle path

#if defined(Q_OS_MAC) && !defined(QT_DEBUG)
    jsonPath.remove(jsonPath.length() - 5, jsonPath.length()); //Remove "MacOS" from path string
    jsonPath.append("Resources/presets/QuNexus.json");
#elif defined(Q_OS_MAC) && defined(QT_DEBUG)
    jsonPath = QString("../../../presets/QuNexus.json");
#elif !defined(Q_OS_MAC) // windows
    jsonPath.append("/presets/QuNexus.json");
#endif

    //Load json into QFile
    jsonFile = new QFile(jsonPath);
    // open file
//    QFile jsonFile(jsonPath);
    jsonFile->open(QIODevice::ReadOnly | QIODevice::Text);
    // error object
    QJsonParseError JsonParseError;
    // convert file to QJsonDocument. this can be read/written to
    QJsonDocument JsonDocument = QJsonDocument::fromJson(jsonFile->readAll(), &JsonParseError);
    // close jsonFile
    jsonFile->close();
    // convert QJsonDocument to QJsonObject. this can be queried and modified in a human-readable way
    QJsonObject RootObject = JsonDocument.object();

    qDebug() << "curdir: " << QDir::currentPath() << "\njsonPath: " << jsonPath << "\nappDir: " << QCoreApplication::applicationDirPath();


    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //qDebug("QuNexus JSON Found");

        //jsonByteArray = jsonFile->readAll();//load json file into a byte array to be processd by the parser
        jsonByteArray = JsonDocument.toJson();
        //jsonMasterMap = parser.parse(jsonByteArray, &ok).toMap(); //parse the json data, convert it to a map and set it equal to the master jsonMap
        jsonMasterMap = RootObject.toVariantMap();
        //jsonMasterMapEDITABLE = jsonMasterMap; //Copy of master map to be used for editing, reverting to factory, and dectecting changes
        jsonMasterMapEDITABLE = jsonMasterMap;

        //////////////////////////////////////////////////////////////
        ///////////////// Verify JSON's Contents Here ////////////////
        //////////////////////////////////////////////////////////////

        //Check JSONs preset contents
        if(!jsonMasterMapEDITABLE.contains(QString("Preset_000")))
        {
            QVariantMap emptyMap;
            jsonMasterMapEDITABLE.insert(QString("Preset_000"), emptyMap);

        }

        //Iterate through presets found in JSON
        for(int i = 0; i <jsonMasterMapEDITABLE.size(); i++)
        {

            QVariantMap    tempPresetMap;
            QString        tempPresetName = slotGetPresetNameFromIndex(i);

            //Copy prest map from file to virtual temp map
            tempPresetMap = jsonMasterMapEDITABLE.value(tempPresetName).toMap();

            if(jsonMasterMapEDITABLE.contains(tempPresetName))
            {
                //Iterate through the default map
                QMapIterator<QString, QVariant> mapIterator(defaultParamMap);

                while(mapIterator.hasNext())
                {
                    mapIterator.next();

                    //Check preset contents
                    if(tempPresetMap.contains(mapIterator.key()))
                    {
//                        qDebug() << mapIterator.key() << "FOUND IN JSON";
                    }
                    else
                    {
                        qDebug() << mapIterator.key() << "**** NOT FOUND in JSON ****" << mapIterator.value() ;
                        tempPresetMap.insert(mapIterator.key(), mapIterator.value()); //add new JSON params with default values if absent
                    }
                }

                //Insert Preset into JSON after verification
                jsonMasterMapEDITABLE.insert(tempPresetName, tempPresetMap); //insert updated map into json map
            }
        }

        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        //Set editable map to master to prepare for write
        jsonMasterMap = jsonMasterMapEDITABLE;

        jsonFile->resize(0);

        QJsonDocument jsonPresets = QJsonDocument::fromVariant(jsonMasterMapEDITABLE);
        jsonFile->write(jsonPresets.toJson()); //write json byte array to file
        jsonFile->close(); //close file and finalize write

    }
    else
    {
        qDebug() << "QuNexus Preset File Not Found!";
    }
}

//-------- Writes to QuNexus.json File, master map --------//
void PresetManager::slotWriteJSON(QVariantMap mapToWrite)
{
    //If not opened
    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {

        qDebug() << "Write Called from Constructor";

        //Write to master JSON
        jsonMasterMap = mapToWrite;

        //Serialize JSON into byte array
//        jsonByteArray = serializer.serialize(jsonMasterMap); //serialize the master json map into the byte array

        //Clear File
        jsonFile->resize(0); //clear jsonFile (set to 0 byte size)

        QJsonDocument jsonPresets = QJsonDocument::fromVariant(jsonMasterMap);

        //Write Byte Array into File
        jsonFile->write(jsonPresets.toJson()); //write json byte array to file

        //Close File
        jsonFile->close(); //close file and finalize write
    }

    //If File not opened/found
    else
    {
        qDebug() << "File could not be saved!";
    }
}

void PresetManager::slotWritePresetToJSON(QVariantMap preset, int newOrOld)
{

    presetIsDirty = false;

    switch(newOrOld)
    {
    case EXISTING_PRESET:

        //If not opened
        if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug() << "-------- slotWritePresetToJSON:open EXISTING_PRESET ------";

            //Insert new preset
            jsonMasterMap.insert(slotGetPresetNameFromIndex(selectedLibraryPreset), preset);

            //Serialize JSON into byte array
//            jsonByteArray = serializer.serialize(jsonMasterMap); //serialize the master json map into the byte array

            //Clear File
            jsonFile->resize(0); //clear jsonFile (set to 0 byte size)

            QJsonDocument jsonPresets = QJsonDocument::fromVariant(jsonMasterMap);

            //Write Byte Array into File
            jsonFile->write(jsonPresets.toJson()); //write json byte array to file

            //Close File
            jsonFile->close(); //close file and finalize write
        }

        //If File not opened/found
        else
        {
            qDebug() << "File could not be saved!";
        }

        break;

    case NEW_PRESET:
qDebug() << "-------- slotWritePresetToJSON:pre open NEW_PRESET ------";
        //If not opened
        if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
        {
            //GetNumber of presets
            int numPresets = jsonMasterMap.size();

            qDebug() << "preset to be inserted" << slotGetPresetNameFromIndex(numPresets);

            //Insert new preset
            jsonMasterMap.insert(slotGetPresetNameFromIndex(numPresets), preset);

            //Serialize JSON into byte array
//            jsonByteArray = serializer.serialize(jsonMasterMap); //serialize the master json map into the byte array

            //Clear File
            jsonFile->resize(0); //clear jsonFile (set to 0 byte size)

            QJsonDocument jsonPresets = QJsonDocument::fromVariant(jsonMasterMap);

            //Write Byte Array into File
            jsonFile->write(jsonPresets.toJson()); //write json byte array to file

            //Close File
            jsonFile->close(); //close file and finalize write
        }

        //If File not opened/found
        else
        {
            qDebug() << "File could not be saved!";
        }

        break;

    default:
        break;
    }
}

void PresetManager::slotRemovePresetFromJSON(int presetIndex)
{
    //If not opened
    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //Set up temp map to allow for re-indexing
        QVariantMap tempMasterMap = jsonMasterMap;

        //----------- Preset Removal, Map Reconstruction, File Saving
        //Remove preset from temp map
        tempMasterMap.remove(slotGetPresetNameFromIndex(presetIndex));

        //Clear the JSON
        jsonMasterMap.clear();

        //Iterate through temp map, get presets and store with correct indexes in master map
        QMapIterator<QString, QVariant> map(tempMasterMap);

        //Index number for
        int mapIndex = 0;

        while (map.hasNext())
        {
            //Move to next object in temp map
            map.next();

            //Insert object into JSON with correctly named string
            jsonMasterMap.insert(slotGetPresetNameFromIndex(mapIndex), map.value());

            //Move to next map
            mapIndex++;
        }

        //Serialize JSON into byte array
//        jsonByteArray = serializer.serialize(jsonMasterMap); //serialize the master json map into the byte array

        //Clear File
        jsonFile->resize(0); //clear jsonFile (set to 0 byte size)

        QJsonDocument jsonPresets = QJsonDocument::fromVariant(jsonMasterMap);

        //Write Byte Array into File
        jsonFile->write(jsonPresets.toJson()); //write json byte array to file

        //Close File
        jsonFile->close(); //close file and finalize write
    }

    //If File not opened/found
    else
    {
        qDebug() << "File could not be saved!";
    }
}

//-------- Called when UI value is changed, controls timeout --------//
void PresetManager::slotValueChanged()
{
    static QString previousKey, previousValue;
    QString thisKey, thisValue;

    //qDebug() << "slotValueChanged called - presetLoading: " << presetLoading << " newControllerKeySelected: " << newControllerKeySelected << " Sender:" << QObject::sender()->objectName();

    // value has changed, deselect any UI objects
    emit signalClearFocus();

    // hack fix to avoid spinboxes calling this twice
    if ( ( QObject::sender()->metaObject()->className() == QString("QSpinBox") ||
          QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox") ) &&
         !QObject::sender()->objectName().startsWith("Controller") ) // filter out controller layer
    {
        if (QObject::sender()->metaObject()->className() == QString("QSpinBox"))
        {
            QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
            thisKey = numberBox->objectName();
            thisValue = QString::number(numberBox->value());
        }
        else if(QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox"))
        {
            QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
            thisKey = floatBox->objectName();
            thisValue = QString::number(floatBox->value());
        }

        qDebug() << "thisKey: " << thisKey << " previousKey: " << previousKey << " thisValue: " << thisValue << " previousValue: " << previousValue;
        if (thisKey == previousKey && thisValue == previousValue) return; // ignore

        previousKey = thisKey;
        previousValue = thisValue;
    }


    if(!newControllerKeySelected)
    {
        if(!specialProcessingParamList.contains(QObject::sender()->objectName()))
        {

            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            //------------------------------  Controller Layer  ---------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            if(QObject::sender()->objectName().startsWith("Controller"))
            {
                //If not a Controller Layer Global
                if(!QObject::sender()->objectName().contains("Global"))
                {

                    QString paramKey = QObject::sender()->objectName();

                    qDebug() << "Controller Object Name" << QObject::sender()->objectName();

                    if(paramKey.contains("Bend"))
                    {
                        paramKey.remove(0, 19); // "_Note_Nnumber" or "_Pressure_Control_Number" etc.
//                        qDebug() << "paramKey" << paramKey;
                    }
                    else
                    {
                        paramKey.remove(0, 14); // "_Note_Nnumber" or "_Pressure_Control_Number" etc.
//                        qDebug() << "paramKey" << paramKey;
                    }

                    for(int i =0; i < controllerLayerSelectedComponents.count(); i++)
                    {

                        //-------- Controller Bend
                        if(controllerLayerSelectedComponents.at(i).contains("Bend"))
                        {
                            QString bendParamKey = paramKey;
                            bendParamKey.prepend("Controller_Bend_Pad_00");

                            if(!bendParamKey.endsWith("Channel"))
                            {
                                qDebug() << "bend param key" << bendParamKey;

                                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                                {
                                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                                    editBufferPresetMap.insert(bendParamKey, menu->currentText());
                                }
                                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                                {
                                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                                    editBufferPresetMap.insert(bendParamKey, numberBox->value());
                                }
                                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                                {
                                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                                    editBufferPresetMap.insert(bendParamKey, checkBox->isChecked());
                                }
                            }
                            else
                            {
                                QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                                editBufferPresetMap.insert(bendParamKey, numberBox->value() - 1);
                                //qDebug() << "channel" << bendParamKey << "," << numberBox->value() - 1;
                            }
                        }

                        //-------- Controller Key
                        else
                        {
                            QString keyParamKey = paramKey;

                            //Get Note #
                            QString paramKeyNum = controllerLayerSelectedComponents.at(i);
                            paramKeyNum.remove(0, 14);

                            //Make "0_Note_Number" or "11_Note_Number" etc.
                            keyParamKey.prepend(paramKeyNum);

                            //Make "Controller_Key_0_Note_Number" etc.
                            keyParamKey.prepend("Controller_Key_");

                            if(!keyParamKey.endsWith("Channel"))
                            {
                                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                                {
                                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                                    editBufferPresetMap.insert(keyParamKey, menu->currentText());
                                }
                                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                                {
                                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                                    editBufferPresetMap.insert(keyParamKey, numberBox->value());
                                }
                                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                                {
                                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                                    editBufferPresetMap.insert(keyParamKey, checkBox->isChecked());
                                }
                            }
                            else
                            {
                                QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                                editBufferPresetMap.insert(keyParamKey, numberBox->value() - 1);
                            }
                        }
                    }
                }

                //Controller Layer Global
                else
                {
                    if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                    {
                        QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                        editBufferPresetMap.insert("Controller_Global_MIDI_Output_Device", menu->currentText()); //Currently only global, no need for string processing
                    }
                }
            }


            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-------------------------------  Keyboard Layer -----------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            else if(QObject::sender()->objectName().startsWith("Keyboard"))
            {
                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                {
                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                    editBufferPresetMap.insert(menu->objectName(), menu->currentText());
                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << menu->objectName() << menu->currentText();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                {
                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(numberBox->objectName(), numberBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << numberBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox"))
                {
                    QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(floatBox->objectName(), floatBox->value());

                    qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << floatBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                {
                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                    editBufferPresetMap.insert(checkBox->objectName(), checkBox->isChecked());

                    //qDebug() << "Value Changed  :" << checkBox->objectName() << QObject::sender()->objectName() << checkBox->checkState();
                }
                else
                {
                    //qDebug() << "slotValueChanged Type Unknown" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName();
                }
            }

            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-------------------------------  SEQ Layer ----------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            else if(QObject::sender()->objectName().startsWith("SEQ"))
            {
                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                {
                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                    editBufferPresetMap.insert(menu->objectName(), menu->currentText());
                    qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << menu->objectName() << menu->currentText();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                {
                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(numberBox->objectName(), numberBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << numberBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox"))
                {
                    QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(floatBox->objectName(), floatBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << floatBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                {
                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                    editBufferPresetMap.insert(checkBox->objectName(), checkBox->isChecked());

                    //qDebug() << "Value Changed  :" << checkBox->objectName() << QObject::sender()->objectName() << checkBox->checkState();
                }
                else
                {
                    //qDebug() << "slotValueChanged Type Unknown" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName();
                }
            }


            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            //------------------------------------  CV Layer ------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            else if(QObject::sender()->objectName().startsWith("CV"))
            {
                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                {
                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                    editBufferPresetMap.insert(menu->objectName(), menu->currentText());
                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << menu->objectName() << menu->currentText();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                {
                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(numberBox->objectName(), numberBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << numberBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox"))
                {
                    QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(floatBox->objectName(), floatBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << floatBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                {
                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                    editBufferPresetMap.insert(checkBox->objectName(), checkBox->isChecked());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << checkBox->checkState();
                }
                else
                {
                    //qDebug() << "slotValueChanged Type Unknown" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName();
                }
            }


            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------  Globals --------------------------------------//
            //-----------------------------------------------------------------------------------//
            //-----------------------------------------------------------------------------------//
            else if(QObject::sender()->objectName().startsWith("Globals"))
            {
                if(QObject::sender()->metaObject()->className() == QString("QComboBox"))
                {
                    QComboBox *menu = reinterpret_cast<QComboBox *>(QObject::sender());
                    editBufferPresetMap.insert(menu->objectName(), menu->currentText());
                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << menu->objectName() << menu->currentText();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QSpinBox"))
                {
                    QSpinBox *numberBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(numberBox->objectName(), numberBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << numberBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QDoubleSpinBox"))
                {
                    QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
                    editBufferPresetMap.insert(floatBox->objectName(), floatBox->value());

                    //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName() << floatBox->value();
                }
                else if(QObject::sender()->metaObject()->className() == QString("QCheckBox"))
                {
                    QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
                    editBufferPresetMap.insert(checkBox->objectName(), checkBox->isChecked());

                    //qDebug() << "Value Changed  :" << checkBox->objectName() << QObject::sender()->objectName() << checkBox->checkState();
                }
                else
                {
                    //qDebug() << "slotValueChanged Type Unknown" << QObject::sender()->metaObject()->className() << QObject::sender()->objectName();
                }
            }
        }


        //-----------------------------------------------------------------------------------//
        //-----------------------------------------------------------------------------------//
        //--------------------------  Special Processing Params -----------------------------//
        //-----------------------------------------------------------------------------------//
        //-----------------------------------------------------------------------------------//
        else
        {
            //---- Gains
            if( QObject::sender()->objectName().contains("Gain") ||
                    QObject::sender()->objectName() == "Keyboard_Global_Bend_Pad_Sensitivity" ||
                    QObject::sender()->objectName() == "Keyboard_Global_Pressure_Sensitivity")
            {
                //Multiply gain by 100 to store correctly in JSON.
                QDoubleSpinBox *floatBox = reinterpret_cast<QDoubleSpinBox *>(QObject::sender());
                editBufferPresetMap.insert(floatBox->objectName(), floatBox->value()*100);
            }

            //Inverted Tilt UI
            else if(QObject::sender()->objectName().contains("Globals_Tilt_Sensitivity"))
            {
                //Multiply gain by 100 to store correctly in JSON.
                QSpinBox *spinBox = reinterpret_cast<QSpinBox *>(QObject::sender());
                editBufferPresetMap.insert(spinBox->objectName(), 127 - spinBox->value());
                //qDebug() << "stored value" << editBufferPresetMap.value("Globals_Tilt_Sensitivity").toInt();
            }

            //---- Keyboard LED Mode param
            else if(QObject::sender()->objectName().contains("Keyboard_Global_LED_Mode"))
            {
                QCheckBox* ledMode = reinterpret_cast<QCheckBox *>(QObject::sender());

                //qDebug() << "Value Changed  :" << QObject::sender()->objectName() << ledMode->checkState();

                if(ledMode->checkState())
                {
                    editBufferPresetMap.insert("Keyboard_Global_LED_Mode", 1);
                    //emit signalSetEnableUiParam("Keyboard_Global_LED_Remote_Channel", 0);

                }
                else
                {
                    editBufferPresetMap.insert("Keyboard_Global_LED_Mode", 0);
                    //emit signalSetEnableUiParam("Keyboard_Global_LED_Remote_Channel", 1);
                }
            }

            //----- Keyboard Channel Rotation Active
            else if(QObject::sender()->objectName().contains("Keyboard_Global_Channel_Rotation_Active"))
            {
                QCheckBox* channelRotationOnOff = reinterpret_cast<QCheckBox *>(QObject::sender());

                //qDebug() << "Value Changed  :" << QObject::sender()->objectName() << channelRotationOnOff->checkState();

                if(channelRotationOnOff->checkState())
                {
                    editBufferPresetMap.insert("Keyboard_Global_Channel_Rotation_Active", 1);
                    emit signalSetEnableUiParam("Keyboard_Global_Polyphony_Number", 1);
                }
                else
                {
                    editBufferPresetMap.insert("Keyboard_Global_Channel_Rotation_Active", 0);
                    emit signalSetEnableUiParam("Keyboard_Global_Polyphony_Number", 0);
                }
            }

            //---- CV MIDI Input Type Menus
            else if(QObject::sender()->objectName().startsWith("CV_Out_CV") && QObject::sender()->objectName().endsWith("MIDI_Input_Type"))
            {

                QComboBox* cvOutMidiInputType = reinterpret_cast<QComboBox *>(QObject::sender());

                //qDebug() << "Value Changed  :" << QObject::sender()->metaObject()->className() << cvOutMidiInputType->objectName() << cvOutMidiInputType->currentText();

//                //If a CC # just treat normally
//                if(cvOutMidiInputType->currentText().startsWith("CC"))
//                {
                    editBufferPresetMap.insert(QObject::sender()->objectName(), cvOutMidiInputType->currentText());
//                }

//                //Otherwise append with CV Out to differentiate from other layer sources in dictionary
//                else
//                {
//                    editBufferPresetMap.insert(QObject::sender()->objectName(), cvOutMidiInputType->currentText().append(" CV Out"));
//                }
            }

            //---- Channels (to be linked to "Keyboard_Global_Channel_Rotation_Offset")
            else if(QObject::sender()->objectName().startsWith("Keyboard_Global_USB") && QObject::sender()->objectName().endsWith("Channel"))
            {

                //--------------- Handle Rotation Offset

                QSpinBox *channelSpinBox = reinterpret_cast<QSpinBox *>(QObject::sender());

                //-------------- Handle UI OBOE Offset and Save
                editBufferPresetMap.insert(QObject::sender()->objectName(), channelSpinBox->value() - 1);

                //Analyze both values, if 2 is greater, set offset to usb 2 channel number
                if(editBufferPresetMap.value("Keyboard_Global_USB_2_Channel").toInt() > editBufferPresetMap.value("Keyboard_Global_USB_1_Channel").toInt())
                {
                    //qDebug() << "Insert USB 2 into Offset:" << editBufferPresetMap.value("Keyboard_Global_USB_2_Channel").toInt();
                    editBufferPresetMap.insert("Keyboard_Global_Channel_Rotation_Offset", editBufferPresetMap.value("Keyboard_Global_USB_2_Channel").toInt());
                }

                //If the same or 1 is greater, set offset to usb 1 channel number
                else
                {
                    //qDebug() << "Insert USB 1 into Offset:" << editBufferPresetMap.value("Keyboard_Global_USB_1_Channel").toInt();
                    editBufferPresetMap.insert("Keyboard_Global_Channel_Rotation_Offset", editBufferPresetMap.value("Keyboard_Global_USB_1_Channel").toInt());
                }
            }

            //---- Keyboard LED Layer OBOE
            else if(QObject::sender()->objectName().contains("Keyboard_Global_LED_Remote_Channel"))
            {
                QSpinBox *spinBox = reinterpret_cast<QSpinBox *>(QObject::sender());

                //-------------- Handle UI OBOE Offset and Save
                editBufferPresetMap.insert(QObject::sender()->objectName(), spinBox->value() - 1);
            }

            //----------------------- Keyboard Pitch Bend
            else if(QObject::sender()->objectName().contains("Keyboard_Global_Pitch_Bend_Range"))
            {
                QSpinBox *spinBox = reinterpret_cast<QSpinBox *>(QObject::sender());

                //Store Pitch Bend Range in JSON
                editBufferPresetMap.insert(QObject::sender()->objectName(), spinBox->value());

                //Set Pitch Bend Min/Max UI, they will be stored on their own
                QSpinBox *bendMin = this->parentWidget()->findChild<QSpinBox *>("Keyboard_Pitch_Bend_Min");
                QSpinBox *bendMax = this->parentWidget()->findChild<QSpinBox *>("Keyboard_Pitch_Bend_Max");

                unsigned char bendRangeMin[13] = {64,58,53,48,43,37,32,27,22,16,11,6,0};
                unsigned char bendRangeMax[13] = {64,69,74,79,85,90,95,100,106,111,116,121,127};

                bendMin->setValue(bendRangeMin[spinBox->value()]);
                bendMax->setValue(bendRangeMax[spinBox->value()]);
            }


            //---- Controller Layer OBOE
            else if(QObject::sender()->objectName().startsWith("Controller") && QObject::sender()->objectName().endsWith("Channel"))
            {
                QSpinBox *spinBox = reinterpret_cast<QSpinBox *>(QObject::sender());

                //-------------- Handle UI OBOE Offset and Save
                editBufferPresetMap.insert(QObject::sender()->objectName(), spinBox->value() - 1);
            }

            //---- CV Layer OBOE
            else if(QObject::sender()->objectName().startsWith("CV") && QObject::sender()->objectName().endsWith("Channel"))
            {
                QSpinBox *spinBox = reinterpret_cast<QSpinBox *>(QObject::sender());

                //-------------- Handle UI OBOE Offset and Save
                editBufferPresetMap.insert(QObject::sender()->objectName(), spinBox->value() - 1);
            }

            //---- CV Gate Articulation
            else if(QObject::sender()->objectName().contains("CV_Gate_Articulation"))
            {
                QCheckBox* checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());

                //qDebug() << "Value Changed  :" << QObject::sender()->objectName() << checkBox->checkState();

                if(checkBox->checkState())
                {
                    editBufferPresetMap.insert("CV_Gate_Articulation", 1);
                }
                else
                {
                    editBufferPresetMap.insert("CV_Gate_Articulation", 0);
                }
            }
            //---- CV Gate Articulation
            else if(QObject::sender()->objectName().contains("CV_Gate_S_Trig"))
            {
                QCheckBox* checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());

                //qDebug() << "Value Changed  :" << QObject::sender()->objectName() << checkBox->checkState();

                if(checkBox->checkState())
                {
                    editBufferPresetMap.insert("CV_Gate_S_Trig", 1);
                }
                else
                {
                    editBufferPresetMap.insert("CV_Gate_S_Trig", 0);
                }
            }
        }

        //If a preset is selected
        if(selectedLibraryPreset >= 0)
        {
            //Insert edit buffer into editable json map
            //qDebug() << "<< Insert edit buffer into editable json map: index" << selectedLibraryPreset;

            jsonMasterMapEDITABLE.insert(slotGetPresetNameFromIndex(selectedLibraryPreset), editBufferPresetMap);
        }

        //Impose limits on source and output mappings
        emit signalLimitSourceOutputMenus();

        // this will check if a preset or JSON is loading
        slotSyncEditBufferToDevice();
    }

    // dirty the preset in the library
    presetIsDirty = true;
    emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_MODIFIED, 0);

}

//-------- Called When Controller Layer Key is Selected --------//
void PresetManager::slotSelectControllerLayerKey()
{
    //If shift is held, do not clear the list of buttons selected
    if(!shiftDown)
    {
        controllerLayerSelectedComponents.clear();
    }

    //If Key is highlighted, and not the only key selected, remove it
    if(controllerLayerSelectedComponents.contains(QObject::sender()->objectName()) && controllerLayerSelectedComponents.count() > 1)
    {
        controllerLayerSelectedComponents.removeOne(QObject::sender()->objectName());
    }

    //Otherwise if key is not highlighted, add it to list and highlight
    else if(!controllerLayerSelectedComponents.contains(QObject::sender()->objectName()))
    {
        controllerLayerSelectedComponents.append(QObject::sender()->objectName());
    }

    emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
}

///////////////////////////////////////////////////////////////////
///////////// Library Preset & Edit Buffer Fucntions //////////////
///////////////////////////////////////////////////////////////////

//-------- Timeout used to trigger Edit Buffer Update --------//
void PresetManager::slotTimeoutSyncPreset()
{
    if(valueChangedTimeoutTime < 250)
    {
        valueChangedTimeoutTime++;
    }
    else
    {
        qDebug() << "\n____Sync Edit Buffer____\n";
        slotSyncEditBufferToDevice();

        emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_MODIFIED, 0);
    }
}

void PresetManager::slotSaveSelectedPreset()
{
    qDebug() << "slotSaveSelectedPreset called";
    if(selectedLibraryPreset != -1)
    {
        slotWritePresetToJSON(jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap(), EXISTING_PRESET);

        //emit signalUpdateParameterUi(editBufferPresetMap);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);
        qDebug() << "jsonMasterMapEDITABLE" << jsonMasterMapEDITABLE.value("Controller_Key_Pressure_Control_Number");
        emit signalSendPresetsToBoardDialog(jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap().value("Preset_Name").toString());
    }
    presetIsDirty = false;
}

void PresetManager::slotRevertSelectedPreset()
{
    qDebug() << "slotRevertSelectedPreset called";
    if(selectedLibraryPreset != -1)
    {
        // set flag to prevent from sending sysex to qunexus - this is now done outside of this function
        //JSONLoading = true;

        //Load prest from master map into editable master map
        jsonMasterMapEDITABLE.insert(slotGetPresetNameFromIndex(selectedLibraryPreset), jsonMasterMap.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap());

        //Insert reverted preset into editBuffer
        editBufferPresetMap = jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap();

        emit signalUpdateParameterUi(editBufferPresetMap);
        emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);

        // clear flag and sync
        //qDebug() << "slotRevertSelectedPreset: JSONLoading set to FALSE";
        //JSONLoading = false;
        slotSyncEditBufferToDevice();
    }
    presetIsDirty = false;
}

// load a file into the edit buffer (now consolidating revert/import factory slots)
bool PresetManager::slotLoadPresetFileIntoEditBuffer(QString filename)
{
    qDebug() << "slotLoadPresetFileIntoEditBuffer: " << filename;
    //If file is selected
    if(!filename.isNull())
    {

        // open file
        QFile presetFile(filename);
        presetFile.open(QIODevice::ReadOnly | QIODevice::Text);
        // error object
        QJsonParseError JsonParseError;
        // convert file to QJsonDocument. this can be read/written to
        QJsonDocument JsonDocument = QJsonDocument::fromJson(presetFile.readAll(), &JsonParseError);
        // close jsonFile
        presetFile.close();
        // convert QJsonDocument to QJsonObject. this can be queried and modified in a human-readable way
        QJsonObject RootObject = JsonDocument.object();

        QVariantMap importedPresetMap = RootObject.toVariantMap();
        //----------------------------------------------------- Check for MISSING parameters in the Imported Preset ----------------------------------------------------//
        QMapIterator<QString, QVariant> i(defaultParamMap);

        //Iterate through default map and compare with imported preset
        while(i.hasNext())
        {
            i.next();

            if(!importedPresetMap.contains(i.key()))
            {
                //If imported preset map does not contain a value in default map, insert it
                importedPresetMap.insert(i.key(), i.value());
            }
        }

        //----------------------------------------------------- Check for EXTRA parameters in the Imported Preset ----------------------------------------------------//
        QMapIterator<QString, QVariant> j(importedPresetMap);

        QStringList badKeys; //Stores keys we need to remove from the map

        //Iterate through preset
        while(j.hasNext())
        {
            j.next();

            //If default map does not contain something in the preset
            if(!defaultParamMap.contains(j.key()))
            {
                //Add to list of bad keys
                badKeys.append(j.key());
            }
        }

        //Iterate through bad keys and remove from preset
        for(int i =0; i<badKeys.count(); i++)
        {
            importedPresetMap.remove(badKeys.at(i));
        }

        //--------------------------- Load Imported Preset into Edit Buffer
        editBufferPresetMap = importedPresetMap;

        // done
        return 1;

    } else {
        return 0;
    }

}

// import a user selected preset file into the library
void PresetManager::slotImportPreset()
{
    qDebug() << "IMPORT SENDER" << QObject::sender()->objectName();

    QString sender = QObject::sender()->objectName();
    QString filename = NULL;

    QSettings settings;

    const QString DEFAULT_DIR_KEY("default_dir");

    filename = QFileDialog::getOpenFileName(this, tr("Import Preset"), settings.value(DEFAULT_DIR_KEY).toString(),tr("QuNexus Preset Files (*.qunexuspreset)"));

    if(!filename.isEmpty() && !filename.isNull())
    {
        if (slotLoadPresetFileIntoEditBuffer(filename))
        {
            // store this folder location for the next time we open a file
            QDir CurrentDir;
            settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(filename));

            // update library
            slotSaveEditBufferToLibrary(editBufferPresetMap.value("Preset_Name").toString());

            // update parameter UI
            emit signalUpdateParameterUi(editBufferPresetMap);
            emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
            emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);
        }
        else
        {
            qDebug() << "Error loading file: " << filename;
        }
    }

}

// load a factory preset file into the current edit buffer
void PresetManager::slotRevertToFactory()
{
    QString sender = QObject::sender()->objectName();
    QString filename = QString(":factorypresets/resources/factorypresets/%1.qunexuspreset").arg(sender);

    if (slotLoadPresetFileIntoEditBuffer(filename))
    {
        // update parameter UI
        emit signalUpdateParameterUi(editBufferPresetMap);
        emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_MODIFIED, 0);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_SELECTED, 0);
    }
    else
    {
        qDebug() << "Error loading file: " << filename;
    }
}

// import a factory preset into the edit buffer and library
void PresetManager::slotImportFactory()
{
    QString sender = QObject::sender()->objectName();
    QString filename = QString(":factorypresets/resources/factorypresets/%1.qunexuspreset").arg(sender);

    qDebug() << "Import factory preset: " << sender;

    if (slotLoadPresetFileIntoEditBuffer(filename))
    {
        // update library
        slotSaveEditBufferToLibrary(editBufferPresetMap.value("Preset_Name").toString());

        // update parameter UI
        emit signalUpdateParameterUi(editBufferPresetMap);
        emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);
    }
    else
    {
        qDebug() << "Error loading file: " << filename;
    }
}

// export the edit buffer to a preset file
void PresetManager::slotExportPreset()
{
    // pull the last stored file location
    QSettings settings;

    const QString DEFAULT_DIR_KEY("default_dir");

    //Set path and filename (default filename is Preset_Name
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Preset"),
                                                    settings.value(DEFAULT_DIR_KEY).toString().append(editBufferPresetMap.value("Preset_Name").toString()),
                                                    tr("QuNexus Preset Files (*.qunexuspreset)"));
    if(!filename.isEmpty() && !filename.isNull())
    {
        // store this file location for the next time we open/save a file
        // store this folder location for the next time we open a file
        QDir CurrentDir;
        settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(filename));


        //This gets the file name without the path
        QFileInfo fileInfo(filename);

        //Open new file to be saved
        QFile* presetFile = new QFile(filename);

        //Remove extension from filename
        QString exportedPresetName = fileInfo.fileName().remove(".qunexuspreset");

        qDebug() << "filename:" << fileInfo.fileName().remove(".qunexuspreset");

        //Make new map to export
        QVariantMap exportMap = editBufferPresetMap;

        //Replace "Preset_Name" with filename typed in dialog
        exportMap.insert("Preset_Name", exportedPresetName);

        //-------------- Open, Write, and Close
        presetFile->open(QIODevice::WriteOnly | QIODevice::Text);

    //    QByteArray presetByteArray = serializer.serialize(editBufferPresetMap);

        presetFile->resize(0);

        QJsonDocument jsonPresets = QJsonDocument::fromVariant(editBufferPresetMap);

        //Write Byte Array into File
        presetFile->write(jsonPresets.toJson()); //write json byte array to file

        //Close File
        presetFile->close(); //close file and finalize write
    }
}

//-------- Opens Dialog for New Preset Naming --------//
void PresetManager::slotSaveNewPreset()
{
    qDebug() << "keys" << jsonMasterMapEDITABLE.keys();

    PresetNameDialog* presetNameDialog = new PresetNameDialog(this->parentWidget());
    connect(presetNameDialog, SIGNAL(signalSaveNewPreset(QString)), this, SLOT(slotSaveEditBufferToLibrary(QString)));
    presetNameDialog->exec();
}

//-------- Saves Edit Buffer to new library preset --------//
void PresetManager::slotSaveEditBufferToLibrary(QString presetName)
{

    qDebug() << "SAVED?" << presetName;
    //Insert preset name into edit buffer map
    editBufferPresetMap.insert("Preset_Name", presetName);

    //GetNumber of presets
    int numPresets = jsonMasterMapEDITABLE.size();

    //Insert new preset into edit map
    jsonMasterMapEDITABLE.insert(slotGetPresetNameFromIndex(numPresets), editBufferPresetMap);

    //qDebug() << "preset Name" << slotGetPresetNameFromIndex(numPresets);

    //Write new preset to JSON so that editable map and master maps align
    slotWritePresetToJSON(editBufferPresetMap, NEW_PRESET);

    selectedLibraryPreset = numPresets;

    //Refresh library display
    emit signalUpdateLibraryPresetsUi(jsonMasterMapEDITABLE, CREATE_PRESET, 0);
    emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);

    presetIsDirty = false;

}

//-------- Sends current edit buffer in editor to device edit buffer (RAM) --------//
void PresetManager::slotSyncEditBufferToDevice()
{
    if (!syncFlag || presetLoading || JSONLoading)
    {
        //qDebug() << "slotSyncEditBufferToDevice called and blocked - presetloading:" << presetLoading << " JSONLoading:" << JSONLoading;
        return; // do not send sysex if we are loading sysex or JSON preset data
    }
    else
    {
        //qDebug() << "slotSyncEditBufferToDevice called and executed";
    }

    // send edit buffer to qunexus ram
    emit signalSysExEncodePreset(editBufferPresetMap, 0x63); // used to be 0xFF
}

//-------- Sends current edit buffer in editor to device Preset (FLASH) --------//
void PresetManager::slotSyncEditBufferToDevicePreset()
{
    QString presetNumString = QObject::sender()->objectName();
    unsigned char     presetNumChar;

    presetNumString.remove(QString("syncPreset"));
    presetNumChar = presetNumString.toInt();

    qDebug() << "Sync Edit Buffer to Device Preset" << presetNumChar;

    //If another preset was associated with a slot on the board
    for(int i = 0; i < 4; i++)
    {
        if(settings->value(QString("boardPreset%1").arg(i)).toInt() == selectedLibraryPreset)
        {
            settings->setValue(QString("boardPreset%1").arg(i), -1);
        }
    }

    settings->setValue(QString("boardPreset%1").arg(presetNumChar), selectedLibraryPreset);

    emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_SELECTED, presetNumChar);
    emit signalSysExEncodePreset(editBufferPresetMap, presetNumChar);
}

/*
void PresetManager::slotSaveAndSend()
{
    if(selectedLibraryPreset != -1)
    {
        slotWritePresetToJSON(jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap(), EXISTING_PRESET);

        //emit signalUpdateParameterUi(editBufferPresetMap);
        emit signalUpdateLibraryPresetsUi(QVariantMap(), SET_MODIFIED, 0);

        //emit signalSendPresetsToBoardDialog(jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap().value("Preset_Name").toString());
    }

    QPushButton *button = reinterpret_cast<QPushButton *>(QObject::sender());

    if(button->text() == "Save (Not Sent)")
    {
        qDebug() << "Save not sent";
    }
    else
    {
        for(int i = 0; i < 4; i ++)
        {
            if(settings->value(QString("boardPreset%1").arg(i)) == selectedLibraryPreset)
            {
                qDebug() << "Saved && Sent";
                emit signalSysExEncodePreset(editBufferPresetMap, i);
                break;
            }
        }
    }

    //qDebug() << "Save && Send or Save (Not Sent)";
}
*/

// save the current preset and then call slotLoadLibraryPresetIntoEditBuffer
void PresetManager::slotSaveThenLoadPreset(QListWidgetItem* selectedItem)
{
    if(selectedLibraryPreset != -1)
    {
        slotWritePresetToJSON(jsonMasterMapEDITABLE.value(slotGetPresetNameFromIndex(selectedLibraryPreset)).toMap(), EXISTING_PRESET);
    }
    slotLoadLibraryPresetIntoEditBuffer(selectedItem);
}

//-------- Loads preset from library into edit buffer, uses "selected" / double-clicked preset --------//
void PresetManager::slotLoadLibraryPresetIntoEditBuffer(QListWidgetItem* selectedItem)
{
    qDebug() << "slotLoadLibraryPresetIntoEditBuffer called";
    qDebug() << "unsavedWarningsEnabled: " <<  settings->value("unsavedWarningsEnabled").toBool() << "dirty: " << presetIsDirty;
    if(settings->value("unsavedWarningsEnabled").toBool() && presetIsDirty == true)
    {
        emit signalCallUnsavedChangesDialog(selectedItem);
        return;
    }

    if(selectedItem)
    {
        qDebug() << "current index" << selectedItem->listWidget()->currentRow();

        // set flag to prevent from sending sysex to qunexus
        JSONLoading = true;

        // revert current preset
        slotRevertSelectedPreset();

        //qDebug() << "load this item?" << selectedItem->text();

        selectedLibraryPreset = selectedItem->listWidget()->row(selectedItem);

        //Get preset nameo highlighted index
        QString tempPresetName = slotGetPresetNameFromIndex(selectedLibraryPreset);

        qDebug() << "\n____Load" << tempPresetName << "into Edit Buffer____";

        //Load preset into edit buffer
        editBufferPresetMap = jsonMasterMapEDITABLE.value(tempPresetName).toMap();

        //Update Keyboard, CV and Controller Tabs
        emit signalUpdateParameterUi(editBufferPresetMap); //Updates Keyboard, CV
        emit signalControlLayerSelectedComponents(controllerLayerSelectedComponents); //Updates Visible Controller Pane

        //Shows correct GUI color scheming
        emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);

        // set loading flag
//        qDebug() << "<< set presetLoading: false";
//        if (syncFlag) presetLoading = false; // don't set this flag positive until after we have done the initial sync
        qDebug() << "Finished loading preset JSON, syncFlag: " << syncFlag << "Setting JSONLoading to FALSE";
        JSONLoading = false;
        presetIsDirty = false;
        slotSyncEditBufferToDevice();
    }
}

//-------- Delete's preset from libarary and JSON file --------//
void PresetManager::slotDeletePreset()
{
    //Copy JSON Master into a Temp Map
    QVariantMap tempMasterMap = jsonMasterMapEDITABLE; //Map used to properly format preset_NNN names upon deletion
    QString tempPresetNameParam;

    qDebug() <<  "slotDeletePreset called";

    //Get "Preset_Name" to display in warning dialog
    tempPresetNameParam = tempMasterMap.value(slotGetPresetNameFromIndex(highlightedLibraryPreset)).toMap().value("Preset_Name").toString();

    qDebug() << "slotDeletePreset - tempPresetNameParam:" << tempPresetNameParam;

    //----------- Preset Removal, Map Reconstruction, File Saving
    //Remove preset from temp map
    tempMasterMap.remove(slotGetPresetNameFromIndex(highlightedLibraryPreset));

    //Clear the JSON
    jsonMasterMapEDITABLE.clear();

    //Iterate through temp map, get presets and store with correct indexes in master map
    QMapIterator<QString, QVariant> map(tempMasterMap);

    //Index number for
    int mapIndex = 0;

    while (map.hasNext())
    {
        //Move to next object in temp map
        map.next();

        //Insert object into JSON with correctly named string
        jsonMasterMapEDITABLE.insert(slotGetPresetNameFromIndex(mapIndex), map.value());

        //Move to next map
        mapIndex++;
    }

    //Write to the actual file
    //slotWriteJSON(jsonMasterMapEDITABLE);

    slotRemovePresetFromJSON(highlightedLibraryPreset);

    deletedPreset = highlightedLibraryPreset;

    qDebug() << "delete now calls signalUpdateLibraryPresetsUi";
    //Update the Library list UI
    emit signalUpdateLibraryPresetsUi(jsonMasterMapEDITABLE, DELETE_PRESET, 0);
    qDebug() << "delete return from signalUpdateLibraryPresetsUi";

    //qDebug() << "Modified Preset Indicies???" << slotCheckPresetEdits();
    emit signalUpdateLibraryPresetsUi(QVariantMap(), LOAD_PRESET, 0);

    //Load new preset into Edit buffer, set lsit item's visual properties
    //slotLoadLibraryPresetIntoEditBuffer(listWidget->item(selectedLibraryPreset));
}

//------- Formats JSON String from Index Number -------/
QString PresetManager::slotGetPresetNameFromIndex(int i)
{
    QString tempPresetName;

    if((i) < 10)
    {
        tempPresetName = QString("Preset_00%1").arg(i);
    }
    else if((i) < 100)
    {
        tempPresetName = QString("Preset_0%1").arg(i);
    }
    else
    {
        tempPresetName = QString("Preset_%1").arg(i);
    }

    return tempPresetName;
}

//------ Check Preset Edits --------//
QList<int> PresetManager::slotCheckPresetEdits()
{
    QList<int>  modifiedIndices; // preset index

    for(int i=0; i < jsonMasterMap.count(); i++)
    {
        //Temp Preset Name
        QString tempPresetName = slotGetPresetNameFromIndex(i);

        //Current Preset Map iteration in both editable map and master (unsaved) map
        QVariantMap editablePreset = jsonMasterMapEDITABLE.value(tempPresetName).toMap();
        QVariantMap jsonPreset = jsonMasterMap.value(tempPresetName).toMap();

        if(jsonPreset != editablePreset)
        {
            modifiedIndices.append(i);
//            qDebug() << "modified preset index" << modifiedIndices << i;
        }
    }

    return modifiedIndices;
}

void PresetManager::slotUpdateHighlightedPreset(QListWidgetItem* selectedItem)
{
    qDebug() << "slotUpdateHighlightedPreset called: " << selectedItem->listWidget()->row(selectedItem);
    highlightedLibraryPreset = selectedItem->listWidget()->row(selectedItem);
}
