// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QWidget>
#include <QDebug>
#include <QtWidgets> // was QT Gui

#include <QFile>
#include <QVariant>
//#include "qjson/src/parser.h"
//#include "qjson/src/serializer.h"
#include <QListWidget>

#include "presetnamedialog.h"


enum {DELETE_PRESET, CREATE_PRESET, SET_HIGHLIGHT, SET_SELECTED, SET_MODIFIED, LOAD_PRESET};
enum {NEW_PRESET, EXISTING_PRESET};

// forward declarations for classes
class SysExEncDecode;

class PresetManager : public QWidget
{
    Q_OBJECT
public:
    explicit PresetManager(QWidget *parent = 0);

    QVariantMap     defaultParamMap; //a hardcoded list of all parameters with default values (key, value)

    QStringList     specialProcessingParamList;

    QStringList     controllerLayerSelectedComponents;

    QListWidgetItem *selectedLibraryItem;
    QListWidget     *listWidget;

    QFile*          jsonFile;
    QByteArray      jsonByteArray;
    QVariantMap     jsonMasterMap;
    QVariantMap     jsonMasterMapEDITABLE;

    QVariantMap     editBufferPresetMap;
    QVariantMap     libraryPresetMaps;

    // class pointers
    SysExEncDecode*     sysExEncDecode;

    int             selectedLibraryPreset;
    int             highlightedLibraryPreset;
    int             deletedPreset;

    bool            newControllerKeySelected;

    QListWidget*    libraryPresetsList;

    QMap<QString, QVariant>::const_iterator mapIterator;

    //EB Remove for qt6 build
    //QJson::Parser       parser;
    //QJson::Serializer   serializer;
    bool ok;

    int     currentPreset;

    int     valueChangedTimeoutTime;

    bool    shiftDown;

    bool    presetLoading;  // flag that we are loading a sysex preset received from QuNexus
    bool    JSONLoading;    // flag that we are loading a JSON preset from the library

    bool    syncFlag;
    bool    presetIsDirty;  // flag to tell us if the current preset has been changed

    QSettings* settings;

    //----------------------------------- Stylesheets
    //Library
    QFile*              selectedStyleSheetFile;
    QString             selectedStyleSheetString;
    QFile*              unselectedStyleSheetFile;
    QString             unselectedStyleSheetString;

    //Buttons
    QFile* blueStyleFile;
    QString blueStyleString;
    //Buttons
    QFile* grayStyleFile;
    QString grayStyleString;
    
signals:
    void signalUpdateParameterUi(QVariantMap);
    void signalUpdateLibraryPresetsUi(QVariantMap, int, int);
    void signalControlLayerSelectedComponents(QStringList selections);

    void signalSetEnableUiParam(QString paramName, bool state);
    void signalLimitSourceOutputMenus();

    void signalAddItemToLibraryPresetList(QString);

    void signalSysExEncodePreset(QVariantMap, unsigned char);

    void signalSendPresetsToBoardDialog(QString);
    void signalCallUnsavedChangesDialog(QListWidgetItem *selectedItem);

    void signalClearFocus();

    void signalLibraryReady();
    
public slots:

    //--------- Initializaion and Control
    void slotCreateDefaultParamMap();
    void slotValueChanged();
    void slotTimeoutSyncPreset();
    void slotSelectControllerLayerKey();

    //-------- Saving, Creating, Recalling, Formatting, Deleting Presets
    void slotSaveSelectedPreset();
    void slotRevertSelectedPreset();
    void slotSaveNewPreset();
    void slotSaveEditBufferToLibrary(QString presetName);
    void slotSaveThenLoadPreset(QListWidgetItem *selectedItem);
    void slotLoadLibraryPresetIntoEditBuffer(QListWidgetItem *selectedItem);
    void slotSyncEditBufferToDevicePreset();
    void slotSyncEditBufferToDevice();
    void slotDeletePreset();
    QList<int> slotCheckPresetEdits();


    QString slotGetPresetNameFromIndex(int i);

    //-------- File I/O
    void slotLoadJSON();
    void slotWriteJSON(QVariantMap);

    //Remove individual presets from JSON - (Retains edited state in preset editable master map)
    void slotWritePresetToJSON(QVariantMap, int newOrOld);
    void slotRemovePresetFromJSON(int presetIndex);

    bool slotLoadPresetFileIntoEditBuffer(QString filename);

    void slotImportPreset();
    void slotExportPreset();

    //void slotSaveAndSend();

    void slotRevertToFactory();

    void slotImportFactory();

    void slotUpdateHighlightedPreset(QListWidgetItem*);
    
};

#endif // PRESETMANAGER_H
