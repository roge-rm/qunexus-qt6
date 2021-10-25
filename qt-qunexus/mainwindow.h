// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFontMetrics>
#include <QtGlobal>

// midi overhaul
#include "kmi_ports.h"
#include "KMI_mdm.h"
#include "RtMidi.h"
#include "KMI_DevData.h"
#include <fwupdate.h>

// qunexus
//#include "qunexusmidi.h" // renamed from mididevicemanager.h for midi overhaul
#include "sysexencdecode.h"
//#include "sysexdecode.h"
#include "presetmanager.h"
#include "sendpresetsdialog.h"
#include "syncqunexusdialog.h"

#include "tableinterface.h"
#include "cvtrimsinterface.h"

#include "ui_perkeysensitivities.h"

#include "kmi_updates.h"

#include "midi.h"

#ifdef Q_OS_MAC
    #define APP_WIDTH 850
    #define APP_HEIGHT 465
#else
    #define APP_WIDTH 850
    #define APP_HEIGHT 490
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QByteArray applicationVersion, thisFw;

    QString betaVersion;

    KMI_Updates * checkUpdates;

    // ------ midi overhaul --------------------------------------------------------

    // kmiPorts handles MIDI I/O changes
    KMI_Ports *kmiPorts;

    // create KMI devices
    MidiDeviceManager* QuNexus;

    // create a virtual port on MacOS, iOS, and Linux. Not supported on Windows.
#ifndef Q_OS_WIN
    MidiDeviceManager* virtualMidiPort;
#endif

    // MIDI aux inputs and outputs are defined here. For products like SoftStep, you would define 8 inputs for controllers
    // and one output for hosted mode. For other editors you would likely define one output port for to mirror the
    // incoming MIDI from the controller, as a workaround for Windows not sharing ports.
    // For KMI_Central we are using these for the input/output dropdowns as a simple MIDI route demo.
    MidiDeviceManager* midiAuxOut;

    // ------ qunexus ------------------------------------------------------------
    //QuNexusMidi*        qunexusMidi;
    PresetManager*      presetManager;
    SysExEncDecode*     sysExEncDecode;
    //SysExDecode*        sysExDecode;

    QListWidget*        libraryPresetsList;

    QListWidgetItem*    nextPresetItem;

    QCloseEvent         *editorCloseEvent;
    //----------------------------------- Stylesheets
    // General
    QFile*              generalStylesFile;
    QString             generalStylesString;

    // Parameter Labels
    QFile*              paramLabelStyleSheetFile;
    QString             paramLabelStyleString;

    // Header1
    QFile*              labelH1StyleSheetFile;
    QString             labelH1StyleString;

    // Header2
    QFile*              labelH2StyleSheetFile;
    QString             labelH2StyleString;

    // combo boxes
    QFile*              comboStyleFile;
    QString             comboStyleString;

    //Library
    QFile*              libraryStyleSheetFile;
    QString             libraryStyleSheetString;
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

    //Fonts
    QFont*  fontOpenSans;
    QFont*  fontAller;
    QFont*  fontDroid;

    // Palettes
    QPalette paletteTooltip;


    // Menu Bar
    QMenuBar*           menubar;
    QTabWidget*         tabWidget;
    QWidget*            controllerTab;
    QVariantMap         controllerElementCopiedMap;
    QVariantMap         copiedLayerMap;
    // Declare paste actions so they can be referred to in multiple scopes
    QAction*            pasteControllerSensor;
    QAction*            pasteControllerLayer;
    QAction*            pasteKeyboardLayer;
    QAction*            pasteCVLayer;
    // Declare midi related actions so they can be disabled when QuNexus is disconnected from another scope
    QAction* updateFirmware;

    QProgressDialog     *firmwareProgressDialog;

    QSpinBox*           onThresh;
    QSpinBox*           offThresh;

    int                 stateLastTab;
    int                 stateControllerTabVisible;
    int                 stateLastPreset;

    QSettings* settings;

    bool connected;
    bool firstRunUnsavedFlag;

    // menu items

    QMenu* loadHardwarePreset;
    QActionGroup* loadHardwarePresetGroup;
    QAction* loadPresetSlotA;
    QAction* loadPresetSlotB;
    QAction* loadPresetSlotC;
    QAction* loadPresetSlotD;


    QMenu* ledRefreshMode;
    QActionGroup* ledRefreshModeGroup;

    QAction* ledRefreshNormal;
    QAction* ledRefreshSilent;
    QAction* ledRefreshAllOff;

    QAction* perKeySensitivityAction;
    QWidget* pksWidget;

    fwUpdate* fwUpdateWindow;

    QAction* cvTrims;
    QAction* viewAndEditTables;

    TableInterface* tableInterface;
    CvTrimsInterface* cvTrimsInterface;
    SyncQuNexusDialog* syncQuNexusDialog;

    QTimer *globalsRequestIterator;

    QAction* toolTipsEnable;
    QAction* unsavedWarningsEnabled;

    void closeEvent(QCloseEvent *event); // handle save before quit

    // ------ midi overhaul --------------------------------------------------------

    // version strings for console and about window
    QString deviceBootloaderVersionString();
    QString deviceFirmwareVersionString();
    QString applicationFirmwareVersionString();

    // ------ end midi overhaul --------------------------------------------------------

protected:
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    bool eventFilter(QObject *obj, QEvent *event);


public slots:

    // ------ midi overhaul --------------------------------------------------------
    void slotMIDIPortChange(QString, uchar, uchar, int); // handles changes to MIDI i/o
    void slotRefreshConnection();
    void slotBootloaderMode(bool fwUpdateRequested);
    void slotFwUpdateSuccessCloseDialog(bool);
    void slotForceFirmwareUpdate();
    void slotFirmwareDetected(MidiDeviceManager *thisMDM, bool);
    void slotUpdateMIDIaux();

    // ------ end midi overhaul --------------------------------------------------------

    void    slotFindAndConnectUi();
    void    slotUpdateParametersUi(QVariantMap map);
    void    slotUpdateLibraryPresetsListUi(QVariantMap presetMaps, int action, int data);
    void    slotUpdateControllerLayerHighlightUi(QStringList activeKeys);

    void    slotSetupMenuBar();
    void    slotShowHideControllerTab();
    void    slotCopyControllerElements();
    void    slotPasteControllerElements();

    void    slotSetEndableUiElement(QString paramName, bool state);

    void    slotLimitSourceOutputMenus();

    void    slotCopyLayer();
    void    slotPasteLayer();

    // MIDI Overhaul - remove
    //void    slotFirmwareOutOfDateDialog(QString foundBoot, QString expectedFirmware, QString foundFirmware);
    //void    slotManualUpdateFirmwareRequestDialog();
    //void    slotFirmwareUpdateCompleteDialog();
    //void    slotProgressBar(QString messageType, int val);

    void    slotUnsavedChangesDialog(QListWidgetItem* newPresetItem);
    void    slotDiscardButton();
    void    slotSaveButton();

    void    slotSendPresetsToBoardDialog(QString presetName);
    void    slotSyncQuNexusDialog();
    void    slotOpenAboutScreen();
    void    slotOpenDocumentation();

    void    slotDeletePresetQuery();
    void    slotQuNexusConnected(bool);

    void    slotLimitThresholds();
    void    slotTabChanged(int index);

    void    slotLoadPresetSlot(QAction*);
    void    slotSetLedRefreshMode();
    void    slotEnableDisableMidiFunctions(bool connected);


    void    slotSetGlobals(QString, QByteArray globalByteArray);
    void    slotEncodeGlobals();
    void    slotResetPerKeySensitivities();

    void    slotToolTipHandler();

    void    slotEnableDisableToolTips();
    void    slotEnableDisableUnsavedWarnings();
    void    slotEnableDisableToggleParameters(bool toggled);
    void    slotEnableDisableToggleControl(bool);

    void    slotOpenPerKeySensitivitiesWindow();
    void    slotOpenTableInterface();
    void    slotOpenCvTrimsInterface();
    void    slotEnableGlobalsWindows();
    void    slotDisableGlobalsWindows();
    void    slotSendGlobalsRequest();

    void    slotRequestPreset(unsigned char preset);
    void    slotSendActiveSense();
    void    slotSyncQnToEditorNewPreset();
    void    slotSyncQnToEditor();
    void    slotSyncEditorToQn();
    //void    slotProcessPreset(QByteArray sysExMessageByteArray);
    void    slotPresetProcessed(unsigned char presetNum);
    void    slotDiscardAndCloseEditor();
    void    slotSaveAndCloseEditor();
    void    slotRevertPreset();

    void    slotDebugSlot();
    void    slotClearFocus();

    void    slotFixDropDownWidth(QComboBox* thisDropDown);


private:
    Ui::MainWindow *ui;
    Ui::PerKeySensitivities *sensitivitiesUi;
};



#endif // MAINWINDOW_H
