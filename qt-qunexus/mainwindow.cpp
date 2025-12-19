// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
/*
    -------------------------------------------------------------------
    -------------------------------------------------------------------

    QuNexus editor
    Originally Written by Conner Lacy - www.connerlacy.com | conner@connerlacy.com

    2.0 updates by Eric Bateman
    (c) Copyright 2024 KMI Music, Inc., all rights reserved.


    -------------------------------------------------------------------
    -------------------------------------------------------------------
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sendpresetsdialog.h"
#include "syncqunexusdialog.h"
#include "KMI_FwVersions.h"
#include "KMI_updates.h"
#include <qicon.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sensitivitiesUi(new Ui::PerKeySensitivities)
{   

    QCoreApplication::setApplicationName("QuNexus Editor");
    QCoreApplication::setOrganizationName("Keith McMillen Instruments");
    QCoreApplication::setOrganizationDomain("keithmcmillen.com");

    qDebug() << "System Locale: " << QLocale::system().name();

    // application version
    QString versionString = QString(APP_VERSION);

    // Split the version string by dots and assign values directly
    QStringList parts = versionString.split('.');

    for (int i = 0; i < 3 && i < parts.size(); ++i)
    {
        applicationVersion.append(static_cast<char>(parts[i].toInt()));
    }

    if (parts.size() > 3)
    {
        betaVersion = parts[3]; // assume this is a letter
    }
    else
    {
        betaVersion = "";
    }


    thisFw = QByteArray(reinterpret_cast<char*>(_fw_ver_qunexus), sizeof(_fw_ver_qunexus));


    // flag that the app has just started, avoid unsaved popup on open
    firstRunUnsavedFlag = true;

    //qApp->installEventFilter(this);     // catch all app mousemovements

    // ******************************
    // KMI_Ports
    // ******************************

    // kmiPorts reports changes in MIDI i/o
    kmiPorts = new KMI_Ports(this);

#ifndef Q_OS_WIN
    //kmiPorts->slotCreateVirtualIn("QuNexus Editor");
    //kmiPorts->slotCreateVirtualOut("QuNexus Editor");
#endif

    // connect kmiPorts to our handler
    connect(kmiPorts, SIGNAL(signalPortUpdated(QString, uchar, uchar, int)),
            this, SLOT(slotMIDIPortChange(QString, uchar, uchar, int)));
    connect(kmiPorts, SIGNAL(signalClearPortMaps()), this, SLOT(slotClearMIDIThruDropdown()));

    //qDebug() << "end connect";

    // ******************************
    // create KMI device handlers
    // ******************************

    QuNexus = new MidiDeviceManager(this, PID_QUNEXUS, "QuNexus", kmiPorts);

    QuNexus->fwSaveRestoreGlobals = true; // set flag to preserve globals during fw update

    // setup firmware image
    QString thisFwFile = QString(":/resources/firmware/QuNexus_Firmware_v%1.%2.%3.syx")
            .arg(uchar(thisFw.at(0)))
            .arg(uchar(thisFw.at(1)))
            .arg(uchar(thisFw.at(2)));

    //qDebug() << "fwFilename: " << thisFwFile;

    if (!QuNexus->slotOpenFirmwareFile(thisFwFile))
    {
        qDebug() << "ERROR - firmware file not found: " << thisFwFile;
    }

    // connect firmware signals
    qDebug() << "connect signalFirmwareDetected";

    // setup MIDI thru output
    midiTHRU = new MidiDeviceManager(this, PID_AUX, "MIDI Thru", kmiPorts);

    // ******************************
    // end KMI_Ports and device handlers
    // ******************************

    connected = false;
    forceFirmwareUpdate = false;

    settings = new QSettings();

    qDebug() << "Qsettings file name" << settings->fileName();

    // check for updates
    QString jsonVersionCheckURL = "https://files.keithmcmillen.com/products/qunexus/editor/softwareVersionCheck.json";
    checkUpdates = new KMI_Updates(this, "QuNexus", settings, applicationVersion, jsonVersionCheckURL);


    // default file location
    const QString DEFAULT_DIR_KEY("default_dir");

    //qDebug() << "Default file save location - pre: " << settings->value(DEFAULT_DIR_KEY).toString();

    // test if this is a directory
    QFileInfo check_file(settings->value(DEFAULT_DIR_KEY).toString());
    if (!check_file.exists() || !check_file.isDir() || settings->value(DEFAULT_DIR_KEY).toString().contains("Contents/MacOS"))
    {
        QString desktop = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
        qDebug() << "Desktop: " << desktop;
        settings->setValue(DEFAULT_DIR_KEY, desktop);     // if key doesn't exist, set it to desktop
    }

    //qDebug() << "Default file save location - post: " << settings->value(DEFAULT_DIR_KEY).toString();



    presetManager = new PresetManager(this);

    qDebug() << "init MIDI Device Manager";
    //qunexusMidi = new QuNexusMidi();

    sysExEncDecode = new SysExEncDecode();              // instantiates a new SysExEncDecode
    sysExEncDecode->QuNexus_MDM = QuNexus;              // class pointer
    sysExEncDecode->presetManager = presetManager;      // class pointer

//    qunexusMidi->SysExEncDecode = sysExEncDecode;   // share pointer to the same object
//    qunexusMidi->QuNexus_MDM = QuNexus;             // share pointer...


    // setup preset pointer for decoding sysex to edit buffer
    sysExEncDecode->editBufferPresetMap = &presetManager->editBufferPresetMap;

    presetManager->sysExEncDecode = sysExEncDecode; // setup shared pointers

    // ---- FONTS --------------------------
    qDebug() << "------------ [FONTS SETUP] ---------------------------------------------------";


    QString openSansSemibold = ":/fonts/open-sans/OpenSans-Semibold.ttf";
    QString openSansLight = ":/fonts/open-sans/OpenSans-Light.ttf";
    QString openSans3 = ":/fonts/open-sans/OpenSans-Regular.ttf";
    QString aller = ":/fonts/Aller/Aller_Lt.ttf";
    QString droidFont = ":/fonts/droid-sans/DroidSans.ttf";
    QString droidMonoFont = ":/fonts/droid-sans/DroidSansMono.ttf";
    QString futuraFont = ":/fonts/futura/futura-normal.ttf";
    QString futuraBFont = ":/fonts/futura/Futura-Bold.ttf";
    QString corbelFont = ":/fonts/corbel/corbel.ttf";
    QString corbelBFont = ":/fonts/corbel/corbelb.ttf";
    QString sourceFont = ":/fonts/source-sans-pro/SourceSansPro-Regular.otf";
    QString sourceCodePro1Font = ":/fonts/Source_Code_Pro/SourceCodePro-VariableFont_wght.ttf";
    QString sourceCodePro2Font = ":/fonts/Source_Code_Pro/SourceCodePro-Italic-VariableFont_wght.ttf";



    if (QFontDatabase::addApplicationFont(openSansSemibold) == -1) qDebug() << "Could not load font: " << openSansSemibold;
    if (QFontDatabase::addApplicationFont(openSansLight) == -1) qDebug() << "Could not load font: " << openSansLight;
    if (QFontDatabase::addApplicationFont(openSans3) == -1) qDebug() << "Could not load font: " << openSans3;
    if (QFontDatabase::addApplicationFont(aller) == -1) qDebug() << "Could not load font: " << aller;
    if (QFontDatabase::addApplicationFont(droidFont) == -1) qDebug() << "Could not load font: " << droidFont;
    if (QFontDatabase::addApplicationFont(droidMonoFont) == -1) qDebug() << "Could not load font: " << droidFont;
    if (QFontDatabase::addApplicationFont(futuraFont) == -1) qDebug() << "Could not load font: " << futuraFont;
    if (QFontDatabase::addApplicationFont(futuraBFont) == -1) qDebug() << "Could not load font: " << futuraBFont;
    if (QFontDatabase::addApplicationFont(corbelFont) == -1) qDebug() << "Could not load font: " << corbelFont;
    if (QFontDatabase::addApplicationFont(corbelBFont) == -1) qDebug() << "Could not load font: " << corbelBFont;
    if (QFontDatabase::addApplicationFont(sourceFont) == -1) qDebug() << "Could not load font: " << sourceFont;
    if (QFontDatabase::addApplicationFont(sourceCodePro1Font) == -1) qDebug() << "Could not load font: " << sourceCodePro1Font;
    if (QFontDatabase::addApplicationFont(sourceCodePro2Font) == -1) qDebug() << "Could not load font: " << sourceCodePro2Font;

    // ---- end FONTS -------------------------

    // general stylesheets
#ifdef Q_OS_MAC
    generalStylesFile = new QFile(":stylesheets/resources/stylesheets/GeneralStyles.qss");
    dialogStylesFile = new QFile(":/stylesheets/appDialog_QuNexus.qss");
#else
    generalStylesFile = new QFile(":stylesheets/resources/stylesheets/GeneralStylesWindows.qss");
    dialogStylesFile = new QFile(":/stylesheets/appDialog_QuNexus_WIN.qss");
#endif
    if (!generalStylesFile->open(QFile::ReadOnly))
    {
        qDebug() << "ERROR: could not open stylesheet: " << generalStylesFile->fileName();
    }
    else
    {
        generalStylesString = QLatin1String(generalStylesFile->readAll());
    }

    if (!dialogStylesFile->open(QFile::ReadOnly))
    {
        qDebug() << "ERROR: could not open stylesheet: " << dialogStylesFile->fileName();
    }
    else
    {
        dialogStylesString = QLatin1String(dialogStylesFile->readAll());
    }

    // Apply general styles to MainWindow. These can/should be previewed in designer
    QMainWindow::setStyleSheet(generalStylesString);


    // setup UI
    ui->setupUi(this);

    // MIDI Overhaul and FW Update

    // connect dropdowns and connection status to MIDI thru ports
    connect(ui->midi_outputs, SIGNAL(activated(int)), this, SLOT(slotUpdateMIDIThru()));
    connect(QuNexus, SIGNAL(signalConnected(bool)), this, SLOT(slotUpdateMIDIThru()));

    // remember last selected MIDI thru port
    MIDI_THRU_KEY = "midi_thru_port";

    qDebug() << "connected thru port";

    // Firmware update Window
    fwUpdateWindow = new fwUpdate(this, "QuNexus", applicationFirmwareVersionString());
    fwUpdateWindow->setStyleSheet(generalStylesString);

    // Troubleshooting Window
    troubleshootWindow = new troubleshoot(this, "QuNexus", applicationFirmwareVersionString());
    //troubleshootWindow->setStyleSheet(generalStylesString);
    troubleshootWindow->hide();

    connect(kmiPorts, SIGNAL(signalInputCount(int)), troubleshootWindow, SLOT(slotInputCount(int)));
    connect(kmiPorts, SIGNAL(signalOutputCount(int)), troubleshootWindow, SLOT(slotOutputCount(int)));
    connect(kmiPorts, SIGNAL(signalInputPort(QString, int)), troubleshootWindow, SLOT(slotInputPort(QString, int)));
    connect(kmiPorts, SIGNAL(signalOutputPort(QString, int)), troubleshootWindow, SLOT(slotOutputPort(QString, int)));

    // end MIDI Overhaul

    //Per Key Sensitivities
    pksWidget = new QWidget(this);
    pksWidget->setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    pksWidget->setFixedSize(911,258);
    sensitivitiesUi->setupUi(pksWidget);
    pksWidget->setStyleSheet("QLabel {color: rgb(50,50,50); border-radius: 1px;}");
    sensitivitiesUi->tabWidget->setCurrentIndex(0);
    sensitivitiesUi->tabWidget->setTabEnabled(1, false);

    //Table Interface
    tableInterface = new TableInterface;
    cvTrimsInterface = new CvTrimsInterface;

    //dialogs
    syncQuNexusDialog = new SyncQuNexusDialog(this);
    syncQuNexusDialog->setModal(false);
    connect(syncQuNexusDialog, SIGNAL(signalCloseWindow()), this, SLOT(slotHideGreyRect()));

#ifndef Q_OS_MAC

#define MENU_BAR_HEIGHT 24
#define MENU_BAR_OFFSET MENU_BAR_HEIGHT - 6

    qDebug() << "Windows only - move widgets down to make room for menu bar";

    QLabel *libraryPaneLabel = this->findChild<QLabel *>("Library_Pane_Label");
    libraryPaneLabel->move(libraryPaneLabel->x(), libraryPaneLabel->y() + MENU_BAR_OFFSET);
    QFrame *headerFrame = this->findChild<QFrame *>("Header_Frame");
    headerFrame->move(headerFrame->x(), headerFrame->y() + MENU_BAR_OFFSET);
    QFrame *presetFrame = this->findChild<QFrame *>("Preset_Frame");
    presetFrame->move(presetFrame->x(), presetFrame->y() + MENU_BAR_OFFSET);
    QFrame *libraryFrame = this->findChild<QFrame *>("Library_Frame");
    libraryFrame->move(libraryFrame->x(), libraryFrame->y() + MENU_BAR_OFFSET);
    QFrame *sendFrame = this->findChild<QFrame *>("Send_Frame");
    sendFrame->move(sendFrame->x(), sendFrame->y() + MENU_BAR_OFFSET);

#endif

    qDebug() << "non-os specific stylesheets";

    //StyleSheets for "blue" (red) pushbuttons
    blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    blueStyleString = QLatin1String(blueStyleFile->readAll());

    //StyleSheets for "grey" pushbuttons
    grayStyleFile = new QFile(":stylesheets/resources/stylesheets/GrayButtonStyleSheet.qss");
    grayStyleFile->open(QFile::ReadOnly);
    grayStyleString = QLatin1String(grayStyleFile->readAll());

    //Tab Widget Handles
    tabWidget = this->findChild<QTabWidget *>("tabWidget");

    //Get Controller Tab Handle for Showing/Hiding
    controllerTab = tabWidget->widget(1);

    slotSetupMenuBar();
    slotFindAndConnectUi();


    //-------------------------------------------------------------- Initialize Library GUI --------------------------------------------------------------//
    //Put the presets in the library
    qDebug() << "Initialize Library UI";
    slotUpdateLibraryPresetsListUi(presetManager->jsonMasterMapEDITABLE, CREATE_PRESET, 0);

    //If no board presets are found yet, set to -1
    for(int i = 0; i < 4; i++)
    {
        if(!settings->contains(QString("boardPreset%1").arg(i)))
        {
            settings->setValue(QString("boardPreset%1").arg(i), -1);
        }
    }

    //Loads edit buffer into display parameters
    slotUpdateParametersUi(presetManager->editBufferPresetMap);
    slotUpdateControllerLayerHighlightUi(presetManager->controllerLayerSelectedComponents);

    //Setup Threshold Limiting - must occur after ui initialization
    onThresh = this->findChild<QSpinBox *>(QString("Globals_On_Thresh"));
    offThresh = this->findChild<QSpinBox *>(QString("Globals_Off_Thresh"));

    onThresh->setMinimum(offThresh->value() + 1);
    offThresh->setMaximum(onThresh->value() - 1);
    offThresh->setMaximum(onThresh->value() - 1);
    onThresh->setMinimum(offThresh->value() + 1);

    connect(onThresh, SIGNAL(editingFinished()), this, SLOT(slotLimitThresholds()));
    connect(offThresh, SIGNAL(editingFinished()), this, SLOT(slotLimitThresholds()));

    slotToolTipHandler();

    qDebug() << "--------------------- selected Preset" << presetManager->selectedLibraryPreset;

    // connect preset warnings
    connect(presetManager, SIGNAL(signalCallUnsavedChangesDialog(QListWidgetItem*)), this, SLOT(slotUnsavedChangesDialog(QListWidgetItem*)));

    // load preset 0
    presetManager->slotLoadLibraryPresetIntoEditBuffer(libraryPresetsList->item(0));
    libraryPresetsList->setCurrentItem(libraryPresetsList->item(0));

    // Select preset in ui
    presetManager->presetIsDirty = 0;
    slotUpdateLibraryPresetsListUi(QVariantMap(), LOAD_PRESET, 0);

    qDebug() << "Connect signalUpdateLibraryPresetsUi";
    // EB Debug
    connect(presetManager, SIGNAL(signalUpdateLibraryPresetsUi(QVariantMap, int, int)), this, SLOT(slotUpdateLibraryPresetsListUi(QVariantMap, int, int))); //Library Window

    // poll times for each OS
    int thisPollTime = 100; // MacOS can poll slower
#ifdef Q_OS_WIN
    // Windows needs to poll fast, we have to catch the disconnects during fw
    // updates because QuNexus ports are reported the same wether they are
    // old ("port 1") or new ("QuNexus Control Surface").
    thisPollTime = 1;
#endif

    kmiPorts->devicePoller->start(thisPollTime);

    // fix windows that are not on the screen
    int posX = this->mapToGlobal(QPoint(0,0)).x();
    int posY = this->mapToGlobal(QPoint(0,0)).y();

    qDebug() << "thisPosition - x: " << posX << " y: " << posY;
//    if (posY < 0)
//    {
//        posY = 0;
//    }

    this->move(posX+1, posY+1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#define DIALOG_WIDTH 400
#define DIALOG_HEIGHT 125
#define DIALOG_TEXT_PADDING 10
#define DIALOG_W_CENTER (DIALOG_WIDTH / 2)
#define DIALOG_BUTT_W 70
#define DIALOG_BUTT_H 28
#define DIALOG_BUTT_X DIALOG_W_CENTER - (DIALOG_BUTT_W / 2)
#define DIALOG_BUTT_Y DIALOG_HEIGHT - DIALOG_BUTT_H - (DIALOG_TEXT_PADDING * 2)
#define DIALOG_TEXT_W (DIALOG_WIDTH - (DIALOG_TEXT_PADDING * 2))
#define DIALOG_TEXT_H (DIALOG_HEIGHT - DIALOG_BUTT_H - (DIALOG_TEXT_PADDING * 2))

void MainWindow::slotCreateDialog(QString dialogText)
{
    QDialog *msgBox = new QDialog(this);
    msgBox->setModal(true);
    msgBox->setWindowFlags(Qt::FramelessWindowHint);
    msgBox->setStyleSheet(dialogStylesString);


    msgBox->setMinimumSize(DIALOG_WIDTH, DIALOG_HEIGHT);
    msgBox->setFixedSize(DIALOG_WIDTH, DIALOG_HEIGHT);

    int x = this->width();
    int y = this->height();

    int dialogX = ((x / 2) - (DIALOG_WIDTH / 2));
    int dialogY = ((y / 2) - (DIALOG_HEIGHT / 2));

    qDebug() << "parent x: " << x << " y: " << y << " dialogX: " << dialogX << "dialogY: " << dialogY;

    msgBox->move(dialogX, dialogY);

    QLabel* text = new QLabel(dialogText, msgBox, Qt::WindowFlags());
    text->setAlignment(Qt::AlignCenter);
    text->setMinimumSize(DIALOG_TEXT_W, DIALOG_TEXT_H);
    text->setFixedSize(DIALOG_TEXT_W, DIALOG_TEXT_H);
    text->move(DIALOG_TEXT_PADDING, DIALOG_TEXT_PADDING);

    QPushButton* okButton = new QPushButton(msgBox);
    okButton->setStyleSheet(grayStyleString);
    okButton->setText("Ok");
    okButton->setGeometry(QRect(DIALOG_BUTT_X, DIALOG_BUTT_Y, DIALOG_BUTT_W,DIALOG_BUTT_H));
    connect(okButton, SIGNAL(clicked()), msgBox, SLOT(close()));

    msgBox->exec();
}

void MainWindow::slotFindAndConnectUi()
{
    //----------------------------------------------------------------------------------------//
    //----------------------------------Default Param List------------------------------------//
    //----------------------------------------------------------------------------------------//

    //--------------- JSON Items
    QMapIterator<QString, QVariant> i(presetManager->defaultParamMap);

    while(i.hasNext())
    {
        //qDebug() << "Next...";
        i.next();

        //Look for Keyboard Params
        //qDebug() << "ThisKey: " << i.key();

        if(i.key().startsWith("Keyboard") || i.key().startsWith("SEQ") )
        {

            //qDebug() << "MAPS" << i.key() << ":" << i.value();

            if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
            {
                //qDebug() << "QComboBox" << menu->objectName();
                connect(menu, SIGNAL(currentIndexChanged(int)), presetManager, SLOT(slotValueChanged()));
                slotFixDropDownWidth(menu);

            }
            else if(QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key()))
            {
                //qDebug() << "QSpinBox" << numberBox->objectName();
                connect(numberBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
            {
                //qDebug() << "QDoubleSpinBox" << floatBox->objectName();
                connect(floatBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QCheckBox* checkBox = this->findChild<QCheckBox *>(i.key()))
            {
                //qDebug() << "QDoubleSpinBox" << floatBox->objectName();
                connect(checkBox, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));
            }
            else
            {
                //------------ Handle Special Keyboard Params: ----------//
                //qDebug() << "Did not find: " << i.key();
                //
                if(i.key() == "Keyboard_Global_LED_Mode")
                {



                }
            }
        }

        //Look for Controller Params (this just filters them out)
        else if(i.key().startsWith("Controller"))
        {
            //qDebug() << "*** Controller Param *** :" << i.key();
        }

        //Look for CV Params
        else if(i.key().startsWith("CV"))
        {
            //qDebug() << "*** CV Param *** :" << i.key();
            if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
            {
                //qDebug() << "QComboBox" << menu->objectName();
                connect(menu, SIGNAL(currentIndexChanged(int)), presetManager, SLOT(slotValueChanged()));
                slotFixDropDownWidth(menu);
            }
            else if(QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key()))
            {
                //qDebug() << "QSpinBox" << numberBox->objectName();
                connect(numberBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
            {
                //qDebug() << "QDoubleSpinBox" << floatBox->objectName();
                connect(floatBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QCheckBox* checkBox = this->findChild<QCheckBox *>(i.key()))
            {
                connect(checkBox, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));
            }
            else
            {
                //qDebug() << "*** CV Param *** :" << i.key();
            }
        }

        //Look for Globals Params
        else if(i.key().startsWith("Globals"))
        {
            if(QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key()))
            {
                //qDebug() << "QComboBox" << menu->objectName();
                connect(numberBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
            {
                connect(floatBox, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
            }
            else if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
            {
                connect(menu, SIGNAL(currentIndexChanged(int)), presetManager, SLOT(slotValueChanged()));
                slotFixDropDownWidth(menu);
            }
            else
            {
                qDebug() << "*** Globals Param Not Fount in UI *** :" << i.key();
            }
        }

        //Not Found in UI / Extras
        else
        {
            //qDebug() << "*** Not Found in UI / Others *** :" << i.key();
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------------------------//
    //------------------------------------------------------------ Controller Edit Pane Params ---------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------------------------------------------------//

    QComboBox* controllerOutputDevice = this->findChild<QComboBox *>("Controller_Global_MIDI_Output_Device");
    connect(controllerOutputDevice, SIGNAL(currentIndexChanged(int)), presetManager, SLOT(slotValueChanged()));
    slotFixDropDownWidth(controllerOutputDevice);

    //------------------------------------------------------------------ Keys --------------------------------------------------------------------------//
    //Channel
    QSpinBox* controllerKeyChannel = this->findChild<QSpinBox *>("Controller_Key_Channel");
    connect(controllerKeyChannel, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Layer Enable/Disable
    QCheckBox* enableKeyController = this->findChild<QCheckBox *>("Controller_Key_Controller_Layer_Enable_Disable");
    connect(enableKeyController, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));
    QCheckBox* enableKeyKeyboard = this->findChild<QCheckBox *>("Controller_Key_Keyboard_Layer_Enable_Disable");
    connect(enableKeyKeyboard, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));
    connect(enableKeyKeyboard, SIGNAL(toggled(bool)), this, SLOT(slotEnableDisableToggleControl(bool)));

    //Note
    QSpinBox* controllerKeyNoteNumber = this->findChild<QSpinBox *>("Controller_Key_Note_Number");
    connect(controllerKeyNoteNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerKeyNoteVelocity = this->findChild<QSpinBox *>("Controller_Key_Note_Velocity");
    connect(controllerKeyNoteVelocity, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Pressure
    QSpinBox* controllerKeyPressureControlNumber = this->findChild<QSpinBox *>("Controller_Key_Pressure_Control_Number");
    connect(controllerKeyPressureControlNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerKeyPressureReturnValue = this->findChild<QSpinBox *>("Controller_Key_Pressure_Return_Value");
    connect(controllerKeyPressureReturnValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Tilt
    QSpinBox* controllerKeyTiltControlNumber = this->findChild<QSpinBox *>("Controller_Key_Tilt_Control_Number");
    connect(controllerKeyTiltControlNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerKeyTiltReturnValue = this->findChild<QSpinBox *>("Controller_Key_Tilt_Return_Value");
    connect(controllerKeyTiltReturnValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Toggle
    QSpinBox* controllerKeyToggleControlNumber = this->findChild<QSpinBox *>("Controller_Key_Toggle_Control_Number");
    connect(controllerKeyToggleControlNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerKeyToggleValue = this->findChild<QSpinBox *>("Controller_Key_Toggle_Value");
    connect(controllerKeyToggleValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerKeyToggleReturnValue = this->findChild<QSpinBox *>("Controller_Key_Toggle_Return_Value");
    connect(controllerKeyToggleReturnValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QCheckBox* enableKeyControllerToggle = this->findChild<QCheckBox *>("Controller_Key_Toggle_On_Off");
    connect(enableKeyControllerToggle, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));
    connect(enableKeyControllerToggle, SIGNAL(toggled(bool)), this, SLOT(slotEnableDisableToggleParameters(bool)));


    //------------------------------------------------------------------ Bend Pad --------------------------------------------------------------------------//
    //Channel
    QSpinBox* controllerBendChannel = this->findChild<QSpinBox *>("Controller_Bend_Pad_Channel");
    connect(controllerBendChannel, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Layer Enable/Disable
    QCheckBox* enableBendController = this->findChild<QCheckBox *>("Controller_Bend_Pad_Controller_Layer_Enable_Disable");
    connect(enableBendController, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));

    QCheckBox* enableBendKeyboard = this->findChild<QCheckBox *>("Controller_Bend_Pad_Keyboard_Layer_Enable_Disable");
    connect(enableBendKeyboard, SIGNAL(stateChanged(int)), presetManager, SLOT(slotValueChanged()));

    //Note
    QSpinBox* controllerBendNoteNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Note_Number");
    connect(controllerBendNoteNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerBendNoteVelocity = this->findChild<QSpinBox *>("Controller_Bend_Pad_Note_Velocity");
    connect(controllerBendNoteVelocity, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Pressure
    QSpinBox* controllerBendPressureControlNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Pressure_Control_Number");
    connect(controllerBendPressureControlNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerBendPressureReturnValue = this->findChild<QSpinBox *>("Controller_Bend_Pad_Pressure_Return_Value");
    connect(controllerBendPressureReturnValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));

    //Tilt
    QSpinBox* controllerBendTiltControlNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Tilt_Control_Number");
    connect(controllerBendTiltControlNumber, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));
    QSpinBox* controllerBendTiltReturnValue = this->findChild<QSpinBox *>("Controller_Bend_Pad_Tilt_Return_Value");
    connect(controllerBendTiltReturnValue, SIGNAL(editingFinished()), presetManager, SLOT(slotValueChanged()));


    //------------------------------------------------------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------- UI Saving/Recalling ------------------------------------------------------------------//
    //------------------------------------------------------------------------------------------------------------------------------------------------------//

    //Save Edit Buffer to Library
    QPushButton* newPreset = this->findChild<QPushButton *>("newPreset");
    connect(newPreset, SIGNAL(clicked()), presetManager, SLOT(slotSaveNewPreset()));

    QPushButton* deletePreset = this->findChild<QPushButton *>("deletePreset");
    connect(deletePreset, SIGNAL(clicked()), this, SLOT(slotDeletePresetQuery()));

    //Save Edit Buffer to Library
    QPushButton* revertButton = this->findChild<QPushButton *>("revertButton");
    connect(revertButton, SIGNAL(clicked()), this, SLOT(slotRevertPreset()));
    // initial disable
    revertButton->setEnabled(false);

    QPushButton* saveButton = this->findChild<QPushButton *>("saveButton");
    connect(saveButton, SIGNAL(clicked()), presetManager, SLOT(slotSaveSelectedPreset()));
    // initial disable
    saveButton->setEnabled(false);

    //Needs to be class member because its display is dynamic
    //saveAndSend = this->findChild<QPushButton *>("saveAndSend");
    //connect(saveAndSend, SIGNAL(clicked()), presetManager, SLOT(slotSaveAndSend()));

    //Update UI Signal/Slot
    connect(presetManager, SIGNAL(signalUpdateParameterUi(QVariantMap)), this, SLOT(slotUpdateParametersUi(QVariantMap)));              //Parameters
    connect(presetManager, SIGNAL(signalSetEnableUiParam(QString,bool)), this, SLOT(slotSetEndableUiElement(QString,bool)));            //Enable disable ui param
    connect(presetManager, SIGNAL(signalCheckChannelOverlap()), this, SLOT(slotCheckChannelOverlap()));
    //Rename Preset Button
    //QPushButton* renamePresetButton = this->findChild<QPushButton *>("renamePresetButton");
    //connect(renamePresetButton, SIGNAL(clicked()), presetManager, SLOT(slotLoadLibraryPresetIntoEditBuffer(QListWidgetItem*)));

    //Library Presets Window
    libraryPresetsList = this->findChild<QListWidget *>("presetLibraryList");

    // double click loads the preset
    connect(libraryPresetsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), presetManager, SLOT(slotLoadLibraryPresetIntoEditBuffer(QListWidgetItem*)));

    // single click highlights the preset
    connect(libraryPresetsList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), presetManager, SLOT(slotUpdateHighlightedPreset(QListWidgetItem*)));


    //------------------------------------------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------- UI Device Preset Syncing -------------------------------------------------------------//
    //------------------------------------------------------------------------------------------------------------------------------------------------------//

    for(int i=0; i<4; i++)
    {
        QPushButton* syncButton = this->findChild<QPushButton *>(QString("syncPreset%1").arg(i));
        connect(syncButton, SIGNAL(clicked()), presetManager, SLOT(slotSyncEditBufferToDevicePreset()));
    }

    //-------------- MIDI & SysEx Preset/Globals Encoding
    connect(presetManager, SIGNAL(signalSysExEncodePreset(QVariantMap, unsigned char)), sysExEncDecode, SLOT(slotSysExEncodePreset(QVariantMap, unsigned char)));    //Sends preset to be encoded
    connect(sysExEncDecode, SIGNAL(signalEncodedPreset(QByteArray)), QuNexus, SLOT(slotSendSysExBA(QByteArray)));        //Sends encoded preset to device
    connect(sysExEncDecode, SIGNAL(signalEncodedGlobal(QByteArray)), QuNexus, SLOT(slotSendSysExBA(QByteArray)));        //Sends encoded global

    //--------------- Controller Key Selection
    for(int i=0; i<25; i++)
    {
        QPushButton* keyHighlightButton = this->findChild<QPushButton *>(QString("Key_Highlight_%1").arg(i));
        connect(keyHighlightButton, SIGNAL(pressed()), presetManager, SLOT(slotSelectControllerLayerKey()));
        keyHighlightButton->setStyleSheet(  "border:2px solid rgba(10, 255, 0, 255);"
                                            "border-radius: 3px;"
                                            "border-bottom-left-radius:8.0;"
                                            "border-bottom-right-radius:8.0;"
                                            "border-top-left-radius:8.0;"
                                            "border-top-right-radius:8.0;");
    }

    QPushButton* bendHighlightButton = this->findChild<QPushButton *>(QString("Bend_Highlight"));
    connect(bendHighlightButton, SIGNAL(pressed()), presetManager, SLOT(slotSelectControllerLayerKey()));
    bendHighlightButton->setStyleSheet(  "border:2px solid rgba(10, 255, 0, 255);"
                                         "border-radius: 3px;"
                                         "border-bottom-left-radius:8.0;"
                                         "border-bottom-right-radius:8.0;"
                                         "border-top-left-radius:8.0;"
                                         "border-top-right-radius:8.0;");

    connect(presetManager, SIGNAL(signalControlLayerSelectedComponents(QStringList)), this, SLOT(slotUpdateControllerLayerHighlightUi(QStringList)));


    //----------- Output-based Source Menu Filtering
    connect(presetManager, SIGNAL(signalLimitSourceOutputMenus()), this, SLOT(slotLimitSourceOutputMenus()));


    // ----------------
    // MIDI Overhaul
    //----------------- Firmware Out of Date Warning, Update Progress Bar, Update Complete


    // connect firmware detection
    connect(QuNexus, SIGNAL(signalFirmwareDetected(MidiDeviceManager*, bool)), this, SLOT(slotFirmwareDetected(MidiDeviceManager*, bool)));

    // connect firmware update window and midi device manager controls and messaging
    connect(fwUpdateWindow, SIGNAL(signalRequestFwUpdate()), QuNexus, SLOT(slotRequestFirmwareUpdate()));                       // request fw
    connect(QuNexus, SIGNAL(signalFwConsoleMessage(QString)), fwUpdateWindow, SLOT(slotAppendTextToConsole(QString)));          // messaging
    connect(QuNexus, SIGNAL(signalFwProgress(int)), fwUpdateWindow, SLOT(slotUpdateProgressBar(int)));                          // console
    connect(QuNexus, SIGNAL(signalFirmwareUpdateComplete(bool)), fwUpdateWindow, SLOT(slotFwUpdateComplete(bool)));             // Update Complete
    connect(fwUpdateWindow, SIGNAL(signalFwUpdateSuccess()), QuNexus, SLOT(slotFirmwareUpdateReset()));                         // stop timeout timers
    connect(fwUpdateWindow, SIGNAL(signalFwUpdateSuccessCloseDialog(bool)), this, SLOT(slotFwUpdateSuccessCloseDialog(bool)));  // close fw dialog and connect
    connect(QuNexus, SIGNAL(signalRequestGlobals()), this, SLOT(slotSendGlobalsRequest()));                                     // request globals
    connect(sysExEncDecode, SIGNAL(signalGlobalsReceived()), this, SLOT(slotEnableGlobalsWindows()));                           // enable globals window when we receive this
    connect(QuNexus, SIGNAL(signalRestoreGlobals()), this, SLOT(slotEncodeGlobals()));                                          // restore the globals after fw update

    // connect fwUpdate console messages to connection troubleshooter
    connect(QuNexus, SIGNAL(signalFwConsoleMessage(QString)), troubleshootWindow, SLOT(slotAppendToStatusLog(QString)));
    connect(fwUpdateWindow, SIGNAL(signalRequestFwUpdate()), troubleshootWindow, SLOT(slotRequestFwUpdate()));
    connect(QuNexus, SIGNAL(signalFirmwareUpdateComplete(bool)), troubleshootWindow, SLOT(slotFirmwareUpdated(bool)));

    // handle device unexpectedly in bootloader mode
    connect(QuNexus, SIGNAL(signalBootloaderMode(bool)), this, SLOT(slotBootloaderMode(bool)));

    //----------------- Send Presets To Board Dialog
    connect(presetManager, SIGNAL(signalSendPresetsToBoardDialog(QString)), this, SLOT(slotSendPresetsToBoardDialog(QString)));

    //---------------------------------- Tab Widget
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));

    //---------------------------------- Connected Light
    // MIDI overhaul
    connect(QuNexus, SIGNAL(signalConnected(bool)), this, SLOT(slotQuNexusConnected(bool)));
    connect(QuNexus, SIGNAL(signalConnected(bool)), this, SLOT(slotEnableDisableMidiFunctions(bool)));

    //---------------------------------- Globals
    connect(sysExEncDecode, SIGNAL(signalSendGlobals(QString,QByteArray)), this, SLOT(slotSetGlobals(QString,QByteArray)));
    QPushButton* resetPerKeySensitivities = pksWidget->findChild<QPushButton *>(QString("ResetPerKeySensitivities"));
    connect(resetPerKeySensitivities, SIGNAL(clicked()), this, SLOT(slotResetPerKeySensitivities()));

    seqDimKeysSetting = this->findChild<QSpinBox *>("SEQ_dim_keys");
    seqBrightKeysSetting = this->findChild<QSpinBox *>("SEQ_bright_keys");

    connect( seqDimKeysSetting, SIGNAL(editingFinished()), this, SLOT(slotEncodeGlobals()) ); // send globals when changed
    connect( seqBrightKeysSetting, SIGNAL(editingFinished()), this, SLOT(slotEncodeGlobals()) ); // send globals when changed

    //---------------------------------- Setup Active Sensing for QuNexus -> editor realtime updates
    QTimer *asTimer = new QTimer(this);
    connect(asTimer, SIGNAL(timeout()), this, SLOT(slotSendActiveSense())); // this lives in sysExEncode to avoid interrupting sysex
    asTimer->start(300); // send every 300ms per MIDI spec

    connect(sysExEncDecode, SIGNAL(signalPresetProcessed(unsigned char)), this, SLOT(slotPresetProcessed(unsigned char)));

    // this fixes per key sensitivity slider numbers
    QSlider* slider;

    for(int i=1; i < 26; i++)
    {
        if(i < 10)
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_0%1").arg(i));
        }
        else
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_%1").arg(i));
        }

        connect(slider, SIGNAL(sliderReleased()), this, SLOT(slotEncodeGlobals()));
    }

    connect(tableInterface, SIGNAL(signalSendUserTables()), this, SLOT(slotEncodeGlobals()));
    connect(cvTrimsInterface, SIGNAL(signalSendToBoard()), this, SLOT(slotEncodeGlobals()));
    connect(sysExEncDecode, SIGNAL(signalRestoreGlobals()), this, SLOT(slotEncodeGlobals()));

    // start the app with preset 0 loaded

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// UI Graphics & Controls //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::slotUpdateParametersUi(QVariantMap map)
{
    qDebug() << "slotUpdateParametersUi called";
    QMapIterator<QString, QVariant> i(map);

    while(i.hasNext())
    {
        i.next();

        //qDebug() << "Key/Value" << i.key() << i.value();

        if(i.key().startsWith("Controller"))
        {

//            if (i.key() == "Controller_Global_MIDI_Output_Device")
//            {
//                qDebug() << "*** update Controller_Global_MIDI_Output_Device - val:" << i.value();
//            }

            //Filter out controller layer params, except controller destinatino
            if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
            {
                int textIndex = menu->findText(i.value().toString());
                menu->setCurrentIndex(textIndex);
            }

        }
        else
        {
            //----- If the param is not in the special list, process normally
            if(!presetManager->specialProcessingParamList.contains(i.key()))
            {

                if(i.key() == "Globals_LED_Refresh_Style")
                {
                    //qDebug() << i.key() << i.value().toString();

                    if(i.value().toString() == "Normal")
                    {
                        ledRefreshNormal->setChecked(true);
                    }
                    else if(i.value().toString() == "Control Only")
                    {
                        ledRefreshSilent->setChecked(true);
                    }
                    else if(i.value().toString() == "All Off")
                    {
                        ledRefreshAllOff->setChecked(true);
                    }
                }
                if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
                {
                    int textIndex = menu->findText(i.value().toString());
                    menu->setCurrentIndex(textIndex);
                    //qDebug() << "textIndex: " << textIndex;
                }
                else if(QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key()))
                {
                    numberBox->setValue(i.value().toInt());

                }
                else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
                {    
                    floatBox->setValue(i.value().toDouble());
                }
                else
                {
                    //qDebug() << "*** Update UI Element Failed *** :" << i.key();
                }
            }

            //----- If in the special list...
            else
            {
                //qDebug() << "Special Param:" << i.key();

                //Gain & Spin Boxes
                if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
                {
                    if(i.key() == "Keyboard_Global_Bend_Pad_Sensitivity" || "Keyboard_Global_Pressure_Sensitivity")
                    {
                        //qDebug() << "FIX: " << i.key() << " value: " << i.value().toInt();

                        if (i.value().toInt() < 25) // filter out leftver bad values from pre 2.0.2 firmware
                        {
                            floatBox->setValue(1.00);
                            //qDebug() << "setValue 1.0";
                        }
                        else
                        {
                           floatBox->setValue(i.value().toDouble()/100.00);
                           //qDebug() << "setvalue: " << i.value().toDouble();
                        }
                    }
                    else
                    {
                        //Divide by 100 to make X.00
                        floatBox->setValue(i.value().toDouble()/100.00);
                    }
                }

                //If Checkboxes in Keyboard Mode
                else if(i.key() == "Keyboard_Global_Channel_Rotation_Active")
                {
                    QCheckBox *checkBox = this->findChild<QCheckBox *>(i.key());

                    if(i.value().toInt() > 0)
                    {
                        checkBox->setChecked(true);
                    }
                    else
                    {
                        checkBox->setChecked(false);
                    }
                }
                //If Checkboxes in Keyboard Mode
                else if(i.key() == "Keyboard_Global_LED_Mode")
                {
                    QCheckBox *checkBox = this->findChild<QCheckBox *>(i.key());

                    if(i.value().toInt() > 0)
                    {
                        checkBox->setChecked(true);
                    }
                    else
                    {
                        checkBox->setChecked(false);
                    }
                }

                //If CV Out MIDI Input Types
                else if(i.key().startsWith("CV_Out_CV") && i.key().endsWith("MIDI_Input_Type"))
                {
                    QComboBox *comboBox = this->findChild<QComboBox *>(i.key());

                    comboBox->setCurrentIndex(comboBox->findText(i.value().toString().remove(QString(" CV Out"), Qt::CaseSensitive)));

//                    qDebug() << "Key:" << i.key() << "Value:" << comboBox->currentText();
                }

                //CV Gate Articulation
                else if(i.key().contains("CV_Gate_Articulation"))
                {
                    QCheckBox *checkBox = this->findChild<QCheckBox *>(i.key());

                    if(i.value().toInt() > 0)
                    {
                        checkBox->setChecked(true);
                    }
                    else
                    {
                        checkBox->setChecked(false);
                    }
                }
                //CV Gate Articulation
                else if(i.key().contains("CV_Gate_S_Trig"))
                {
                    QCheckBox *checkBox = this->findChild<QCheckBox *>(i.key());

                    if(i.value().toInt() > 0)
                    {
                        checkBox->setChecked(true);
                    }
                    else
                    {
                        checkBox->setChecked(false);
                    }
                }

                //Channel Offsets
                else if(i.key().endsWith("Channel"))
                {
                    QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key());
                    numberBox->setValue(i.value().toInt() + 1);
                }
                else if(i.key().contains("Keyboard_Global_Pitch_Bend_Range"))
                {
                    QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key());
                    numberBox->setValue(i.value().toInt());
                }
                else if(i.key().contains("Globals_Tilt_Sensitivity"))
                {
                    QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key());
                    numberBox->setValue(127 - i.value().toInt());
//                    qDebug() << "graphic value" << numberBox->value();
                }
            }
        }
    }
}

void MainWindow::slotLimitSourceOutputMenus()
{
    //qDebug() << "slotLimitSourceOutputMenus called";
    QList<QString> sourceStrings;
    sourceStrings.append("Keyboard_Pitch_Bend_Source");
    sourceStrings.append("Keyboard_Channel_Pressure_Source");
    sourceStrings.append("Keyboard_Poly_Aftertouch_Source");
    sourceStrings.append("Keyboard_CC_00_Source");
    sourceStrings.append("Keyboard_CC_01_Source");
    sourceStrings.append("Keyboard_CC_02_Source");

    QComboBox *currentSourceMenu;

    QString activeTiltDestination;
    QString activePressureDestination;
    QString activeExpressionDestination;

    bool    tiltFlag = false;;
    bool    pressureFlag = false;;
    bool    exprPedalFlag = false;;

    QVariant disableVariant(0);
    QVariant enableVariant(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QModelIndex currentModelIndex;

    //---------------------------------------------------------- Check Flags ----------------------------------------------------------//
    // Find source menus with assignments
    for(int i =0; i < sourceStrings.count(); i++)
    {
        currentSourceMenu = this->findChild<QComboBox *>(sourceStrings.at(i));

        //Tilt Source
        if(currentSourceMenu->currentText() == "Tilt")
        {
            //qDebug() << "Tilt  Flag";
            tiltFlag = true;
            activeTiltDestination = sourceStrings.at(i);
        }

        //Pressure Source
        else if(currentSourceMenu->currentText() == "Pressure")
        {
            //qDebug() << "Pressure Flag";
            pressureFlag = true;
            activePressureDestination = sourceStrings.at(i);
        }

        //Expression Pedal Source
        else if(currentSourceMenu->currentText() == "Expression Pedal")
        {
            //qDebug() << "Expression Pedal Flag";
            exprPedalFlag = true;
            activeExpressionDestination = sourceStrings.at(i);
        }

    }

//    qDebug() << "tilt" << tiltFlag << activeTiltDestination;
//    qDebug() << "pressure" << pressureFlag << activePressureDestination;
//    qDebug() << "expr pedal" << exprPedalFlag << activeExpressionDestination;

    //---------------------------------------------------------- Limit Menus ----------------------------------------------------------//
    for(int i = 0; i < sourceStrings.count(); i++)
    {
        currentSourceMenu = this->findChild<QComboBox *>(sourceStrings.at(i));

        //qDebug() << "sourceStrings.at(i): " << sourceStrings.at(i);
        //------------------------------------------------------------------------------------------------//
        //--------------------------------------------- Tilt ---------------------------------------------//
        //------------------------------------------------------------------------------------------------//
        //If Tilt active and this isn't the source
        if(tiltFlag && sourceStrings.at(i) != activeTiltDestination)
        {
            //qDebug() << "disable Tilt";
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Tilt"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, disableVariant, Qt::UserRole -1);
        }
        else
        {
            //qDebug() << "enable Tilt";
            //Enable Tilt for this Dest
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Tilt"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, enableVariant, Qt::UserRole -1);
        }

        //------------------------------------------------------------------------------------------------//
        //------------------------------------------- Pressure -------------------------------------------//
        //------------------------------------------------------------------------------------------------//
        //If Pressure is active and this isn't the source
        if(pressureFlag && sourceStrings.at(i) != activePressureDestination)
        {
            //qDebug() << "disable Pressure";
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Pressure"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, disableVariant, Qt::UserRole -1);
        }
        else
        {
            //qDebug() << "enable Pressure";
            //Enable Pressure for this Dest
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Pressure"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, enableVariant, Qt::UserRole -1);
        }


        //------------------------------------------------------------------------------------------------//
        //----------------------------------------- Expr. Pedal ------------------------------------------//
        //------------------------------------------------------------------------------------------------//
        //If Tilt and a USB Ouput occurred
        if(exprPedalFlag && sourceStrings.at(i) != activeExpressionDestination)
        {
            //qDebug() << "disable Expression Pedal";
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Expression Pedal"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, disableVariant, Qt::UserRole -1);
        }
        else
        {
            //qDebug() << "enable Expression Pedal";
            //Enable Expr for this Dest
            currentModelIndex = currentSourceMenu->model()->index(currentSourceMenu->findText("Expression Pedal"), 0);
            currentSourceMenu->model()->setData(currentModelIndex, enableVariant, Qt::UserRole -1);
        }

    }
}

void MainWindow::slotUpdateLibraryPresetsListUi(QVariantMap presetMaps, int action, int data)
{
    qDebug() << "slotUpdateLibraryPresetsListUi - Data: " << data << " Action: " << action;
    for(int i = 0; i < libraryPresetsList->count(); i++)
    {
        libraryPresetsList->item(i)->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::AlignAbsolute);
    }

    //int selectedPresetTitleTextSize;
    QString selectedPresetTitleText;
    //QLabel*  selectedPresetTitle = this->findChild<QLabel *>("selectedPresetLabel");

    // for modify case, declare here to avoid switch case errors
    QPushButton* revertButton;
    QPushButton* saveButton;
    QListWidgetItem *thisItem;
    QString itemText;

    //---------- Handle UI for Each Library Action
    switch(action)
    {
    case DELETE_PRESET:

        qDebug() << "Delete Preset";
        //Clear list (old map)
        libraryPresetsList->blockSignals(true); // single click signals need to be blocked or program will crash
        libraryPresetsList->clear();
        libraryPresetsList->blockSignals(false);

        qDebug() << "repopulate";

        //Repopulate list with new map
        for(int i =0; i<presetMaps.size(); i++)
        {
            qDebug() << "repopulate i: " << i;
            if(i < 10)
            {
                qDebug() << presetMaps.value(QString("Preset_00%1").arg(i)).toMap().value("Preset_Name").toString();
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_00%1").arg(i)).toMap().value("Preset_Name").toString());

            }
            else if(i < 100)
            {
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_0%1").arg(i)).toMap().value("Preset_Name").toString());
            }
            else
            {
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_%1").arg(i)).toMap().value("Preset_Name").toString());
            }
        }
        qDebug() << "finish repopulate";

        //------------- Handle Board Preset Icon Display

        for(int i = 0; i < 4; i++)
        {
            //If preset being deleted had been sent to board, set it to -1
            if(settings->value(QString("boardPreset%1").arg(i)).toInt() == presetManager->selectedLibraryPreset)
            {
                settings->setValue(QString("boardPreset%1").arg(i),-1);
            }

            //If preset being deleted had been sent to board, set it to -1
            if(settings->value(QString("boardPreset%1").arg(i)).toInt() > presetManager->selectedLibraryPreset)
            {
                qDebug() << "prest count" << libraryPresetsList->count();
                settings->setValue(QString("boardPreset%1").arg(i), settings->value(QString("boardPreset%1").arg(i)).toInt() - 1);
            }
        }

        //------------- Retain Correct Selected Preset upon deletion
        //If index of deleted item was EQUAL tp the index of selected item
        if(presetManager->selectedLibraryPreset == libraryPresetsList->count())
        {
            presetManager->selectedLibraryPreset -= 1;
            libraryPresetsList->setCurrentRow(presetManager->selectedLibraryPreset);
        }
        else
        {
            libraryPresetsList->setCurrentRow(presetManager->selectedLibraryPreset);
        }

        break;

    case CREATE_PRESET:
        qDebug() << "Create Preset";

        //Clear list (old map)
        libraryPresetsList->blockSignals(true); // single click signals need to be blocked or program will crash
        libraryPresetsList->clear();
        libraryPresetsList->blockSignals(false);

        //Populate list with new map
        qDebug() << "populate list";
        for(int i =0; i<presetMaps.size(); i++)
        {
            if(i < 10)
            {
                //qDebug() << presetMaps.value(QString("Preset_00%1").arg(i)).toMap().value("Preset_Name").toString();
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_00%1").arg(i)).toMap().value("Preset_Name").toString());
            }
            else if(i < 100)
            {
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_0%1").arg(i)).toMap().value("Preset_Name").toString());
            }
            else
            {
                libraryPresetsList->addItem(presetMaps.value(QString("Preset_%1").arg(i)).toMap().value("Preset_Name").toString());
            }
        }
        qDebug() << "end populate list";
        libraryPresetsList->setCurrentRow(presetManager->selectedLibraryPreset);

        break;

    case SET_HIGHLIGHT:
        qDebug() << "Set Highlight";
        //Nothing here for now...
        break;

    case SET_MODIFIED:
        //qDebug() << "---- SET_MODIFIED Called" << presetManager->slotCheckPresetEdits();

        // update buttons
        revertButton = this->findChild<QPushButton *>("revertButton");
        revertButton->setEnabled(true);

        saveButton = this->findChild<QPushButton *>("saveButton");
        saveButton->setEnabled(true);

        // add asterisk to text
        thisItem = libraryPresetsList->item(presetManager->selectedLibraryPreset);
        itemText = thisItem->text();
        if(!itemText.contains("*"))
        {
            thisItem->setText(thisItem->text().append("*")); // sets the preset name as dirty
        }
        // fall through
        //break;

    case LOAD_PRESET:
        qDebug() << "load preset";
    case SAVE_PRESET:
        qDebug() << "save preset, remove dirty asterisk";
    case SET_SELECTED:
        // this crashes the program when not called by a signal
        //qDebug() << "set selected - object sender: " << QObject::sender()->objectName();
        if(presetManager->selectedLibraryPreset != -1)
        {

            //Clear red indicators
            for(int i = 0; i < libraryPresetsList->count(); i++)
            {
                QListWidgetItem *item = libraryPresetsList->item(i);
                QString itemText = item->text();

                if (action == SAVE_PRESET || action == LOAD_PRESET)
                {
                    if(itemText.contains("*"))
                    {
                        itemText.remove("*");
                    }
                }
                if(itemText.contains("[ "))
                {
                    itemText.remove("[ ");
                }
                if(itemText.contains(" ]"))
                {
                    itemText.remove(" ]");
                }

                item->setText(itemText);
                item->setForeground(QBrush(QColor(175,175,175,255)));
            }

            selectedPresetTitleText = libraryPresetsList->item(presetManager->selectedLibraryPreset)->text();
            qDebug() << "<<< select preset" << selectedPresetTitleText;

            // turn red
            QListWidgetItem *thisItem = libraryPresetsList->item(presetManager->selectedLibraryPreset);

            // add brackets to current loaded preset
            QString bracketText = thisItem->text();
            bracketText.prepend("[ ");
            bracketText.append(" ]");
            thisItem->setText(bracketText);

            // set text to red
            thisItem->setForeground(QBrush(QColor(241,101,65,255)));

            // highlight selected item
            libraryPresetsList->setCurrentItem(thisItem);

            //-------------------------------------------------------------------- Manage Send Buttons and Send Warnin
//            qDebug() << "<<< preset loaded: SET_SELECTED";
            // send to edit buffer
            qDebug() << "** send to preset buffer **";
            //presetManager->presetLoading = false; // enable preset send

            // if saving then we don't need to sync the preset
            if (action != SAVE_PRESET)
            {
                presetManager->slotSyncEditBufferToDevice();
            }
        }
        break;
    }

    //----------------------------------------------------------------- Update Icon Highlights -------------------------------------------------------------------------//

    libraryPresetsList->setIconSize(QSize(12,12));

    //If there is a currently selected item
    if(libraryPresetsList->currentItem())
    {
        for(int i = 0; i < libraryPresetsList->count(); i++)
        {
            QString text;

            //If the currently changed board preset == another board preset, set older preset to -1, update new value
            for(int j = 0; j < 4; j++)
            {
                if(settings->value(QString("boardPreset%1").arg(j)) == i)
                {
//                    qDebug() << "boardPreset" << j << "index" << i;

                    switch (j)
                    {
                    case 0:
                        text.append("A");
                        break;
                    case 1:
                        text.append("B");
                        break;
                    case 2:
                        text.append("C");
                        break;
                    case 3:
                        text.append("D");
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    //text = QString(""); <--- This manifests with blank constructor above.
                }
            }

            QIcon presetIcon;
            QPixmap iconPixmap(100,100);
            iconPixmap.fill(Qt::transparent);

            QPainter painter(&iconPixmap);
            painter.fillRect(iconPixmap.rect(), Qt::NoBrush);
            painter.setPen(Qt::white);

#ifdef Q_OS_MAC // EB attempt to make this cross platform
            painter.setFont(QFont("Arial", 100));
#else
            painter.setFont(QFont("Arial", 75));
#endif


            //qDebug() << "---------------------------------- preset #" << i << "text" << text;
            painter.drawStaticText(0,0, QStaticText(text));
            presetIcon.addPixmap(iconPixmap, QIcon::Normal, QIcon::Off);
            libraryPresetsList->item(i)->setIcon(presetIcon);
        }
    }

    settings->setValue("selectedPreset", presetManager->selectedLibraryPreset);
    //settings->setValue("ledRefreshStyle", ledRefreshModeGroup->checkedAction()->objectName());

    //------------------------------- Update LED Refresh Menu Styes
    //qDebug() << "--------------" << presetManager->editBufferPresetMap.value("Globals_LED_Refresh_Style").toString();

    if(presetManager->editBufferPresetMap.value("Globals_LED_Refresh_Style").toString() == "Normal")
    {
        ledRefreshNormal->setChecked(true);
    }
    else if(presetManager->editBufferPresetMap.value("Globals_LED_Refresh_Style").toString() == "Control Only")
    {
        ledRefreshSilent->setChecked(true);
    }
    else if(presetManager->editBufferPresetMap.value("Globals_LED_Refresh_Style").toString() == "All Off")
    {
        ledRefreshAllOff->setChecked(true);
    }
}

void MainWindow::slotUpdateControllerLayerHighlightUi(QStringList activeKeys)
{

    //Disable slotValueChanged (prevents sysex from sending on new controller layer click)
    presetManager->newControllerKeySelected = true;

    //------------ Handles Controller Layer Highlights
    QPushButton* highlightButton;
    //qDebug() << "slotUpdateControllerLayerHighlightUi: active keys" << activeKeys;

    //---------------------------------------------------- Set Highlights, just graphical --------------------------------------------------------//
    for(int i=0; i<25; i++)
    {
        highlightButton = this->findChild<QPushButton *>(QString("Key_Highlight_%1").arg(i));
        highlightButton->setStyleSheet(  "border:2px solid rgba(10, 255, 255, 0);"
                                         "border-radius: 4px;"
                                         "border-bottom-left-radius:8.0;"
                                         "border-bottom-right-radius:8.0;"
                                         "border-top-left-radius:8.0;"
                                         "border-top-right-radius:8.0;");
    }

    highlightButton = this->findChild<QPushButton *>(QString("Bend_Highlight"));
    highlightButton->setStyleSheet(  "border:2px solid rgba(10, 255, 0,0);"
                                     "border-radius: 4px;"
                                     "border-bottom-left-radius:8.0;"
                                     "border-bottom-right-radius:8.0;"
                                     "border-top-left-radius:8.0;"
                                     "border-top-right-radius:8.0;");


    for(int i=0; i<activeKeys.count(); i++)
    {
        highlightButton = this->findChild<QPushButton *>(activeKeys.at(i));
        highlightButton->setStyleSheet(   "border:2px solid rgb(10, 255, 0);"
                                          "border-radius: 4px;"
                                          "border-bottom-left-radius:8.0;"
                                          "border-bottom-right-radius:8.0;"
                                          "border-top-left-radius:8.0;"
                                          "border-top-right-radius:8.0;");
    }


    //------------------------- Text Label of Selected Controller Layer Key
    QStackedWidget* editPane = this->findChild<QStackedWidget *>("Edit_Panes");
    //QLabel* editPaneLabel = this->findChild<QLabel *>("Edit_Pane_Label");
    QString labelString;
    QFont font;
    font.setBold(1);
    font.setItalic(1);
    //editPaneLabel->setFont(font);

    QString jsonControllerNum;

    if(activeKeys.at(0).contains("Key"))
    {
        editPane->setCurrentIndex(0);

        labelString = activeKeys.at(0);
        labelString.remove(0,14);

        jsonControllerNum = labelString;
        jsonControllerNum.append("_");

        labelString.prepend("Key ");

        //editPaneLabel->setText(labelString);

        ///////// Keys /////////
        //Channel
        QSpinBox* controllerKeyChannel = this->findChild<QSpinBox *>(QString("Controller_Key_Channel"));
        controllerKeyChannel->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Channel").arg(jsonControllerNum)).toInt() + 1);

        //Layer Enable/Disable
        QCheckBox* enableKeyController = this->findChild<QCheckBox *>("Controller_Key_Controller_Layer_Enable_Disable");
        enableKeyController->setChecked(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Controller_Layer_Enable_Disable").arg(jsonControllerNum)).toBool());

        QCheckBox* enableKeyKeyboard = this->findChild<QCheckBox *>("Controller_Key_Keyboard_Layer_Enable_Disable");
        enableKeyKeyboard->setChecked(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Keyboard_Layer_Enable_Disable").arg(jsonControllerNum)).toBool());
        slotEnableDisableToggleControl(enableKeyKeyboard->checkState());

        //Note
        QSpinBox* controllerKeyNoteNumber = this->findChild<QSpinBox *>("Controller_Key_Note_Number");
        controllerKeyNoteNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Note_Number").arg(jsonControllerNum)).toInt());
        QSpinBox* controllerKeyNoteVelocity = this->findChild<QSpinBox *>("Controller_Key_Note_Velocity");
        controllerKeyNoteVelocity->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Note_Velocity").arg(jsonControllerNum)).toInt());

        //Pressure
        QSpinBox* controllerKeyPressureControlNumber = this->findChild<QSpinBox *>("Controller_Key_Pressure_Control_Number");
        controllerKeyPressureControlNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Pressure_Control_Number").arg(jsonControllerNum)).toInt());
        QSpinBox* controllerKeyPressureReturnValue = this->findChild<QSpinBox *>("Controller_Key_Pressure_Return_Value");
        controllerKeyPressureReturnValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Pressure_Return_Value").arg(jsonControllerNum)).toInt());

        //Tilt
        QSpinBox* controllerKeyTiltControlNumber = this->findChild<QSpinBox *>("Controller_Key_Tilt_Control_Number");
        controllerKeyTiltControlNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Tilt_Control_Number").arg(jsonControllerNum)).toInt());
        QSpinBox* controllerKeyTiltReturnValue = this->findChild<QSpinBox *>("Controller_Key_Tilt_Return_Value");
        controllerKeyTiltReturnValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Tilt_Return_Value").arg(jsonControllerNum)).toInt());

        //Toggle
        QSpinBox* controllerKeyToggleControlNumber = this->findChild<QSpinBox *>("Controller_Key_Toggle_Control_Number");
        controllerKeyToggleControlNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Toggle_Control_Number").arg(jsonControllerNum)).toInt());
        QSpinBox* controllerKeyToggleValue = this->findChild<QSpinBox *>("Controller_Key_Toggle_Value");
        controllerKeyToggleValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Toggle_Value").arg(jsonControllerNum)).toInt());
        QSpinBox* controllerKeyToggleReturnValue = this->findChild<QSpinBox *>("Controller_Key_Toggle_Return_Value");
        controllerKeyToggleReturnValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Toggle_Return_Value").arg(jsonControllerNum)).toInt());
        QCheckBox* enableKeyControllerToggle = this->findChild<QCheckBox *>("Controller_Key_Toggle_On_Off");
        enableKeyControllerToggle->setChecked(presetManager->editBufferPresetMap.value(QString("Controller_Key_%1Toggle_On_Off").arg(jsonControllerNum)).toBool());
        slotEnableDisableToggleParameters(enableKeyControllerToggle->checkState());
    }

    else if(activeKeys.at(0).contains("Bend"))
    {
        editPane->setCurrentIndex(1);
        labelString = "Bend Pad";
        //editPaneLabel->setText(labelString);

        //////// Bend Pad //////////
        //Channel
        QSpinBox* controllerBendChannel = this->findChild<QSpinBox *>("Controller_Bend_Pad_Channel");
        controllerBendChannel->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Channel")).toInt() + 1);

        //Layer Enable/Disable
        QCheckBox* enableBendController = this->findChild<QCheckBox *>("Controller_Bend_Pad_Controller_Layer_Enable_Disable");
        enableBendController->setChecked(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Controller_Layer_Enable_Disable")).toBool());
        QCheckBox* enableBendKeyboard = this->findChild<QCheckBox *>("Controller_Bend_Pad_Keyboard_Layer_Enable_Disable");
        enableBendKeyboard->setChecked(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable")).toBool());

        //Note
        QSpinBox* controllerBendNoteNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Note_Number");
        controllerBendNoteNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Note_Number")).toInt());
        QSpinBox* controllerBendNoteVelocity = this->findChild<QSpinBox *>("Controller_Bend_Pad_Note_Velocity");
        controllerBendNoteVelocity->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Note_Velocity")).toInt());

        //Pressure
        QSpinBox* controllerBendPressureControlNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Pressure_Control_Number");
        controllerBendPressureControlNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Pressure_Control_Number")).toInt());
        QSpinBox* controllerBendPressureReturnValue = this->findChild<QSpinBox *>("Controller_Bend_Pad_Pressure_Return_Value");
        controllerBendPressureReturnValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Pressure_Return_Value")).toInt());

        //Tilt
        QSpinBox* controllerBendTiltControlNumber = this->findChild<QSpinBox *>("Controller_Bend_Pad_Tilt_Control_Number");
        controllerBendTiltControlNumber->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Tilt_Control_Number")).toInt());
        QSpinBox* controllerBendTiltReturnValue = this->findChild<QSpinBox *>("Controller_Bend_Pad_Tilt_Return_Value");
        controllerBendTiltReturnValue->setValue(presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Tilt_Return_Value")).toInt());
    }

    //Handle Controller Port
    QComboBox* controllerDeviceOutput = this->findChild<QComboBox *>("Controller_Global_MIDI_Output_Device");
    int textIndex = controllerDeviceOutput->findText(presetManager->editBufferPresetMap.value("Controller_Global_MIDI_Output_Device").toString());
    controllerDeviceOutput->setCurrentIndex(textIndex);

    //Re-enable slotValueChanged
    presetManager->newControllerKeySelected = false;


}

void MainWindow::slotEnableDisableToggleParameters(bool toggled){
    QSpinBox *toggleParam1;
    QSpinBox *toggleParam2;
    QSpinBox *toggleParam3;

    //QPalette pal;
    toggleParam1 = this->findChild<QSpinBox *>("Controller_Key_Toggle_Control_Number");
    toggleParam2 = this->findChild<QSpinBox *>("Controller_Key_Toggle_Return_Value");
    toggleParam3 = this->findChild<QSpinBox *>("Controller_Key_Toggle_Value");

    toggleParam1->setEnabled(toggled);
    toggleParam2->setEnabled(toggled);
    toggleParam3->setEnabled(toggled);

    if(toggled){
        toggleParam1->setStyleSheet(
                    "font-family:Arial;"
                    "border-radius:4.0;"
                    "border-color:black;"
                    "background-color: rgba(67, 67, 67, 255);"
                    "font-size: 11px;"
                    "color: white;"
                    "padding:0, 3, 0, 0;");

        toggleParam2->setStyleSheet(toggleParam1->styleSheet());
        toggleParam3->setStyleSheet(toggleParam1->styleSheet());
    }
    else{
        toggleParam1->setStyleSheet(
                    "font-family:Arial;"
                    "border-radius:4.0;"
                    "border-color:black;"
                    "background-color: rgba(67, 67, 67, 255);"
                    "font-size: 11px;"
                    "color: gray;"
                    "padding:0, 3, 0, 0;");
        toggleParam2->setStyleSheet(toggleParam1->styleSheet());
        toggleParam3->setStyleSheet(toggleParam1->styleSheet());

    }
}

void MainWindow::slotEnableDisableToggleControl(bool enabled){
    //qDebug() << "slotEnableDisableToggleControl called";
    QCheckBox *enableToggleParams;
    enableToggleParams = this->findChild<QCheckBox *>("Controller_Key_Toggle_On_Off");

    if(enabled){
        slotEnableDisableToggleParameters(false);
        enableToggleParams->setChecked(false);
        enableToggleParams->setEnabled(false);
    }
    else{
        enableToggleParams->setEnabled(true);
        slotEnableDisableToggleParameters(enableToggleParams->checkState());
    }
}

void MainWindow::slotSetEndableUiElement(QString paramName, bool state)
{
    qDebug() << "slotSetEndableUiElement called - paramName: " << paramName;

    QWidget *thisWidget;
    thisWidget = this->findChild<QWidget *>(paramName);

    if(!paramName.contains("connected"))
    {
        //Enable / Disable is a widget-level function...
        thisWidget->setEnabled(state);
    }



    //If a sync preset button
    if(paramName.contains("syncPreset"))
    {
        if(connected)
        {
            QLabel *label = this->findChild<QLabel *>("QuNexus_Not_Connected");
            label->hide();

            if(state)
            {
                thisWidget->setStyleSheet(blueStyleString);
                thisWidget->show();

                //saveAndSend->hide();
            }
            else
            {
                thisWidget->setStyleSheet(grayStyleString);
                thisWidget->hide();
                QLabel *label = this->findChild<QLabel *>("QuNexus_Not_Connected");
                label->hide();


                for(int i = 0; i < 4; i++)
                {
                    if(settings->value(QString("boardPreset%1").arg(i)) == presetManager->selectedLibraryPreset)
                    {
                        break;
                    }
                }
            }
        }
    }
    else if(paramName.contains("connected"))
    {
        if(state)
        {
            QLabel *label = this->findChild<QLabel *>("QuNexus_Not_Connected");
            label->hide();

            for(int i = 0; i < 4; i++)
            {
                QPushButton *syncButton = this->findChild<QPushButton *>(QString("syncPreset%1").arg(i));
                syncButton->show();

            }
        }
        else
        {
            QLabel *label = this->findChild<QLabel *>("QuNexus_Not_Connected");
            label->show();

            for(int i = 0; i < 4; i++)
            {
                QPushButton *syncButton = this->findChild<QPushButton *>(QString("syncPreset%1").arg(i));
                syncButton->hide();
            }
        }
    }

    //If a spin box
    else
    {
        qDebug() << "slotSetEndableUiElement called - paramName: " << paramName;
        if(state)
        {
            thisWidget->setStyleSheet( "font-family: 'arial';"
                                   "border-radius:4.0;"
                                   "border-color:black;"
                                   "background-color: rgba(67, 67, 67, 255);"
                                   "font-size: 11px;"
                                   "color: white;"
                                   "padding:0, 2, 0, 0;"
                                   );
        }
        else
        {
            thisWidget->setStyleSheet( "font-family: 'arial';"
                                   "border-radius:4.0;"
                                   "border-color:black;"
                                   "background-color: rgba(30, 30, 30, 255);"
                                   "font-size: 11px;"
                                   "color: rgba(100,100,100,255);"
                                   "padding:0, 2, 0, 0;"
                                   );
        }

        // hide this when disabled
        if (paramName.contains("Keyboard_Global_Polyphony_Number"))
        {
            if (state)
            {
                thisWidget->show();
            }
            else
            {
                thisWidget->hide();
            }
        }
    }
}
void MainWindow::slotCheckChannelOverlap()
{
    if (ui->SEQ_track_1_midi_ch->currentText() != "MPE") return;

    int mpeZoneChannelCount = ui->Keyboard_Global_Polyphony_Number->value() + 1;
    int track2channel = ui->SEQ_track_2_midi_ch->currentIndex() + 1;
    int track3channel = ui->SEQ_track_3_midi_ch->currentIndex() + 1;

    qDebug() << "slotCheckChannelOverlap called - t2: " << track2channel << " t3:" << track3channel << " mpeZone: " << mpeZoneChannelCount;
    if (track2channel <= mpeZoneChannelCount)
    {
        ui->SEQ_track_2_midi_ch->setCurrentIndex(14);
    }
    if (track3channel <= mpeZoneChannelCount)
    {
        ui->SEQ_track_3_midi_ch->setCurrentIndex(15);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// Dialogs //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::slotUnsavedChangesDialog(QListWidgetItem* newPresetItem)
{
    // stop this dialog from popping up at open (nasty hack)
    if (firstRunUnsavedFlag)
    {
        firstRunUnsavedFlag = false;
        return;
    }
    // store this global pointer to be called if save or discard are called
    nextPresetItem = newPresetItem;

    qDebug() << "slotUnsavedChangesDialog called";

    slotShowGreyRect(); // darken QuNexus editor interface

    QDialog *msgBox = new QDialog(this);
    msgBox->setMinimumSize(300, 100);
    msgBox->setFixedSize(300, 100);

    QPoint centerparent(
                this->x() + ((this->frameGeometry().width() - msgBox->frameGeometry().width()) /2),
                this->y() + ((this->frameGeometry().height() - msgBox->frameGeometry().height()) /2));

    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->move(centerparent);


    msgBox->setStyleSheet("QWidget{background: rgba(100,100,100, 255); border-color:black; border:5px}");

    QString currentPresetText = libraryPresetsList->item(presetManager->selectedLibraryPreset)->text();
    QString dialogText = QString("Save Changes to %1?").arg(currentPresetText);
    QLabel* text = new QLabel(dialogText, msgBox, Qt::WindowFlags());
    text->setAlignment(Qt::AlignCenter);
    text->setMinimumSize(300, 50);
    text->setFixedSize(300, 50);
    text->move(0, 10);

#ifdef Q_OS_MAC // EB attempt to make this cross platform
    text->setStyleSheet("font: 12pt;");
#else
    text->setStyleSheet("font: 10pt;");
#endif

    QPushButton* discardButton = new QPushButton(msgBox);
    discardButton->setStyleSheet(blueStyleString);
    discardButton->setText("Don't Save");
    discardButton->setGeometry(QRect(10, 60, 70, 28));
    connect(discardButton, SIGNAL(clicked()), msgBox, SLOT(close()));
    connect(discardButton, SIGNAL(clicked()), this, SLOT(slotDiscardButton()));

    QPushButton* cancelButton = new QPushButton(msgBox);
    cancelButton->setStyleSheet(grayStyleString);
    cancelButton->setText("Cancel");
    cancelButton->setGeometry(QRect(140, 60, 70, 28));
    connect(cancelButton, SIGNAL(clicked()), msgBox, SLOT(close()));

    QPushButton* saveButton = new QPushButton(msgBox);
    saveButton->setStyleSheet(blueStyleString);
    saveButton->setText("Save");
    saveButton->setGeometry(QRect(220, 60, 70, 28));
    connect(saveButton, SIGNAL(clicked()), msgBox, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveButton()));

    msgBox->exec();
    slotHideGreyRect(); // darken QuNexus editor interface
}

void MainWindow::slotDiscardButton()
{
    presetManager->JSONLoading = true; // set flag to halt updating qunexus hardware
    presetManager->slotRevertSelectedPreset();
    presetManager->slotLoadLibraryPresetIntoEditBuffer(nextPresetItem);
}

void MainWindow::slotSaveButton()
{
    presetManager->JSONLoading = true; // set flag to halt updating qunexus hardware
    presetManager->slotSaveThenLoadPreset(nextPresetItem);
}


void MainWindow::slotOpenAboutScreen()
{

    slotShowGreyRect();

    QDialog *msgBox = new QDialog(this);
    msgBox->setMinimumSize(300, 325);
    msgBox->setFixedSize(300, 325);

    QLabel* title = new QLabel(QString("QuNexus Editor v%1.%2.%3")
                               .arg(uchar(applicationVersion.at(0)))
                               .arg(uchar(applicationVersion.at(1)))
                               .arg(uchar(applicationVersion.at(2))), msgBox, Qt::WindowFlags());

    if (betaVersion != "")
    {
        QString addToThis = title->text();
        addToThis.append(betaVersion);
        title->setText(addToThis);
    }

    title->setMinimumSize(300, 50);
    title->setFixedSize(300, 50);
    title->setStyleSheet("font: bold 16pt;");
    title->move(0, 15);
    title->setAlignment(Qt::AlignCenter);


    QString thisLabelString;
    if (connected)
    {
        thisLabelString = deviceBootloaderVersionString();

        thisLabelString.append(applicationFirmwareVersionString());

        thisLabelString.append(deviceFirmwareVersionString());
    }
    else
    {
        thisLabelString = applicationFirmwareVersionString();

        thisLabelString.append(QString("Device Not Connected"));
    }

    qDebug() << "thisLabelString: " << thisLabelString;
    QLabel* text = new QLabel(thisLabelString, msgBox, Qt::WindowFlags());
    text->setAlignment(Qt::AlignCenter);
    text->setMinimumSize(300, 100);
    text->setFixedSize(300, 100);
    text->move(0, 50);


#ifdef Q_OS_MAC // EB attempt to make this cross platform
    text->setStyleSheet("font: 12pt;");
#else
    text->setStyleSheet("font: 10pt;");
#endif


    QPoint centerparent(
                this->x() + ((this->frameGeometry().width() - msgBox->frameGeometry().width()) /2),
                this->y() + ((this->frameGeometry().height() - msgBox->frameGeometry().height()) /2));

    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->move(centerparent);

    msgBox->setStyleSheet("QWidget{background: rgba(120,120,120, 255)}; QDialog::QWidget {border: 3px solid black};");

    QLabel* aboutInfo = new QLabel(QString("Editor originally developed by Conner Lacey, "
                                         "\nDiane Douglas, and Sarah Howe."

                                       "\n\nVersion 2 editor developed by Eric Bateman "
                                         "\nand Andrej Hronco.")
                               , msgBox, Qt::WindowFlags());

    aboutInfo->setMinimumSize(300, 90);
    aboutInfo->setFixedSize(300, 90);
#ifdef Q_OS_MAC // EB attempt to make this cross platform
    aboutInfo->setStyleSheet("font:  12pt;");
    aboutInfo->setGeometry(0, 160, 275, 90);
#else
    aboutInfo->setStyleSheet("font:  10pt;");
    aboutInfo->setGeometry(0, 160, 275, 110);
#endif



    aboutInfo->setAlignment(Qt::AlignCenter);

    // sugar
    QLabel* sugarImage = new QLabel("", msgBox, Qt::WindowFlags());

    QPixmap sugarPix (":/images/resources/images/cat_sugar.png");
    sugarImage->setPixmap(sugarPix);
    sugarImage->setScaledContents(true);
    sugarImage->setStyleSheet("QWidget{background-color: transparent;}");
    sugarImage->setGeometry(0, 245, 60, 80);

    // Plum
    QLabel* plumImage = new QLabel("", msgBox, Qt::WindowFlags());

    QPixmap plumPix (":/images/resources/images/cat_plum.png");
    plumImage->setPixmap(plumPix);
    plumImage->setScaledContents(true);
    plumImage->setStyleSheet("QWidget{background-color: transparent;}");
    plumImage->setGeometry(190, 240, 140, 140);

    QPushButton* updateButton = new QPushButton(msgBox);
    updateButton->setStyleSheet(blueStyleString);
    updateButton->setText("Ok");
    updateButton->setGeometry(QRect(115, 275, 70, 28));
    connect(updateButton, SIGNAL(clicked()), msgBox, SLOT(close()));

    msgBox->exec();
    slotHideGreyRect();
}

void MainWindow::slotShowGreyRect()
{
    ui->greyRect->show();
    ui->greyRect->activateWindow();
    ui->greyRect->raise();
}

void MainWindow::slotHideGreyRect()
{
   ui->greyRect->hide();
}

// MIDI OVERHAUL --------------------------------------
QString MainWindow::deviceBootloaderVersionString()
{
    return QString("Device Bootloader Version: %1.%2.%3\n\n")
            .arg(uchar(QuNexus->devicebootloaderVersion.at(0)))
            .arg(uchar(QuNexus->devicebootloaderVersion.at(1)))
            .arg(uchar(QuNexus->devicebootloaderVersion.at(2)));
}


QString MainWindow::deviceFirmwareVersionString()
{
    return QString("Device Firmware Version: %1.%2.%3")
            .arg(uchar(QuNexus->deviceFirmwareVersion.at(0)))
            .arg(uchar(QuNexus->deviceFirmwareVersion.at(1)))
            .arg(uchar(QuNexus->deviceFirmwareVersion.at(2)));
}

QString MainWindow::applicationFirmwareVersionString()
{
    return QString("Application Firmware Version: %1.%2.%3\n\n")
            .arg(uchar(thisFw.at(0)))
            .arg(uchar(thisFw.at(1)))
            .arg(uchar(thisFw.at(2)));
}
// END MIDI OVERHAUL --------------------------------------


void MainWindow::slotSendPresetsToBoardDialog(QString presetName)
{
    if(connected)
    {
        if(settings->value("hideSendPresets").toInt() != 1)
        {
            SendPresetsDialog* sendPresetDialog = new SendPresetsDialog(this, presetName, presetManager);
            //connect(sendPresetDialog, SIGNAL(signalSysExEncodePreset(QVariantMap,unsigned char)), sysExEncode, SLOT(slotSysExEncodePreset(QVariantMap, unsigned char)));

            sendPresetDialog->exec();
        }
    }
}

void MainWindow::slotSyncQuNexusDialog()
{
    qDebug() << "slotSyncQuNexusDialog called";

    slotShowGreyRect();

    QuNexus->slotStopPolling("slotSyncQuNexusDialog"); // stop loop
    syncQuNexusDialog->slotCenterDialog();
    syncQuNexusDialog->show();
}

void MainWindow::slotDeletePresetQuery()
{

    qDebug() << "delete preset query called";

    QDialog *msgBox = new QDialog(this);
    msgBox->setMinimumSize(300, 100);
    msgBox->setFixedSize(300, 100);

    QPoint centerparent(
                this->x() + ((this->frameGeometry().width() - msgBox->frameGeometry().width()) /2),
                this->y() + ((this->frameGeometry().height() - msgBox->frameGeometry().height()) /2));

    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->move(centerparent);

    msgBox->setStyleSheet("QWidget{background: rgba(120,120,120, 255);}");

    //Copy JSON Master into a Temp Map
    QVariantMap tempMasterMap = presetManager->jsonMasterMapEDITABLE; //Map used to properly format preset_NNN names upon deletion
    QString     tempPresetNameParam;

    //If a libarary preset is highlighted...
    if(presetManager->selectedLibraryPreset != -1)
    {
        //Get "Preset_Name" to display in warning dialog
        tempPresetNameParam = tempMasterMap.value(presetManager->slotGetPresetNameFromIndex(libraryPresetsList->currentRow())).toMap().value("Preset_Name").toString();

        QLabel* text = new QLabel(QString("Are you sure you want to delete the preset:\n\"%1\"?").arg(tempPresetNameParam), msgBox, Qt::WindowFlags());
#ifdef Q_OS_MAC // EB attempt to make this cross platform
        text->setStyleSheet("font: 12pt;");
#else
        text->setStyleSheet("font: 10pt;");
#endif
        text->setGeometry(0, 10, 300, 50);
        text->setAlignment(Qt::AlignCenter);

        QPushButton* cancelButton = new QPushButton(msgBox);
        cancelButton->setStyleSheet(grayStyleString);
        cancelButton->setText("Cancel");
        cancelButton->setGeometry(QRect(80, 60, 70, 28));
        connect(cancelButton, SIGNAL(clicked()), msgBox, SLOT(close()));

        QPushButton* updateButton = new QPushButton(msgBox);
        updateButton->setStyleSheet(blueStyleString);
        updateButton->setText("Delete");
        updateButton->setGeometry(QRect(160, 60, 70, 28));
        updateButton->setFocus();
        connect(updateButton, SIGNAL(clicked()), msgBox, SLOT(close()));
        connect(updateButton, SIGNAL(clicked()), presetManager, SLOT(slotDeletePreset()));
    }

    //If no library preset is selected
    else
    {
        QLabel* text = new QLabel(QString("No Preset Selected").arg(tempPresetNameParam), msgBox, Qt::WindowFlags());
        text->setStyleSheet("font: 12pt;");
        text->move(35, 20);

        QPushButton* cancelButton = new QPushButton(msgBox);
        cancelButton->setStyleSheet(grayStyleString);
        cancelButton->setText("Cancel");
        cancelButton->setGeometry(QRect(120, 60, 70, 28));
        connect(cancelButton, SIGNAL(clicked()), msgBox, SLOT(close()));
    }



    msgBox->exec();
}

void MainWindow::slotQuNexusConnected(bool state)
{
    qDebug() << "slotQuNexusConnected called - state: " << state;
    if(state)
    {
        QPushButton* connectedLabel = this->findChild<QPushButton *>("QuNexus_Connected_Label");
        connectedLabel->setText("Connected");

        QFrame* connectedFrame = this->findChild<QFrame *>("QuNexus_Connected_Frame");
        connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background:rgb(10,255,0); border-radius:6;}");
        //qDebug() << "<< set connected: true";
        connected = true;

        //presetManager->presetLoading = true; // block sending any data from editor until after initial sync
        presetManager->syncFlag = true;

        // attempt to recall midi thru port when QuNexus connects
        slotRecallMIDIThru();
        troubleshootWindow->slotConnected(true);
    }
    else
    {
        //qDebug() << "<< set connected: false";

        // safety to ensure we don't connect this twice
        if (connected)
        {
            QuNexus->disconnect(SIGNAL(signalRxSysExBA(QByteArray)));
        }
        connected = false;

        slotDisableGlobalsWindows(); // just in case
        syncQuNexusDialog->done(1); // close the sync dialog if it's open
        presetManager->syncFlag = false; // update presetmanager
        slotHideGreyRect(); // hide grey rectangle when qunexus disconnects
        troubleshootWindow->slotConnected(false);
    }

    slotSetEndableUiElement("connected", connected);
    //slotUpdateLibraryPresetsListUi(QVariantMap(), SET_SELECTED, 0);

}

void MainWindow::slotLimitThresholds(){

    if(presetManager->selectedLibraryPreset > -1)
    {
        //if the On threshold is being adjusted, set its minimum to 1 + the value of the off thresh.
        if(QObject::sender()->objectName().contains("On")){
            onThresh->setMinimum(offThresh->value() + 1);
            offThresh->setMaximum(onThresh->value() - 1);
        }

        //if the Off threshold is being adjusted, set its maximum to the value of the on threshold - 1.
        if(QObject::sender()->objectName().contains("Off")){
            offThresh->setMaximum(onThresh->value() - 1);
            onThresh->setMinimum(offThresh->value() + 1);
        }
    }
    //This way the on is always at least one more than the off, no matter which one is being adjusted.
}

void MainWindow::slotTabChanged(int index)
{
    qDebug() << "current tab" << index;
    settings->setValue("tabIndex", index);
}

//Sets the Globals in the UI
void MainWindow::slotSetGlobals(QString global, QByteArray globalByteArray)
{
    qDebug() << "slotSetGlobals called";
//#ifdef Q_OS_MAC // EB - not sure why this is here, no reference to __int16 anywhere in the project
//#else
//    typedef __int16 int16_t;
//#endif

    if(global == "Per Key Sensitivities")
    {
        //---Sets slider values in GUI
        QSlider* slider;

        for(int i=0; i < globalByteArray.size(); i++)
        {
            //qDebug() << i << "sensitivities at =====" << (int)globalByteArray.at(i);

            if(i < 9)
            {
                slider = pksWidget->findChild<QSlider *>(QString("slider_0%1").arg(i + 1));
            }
            else
            {
                slider = pksWidget->findChild<QSlider *>(QString("slider_%1").arg(i + 1));
            }

            slider->setValue((int)globalByteArray.at(i));
        }
    }
    else if(global == "Table 1")
    {

        //User Table 1
        for(int i=0; i < globalByteArray.size(); i++)
        {
            //tableInterface->slotDisplayUserTable(0, i, int(globalByteArray.at(i)));
            tableInterface->userTable1[127 - i] = 127 - (unsigned char)globalByteArray.at(i);
        }
    }
    else if(global == "Table 2")
    {
        //User Table 2
        for(int i=0; i < globalByteArray.size(); i++)
        {
            //tableInterface->slotDisplayUserTable(1, i, int(globalByteArray.at(i)));
            tableInterface->userTable2[127 - i] = 127 - (unsigned char)globalByteArray.at(i);
        }
    }
    else if(global == "CV Trims")
    {
        //qDebug() << "cv trims size" << globalByteArray.size() << (double)globalByteArray.at(0)/100;;

        cvTrimsInterface->voltPerOctaveTrim = (double)globalByteArray.at(0)/100;
        cvTrimsInterface->_1_2_voltPerOctaveTrim = (double)globalByteArray.at(1)/100;
        cvTrimsInterface->hzPerVoltTrim = (double)globalByteArray.at(2)/100;

        cvTrimsInterface->slotShowTrims();
    }
    else if(global == "CV Offsets")
    {
        int16_t voltPerOctaveMSB = globalByteArray.at(0) & 0xFF;
        int16_t voltPerOctaveLSB = globalByteArray.at(1) & 0xFF;

        voltPerOctaveMSB =  voltPerOctaveMSB << 8;
        voltPerOctaveMSB = voltPerOctaveMSB | voltPerOctaveLSB;

        int16_t _1_2_voltPerOctaveMSB = globalByteArray.at(2) & 0xFF;
        int16_t _1_2_voltPerOctaveLSB = globalByteArray.at(3) & 0xFF;

        _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB << 8;
        _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB | _1_2_voltPerOctaveLSB;

        int16_t hzPerVolteMSB = globalByteArray.at(4) & 0xFF;
        int16_t hzPerVolteLSB = globalByteArray.at(5) & 0xFF;

        hzPerVolteMSB = hzPerVolteMSB << 8;
        hzPerVolteMSB = hzPerVolteMSB | hzPerVolteLSB;

        cvTrimsInterface->voltPerOctaveOffset = voltPerOctaveMSB;
        cvTrimsInterface->_1_2_voltPerOctaveOffset = _1_2_voltPerOctaveMSB;
        cvTrimsInterface->hzPerVoltOffset = hzPerVolteMSB;

        cvTrimsInterface->slotShowOffsets();
    }
    else if(global == "LED Settings")
    {
        seqDimKeysSetting->setValue(globalByteArray.at(0));
        seqBrightKeysSetting->setValue(globalByteArray.at(1));
    }

}

// load globals into a QList and then send them to QuNexus via SysEx
void MainWindow::slotEncodeGlobals()
{
    qDebug() << "slotencodeGlobals called";
    //---Called when a slider is released

    //qDebug() << "============ object name" << QObject::sender()->objectName();
    //qDebug() << reinterpret_cast<QSlider *>(QObject::sender())->value();

    QList<int> globalsList;

    //---------------------------------- Per Key Sensitivities
    QSlider* slider;

    for(int i=1; i < 26; i++)
    {
        if(i < 10)
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_0%1").arg(i));
        }
        else
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_%1").arg(i));
        }

        //qDebug() << i << "---" << slider->value();

        globalsList.append(slider->value());
    }

    //---------------------------------- Tables

    for(int i = 0; i < 128; i++)
    {
        globalsList.append(tableInterface->userTable1[i]);
    }

    for(int i = 0; i < 128; i++)
    {
        globalsList.append(tableInterface->userTable2[i]);
    }

    //---------------------------------- CV Trims and Offsets

    if (QuNexus->firmwareUpdateState) // reset these on a firmware update, not ideal but will avoid many zendesk tickets
    {
        globalsList.append(100);
        globalsList.append(100);
        globalsList.append(100);

        globalsList.append(0);
        globalsList.append(0);
        globalsList.append(0);
        fwUpdateWindow->slotAppendTextToConsole("\nSetting CV Trims to default settings...\n");
    }
    else
    {
        globalsList.append(cvTrimsInterface->voltPerOctaveTrim*100);
        globalsList.append(cvTrimsInterface->_1_2_voltPerOctaveTrim*100);
        globalsList.append(cvTrimsInterface->hzPerVoltTrim*100);

        globalsList.append(cvTrimsInterface->voltPerOctaveOffset);
        globalsList.append(cvTrimsInterface->_1_2_voltPerOctaveOffset);
        globalsList.append(cvTrimsInterface->hzPerVoltOffset);
    }

    //---------------------------------- Sequencer LED brightness

    //qDebug() << "size of globals0" << globalsList.size();

    globalsList.append((char)seqDimKeysSetting->value());
    globalsList.append((char)seqBrightKeysSetting->value());

    //qDebug() << "size of globals1" << globalsList.size();
    //qDebug() << "dimKeyVal: " << seqDimKeysSetting->value();
    //qDebug() << "brightKeyVal: " << seqBrightKeysSetting->value();

    //----------------------------------
    sysExEncDecode->slotSysExEncodeGlobal("Encode Globals", globalsList);
}

void MainWindow::slotResetPerKeySensitivities()
{
    qDebug() << "slotResetPerKeySensitivities called";
    QList<int> sensitivityList;

    QSlider* slider;

    for(int i=1; i < 26; i++)
    {
        if(i < 10)
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_0%1").arg(i));
        }
        else
        {
            slider = pksWidget->findChild<QSlider *>(QString("slider_%1").arg(i));
        }

        slider->setValue(64);
        sensitivityList.append(slider->value());
    }

    slotEncodeGlobals();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// Menu Bar /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------ Menu Bar Setup & Connections ------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------//

void MainWindow::slotOpenPresetDirectory()
{
    QString presetDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDesktopServices::openUrl(QUrl::fromLocalFile(presetDir));
}

void MainWindow::slotSetupMenuBar()
{
    menubar = new QMenuBar(this);

#ifndef Q_OS_MAC
    menubar->setGeometry(0,0,this->width(), MENU_BAR_HEIGHT);
    QFile menuStyleFile = QFile(":stylesheets/resources/stylesheets/MenuBarWinStyleSheet.qss");
    QString menuStyleString;
    qDebug() << "Menubar stylestring: " << menuStyleString;

    menuStyleFile.open(QFile::ReadOnly);
    menuStyleString = QLatin1String(menuStyleFile.readAll());

    menubar->setStyleSheet(menuStyleString);
    //menubar->setStyleSheet("color:white;");

    //qDebug() << "menubar stylesheet: " << menubar->styleSheet();

#endif

    //---------------------------------------------------- File ----------------------------------------------------//

    QMenu* file = new QMenu("File");

    QAction* newPreset = new QAction("New Preset...", file);
    connect(newPreset, SIGNAL(triggered()), presetManager, SLOT(slotSaveNewPreset()));
    newPreset->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    file->addAction(newPreset);

    QAction* savePreset = new QAction("Save Preset", file);
    connect(savePreset, SIGNAL(triggered()), presetManager, SLOT(slotSaveSelectedPreset()));
    savePreset->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    file->addAction(savePreset);

    QAction* deletePreset = new QAction("Delete Preset", file);
    connect(deletePreset, SIGNAL(triggered()), this, SLOT(slotDeletePresetQuery()));
    deletePreset->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    file->addAction(deletePreset);

    QAction* revertPreset = new QAction("Revert Preset", file);
    connect(revertPreset, SIGNAL(triggered()), this, SLOT(slotRevertPreset()));
    revertPreset->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    file->addAction(revertPreset);

    file->addSeparator();


    //--------------------------------------------------------------- Define Factory Presets ------------------------------------------------------------------//
#define NUM_FACT_PRESETS 6
    QString factoryPresetName[NUM_FACT_PRESETS] = // these strings need to match the filenames in the resources directory
    {
        "Basic 2.0",
        "Aftertouch and Modulation",
        "MPE and Sequencer",
        "Drums and Clips 2.0",
        "Reason Controls",
        "SymphonixEvolution"
    };
//    QAction* revertFactoryAction[NUM_FACT_PRESETS];
    QAction* importFactoryAction[NUM_FACT_PRESETS];

    //--------------------------------------------------------------- Import Factory Presets ---------------------------------------------------------------//
    QMenu* importFactory = new QMenu("Import Factory Preset");
    file->addMenu(importFactory);

    for (int i = 0; i < NUM_FACT_PRESETS; i++)
    {
        importFactoryAction[i] = new QAction(factoryPresetName[i], file);
        importFactoryAction[i]->setObjectName(factoryPresetName[i]);
        connect(importFactoryAction[i], SIGNAL(triggered()), presetManager, SLOT(slotImportFactory()));
        importFactory->addAction(importFactoryAction[i]);
    }

    //--------------------------------------------------------------- Revert To Factory Presets ---------------------------------------------------------------//
    QAction* revertToFactory = new QAction("Reset Factory Presets to Defaults", file);
    connect(revertToFactory, SIGNAL(triggered()), this, SLOT(slotResetFactoryPresets()));
    file->addAction(revertToFactory);

    file->addSeparator();

    //---------------------------------------------------------------------- Import / Export -----------------------------------------------------------------//
    QAction* exportPreset = new QAction("Export Preset", file);
    connect(exportPreset, SIGNAL(triggered()), presetManager, SLOT(slotExportPreset()));
    //exportPreset->setEnabled(false);
    file->addAction(exportPreset);

    QAction* importPreset = new QAction("Import Preset", file);
    importPreset->setObjectName("importPreset");
    connect(importPreset, SIGNAL(triggered()), presetManager, SLOT(slotImportPreset()));
    //importPreset->setEnabled(false);
    file->addAction(importPreset);

    openAppDataDir = new QAction("Open Editor Preset Directory", file);
    openAppDataDir->setObjectName("openAppDataDir");
    file->addAction(openAppDataDir);

    connect(openAppDataDir, SIGNAL(triggered()), this, SLOT(slotOpenPresetDirectory()));

    menubar->addMenu(file);


    //---------------------------------------------------- Edit ----------------------------------------------------//

    QMenu* edit = new QMenu(" Edit");

    //------- Copy Controller Sensor
    QAction* copyControllerSensor= new QAction("Copy Controller Sensor", edit);
    connect(copyControllerSensor, SIGNAL(triggered()), this, SLOT(slotCopyControllerElements()));
    copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    edit->addAction(copyControllerSensor);

    //------- Paste Controller Sensor
    pasteControllerSensor= new QAction("Paste Controller Sensor", edit);
    connect(pasteControllerSensor, SIGNAL(triggered()), this, SLOT(slotPasteControllerElements()));
    pasteControllerSensor->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    edit->addAction(pasteControllerSensor);
    pasteControllerSensor->setEnabled(false);

    edit->addSeparator();

    QMenu* copyPasteLayer = new QMenu("Copy/Paste Layer");
    edit->addMenu(copyPasteLayer);

    //------- Copy Controller Layer
    QAction* copyControllerLayer = new QAction("Copy Controller Layer", edit);
    connect(copyControllerLayer, SIGNAL(triggered()), this, SLOT(slotCopyLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(copyControllerLayer);

    //------- Paste Controller Layer
    pasteControllerLayer = new QAction("Paste Controller Layer", edit);
    connect(pasteControllerLayer, SIGNAL(triggered()), this, SLOT(slotPasteLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(pasteControllerLayer);
    pasteControllerLayer->setEnabled(false);

    copyPasteLayer->addSeparator();

    //------- Copy Keyboard Layer
    QAction* copyKeyboardLayer = new QAction("Copy Keyboard Layer", edit);
    connect(copyKeyboardLayer, SIGNAL(triggered()), this, SLOT(slotCopyLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(copyKeyboardLayer);

    //------- Paste Keyboard Layer
    pasteKeyboardLayer = new QAction("Paste Keyboard Layer", edit);
    connect(pasteKeyboardLayer, SIGNAL(triggered()), this, SLOT(slotPasteLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(pasteKeyboardLayer);
    pasteKeyboardLayer->setEnabled(false);
    copyPasteLayer->addSeparator();

    //------- Copy CV Layer
    QAction* copyCVLayer = new QAction("Copy CV Layer", edit);
    connect(copyCVLayer, SIGNAL(triggered()), this, SLOT(slotCopyLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(copyCVLayer);

    //------- Paste CV Layer
    pasteCVLayer = new QAction("Paste CV Layer", edit);
    connect(pasteCVLayer, SIGNAL(triggered()), this, SLOT(slotPasteLayer()));
    //copyControllerSensor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyPasteLayer->addAction(pasteCVLayer);
    pasteCVLayer->setEnabled(false);
    menubar->addMenu(edit);




    //----------------------------------------------------- View ------------------------------------------------------//

    QMenu* view = new QMenu("View");
    QAction* showHideControllerTab;

    //Manage controller tab display states with QSettings
    if(!settings->value("controllerTabVisible").toInt())
    {
        //tabWidget->removeTab(1);
        showHideControllerTab = new QAction("Show Controller Tab", view);
    }
    else
    {
        showHideControllerTab = new QAction("Hide Controller Tab", view);
    }

    tabWidget->setCurrentIndex(settings->value("tabIndex").toInt());
    connect(showHideControllerTab, SIGNAL(triggered()), this, SLOT(slotShowHideControllerTab()));
    view->addAction(showHideControllerTab);

    //menubar->addMenu(view);


    //---------------------------------------------------- Window ----------------------------------------------------//

    //QMenu* window = new QMenu("Window");
    //menubar->addMenu(window);


    //---------------------------------------------------- Hardware ----------------------------------------------------//

    //Setup Hardware Menu
    QMenu* hardware = new QMenu("Hardware");

    //Add the update firmware action
    updateFirmware = new QAction("Update Firmware...", hardware);
    connect(updateFirmware, SIGNAL(triggered()), this, SLOT(slotForceFirmwareUpdate()));
    hardware->addAction(updateFirmware);

    hardware->addSeparator();

    //----------------------------------------------------------------- Load presets from QuNexus

    //Create Submenu
    loadHardwarePreset = new QMenu("Load Preset");
    hardware->addMenu(loadHardwarePreset);

    //Create an action group to hold our submenu items
    loadHardwarePresetGroup = new QActionGroup(hardware);

    //--------- Create Submenu Actions

    loadPresetSlotA = new QAction("Slot A", loadHardwarePresetGroup);
    loadPresetSlotA->setObjectName("loadPresetSlotA");
    loadHardwarePreset->addAction(loadPresetSlotA);

    loadPresetSlotB = new QAction("Slot B", loadHardwarePresetGroup);
    loadPresetSlotB->setObjectName("loadPresetSlotB");
    loadHardwarePreset->addAction(loadPresetSlotB);

    loadPresetSlotC = new QAction("Slot C", loadHardwarePresetGroup);
    loadPresetSlotC->setObjectName("loadPresetSlotC");
    loadHardwarePreset->addAction(loadPresetSlotC);

    loadPresetSlotD = new QAction("Slot D", loadHardwarePresetGroup);
    loadPresetSlotD->setObjectName("loadPresetSlotD");
    loadHardwarePreset->addAction(loadPresetSlotD);

    connect(loadHardwarePreset, SIGNAL(triggered(QAction*)), this, SLOT(slotLoadPresetSlot(QAction*)));

    //Add Hardware Factory Reset
    hardwareFactoryReset = new QAction("Factory Reset", hardware);
    connect(hardwareFactoryReset, SIGNAL(triggered()), this, SLOT(slotHardwareFactoryReset()));
    hardware->addAction(hardwareFactoryReset);


    hardware->addSeparator();
    //----------------------------------------------------------------- LED modes

    //Create Submenu "LED Refresh Mode"
    ledRefreshMode = new QMenu("LED Refresh Mode");
    hardware->addMenu(ledRefreshMode);

    //Create an action group to hold our submenu items, selectable radiobutton style
    ledRefreshModeGroup = new QActionGroup(hardware);

    //----------------------------------------------------------------- Create Submenu Actions

    //Normal
    ledRefreshNormal = new QAction("Normal", ledRefreshMode);
    ledRefreshNormal->setObjectName("Normal");
    connect(ledRefreshNormal, SIGNAL(triggered()), this, SLOT(slotSetLedRefreshMode()));
    ledRefreshMode->addAction(ledRefreshNormal);
    ledRefreshNormal->setCheckable(true);

    //Silent
    ledRefreshSilent = new QAction("Control Only", ledRefreshMode);
    ledRefreshSilent->setObjectName("Control Only");
    connect(ledRefreshSilent, SIGNAL(triggered()), this, SLOT(slotSetLedRefreshMode()));
    ledRefreshMode->addAction(ledRefreshSilent);
    ledRefreshSilent->setCheckable(true);

    //All Off
    ledRefreshAllOff = new QAction("All Off", ledRefreshMode);
    ledRefreshAllOff->setObjectName("All Off");
    connect(ledRefreshAllOff, SIGNAL(triggered()), this, SLOT(slotSetLedRefreshMode()));
    ledRefreshMode->addAction(ledRefreshAllOff);
    ledRefreshAllOff->setCheckable(true);

    //Set above group of actions to behave exclusively (like radio buttons)
    ledRefreshModeGroup->setExclusive(true);

    //Add the actions to the group
    ledRefreshModeGroup->addAction(ledRefreshNormal);
    ledRefreshModeGroup->addAction(ledRefreshSilent);
    ledRefreshModeGroup->addAction(ledRefreshAllOff);

    hardware->addSeparator();

    //Add the update firmware action
    perKeySensitivityAction = new QAction("Per Key Sensitivities...", hardware);
    connect(perKeySensitivityAction, SIGNAL(triggered()), this, SLOT(slotOpenPerKeySensitivitiesWindow()));
    hardware->addAction(perKeySensitivityAction);

    //Add CV Trims action
    cvTrims = new QAction("CV Trims...", hardware);
    connect(cvTrims, SIGNAL(triggered()), this, SLOT(slotOpenCvTrimsInterface()));
    hardware->addAction(cvTrims);

    viewAndEditTables= new QAction("Tables...", hardware);
    connect(viewAndEditTables, SIGNAL(triggered()), this, SLOT(slotOpenTableInterface()));
    hardware->addAction(viewAndEditTables);

    menubar->addMenu(hardware);

    //Set menu to initially selected preset's led referesh style

    QString refreshVal = presetManager->editBufferPresetMap.value("Globals_LED_Refresh_Style").toString();

    //qDebug() << "--------------" << refreshVal;

    if(refreshVal == "Normal")
    {
        ledRefreshNormal->setChecked(true);
    }
    else if(refreshVal == "Control Only")
    {
        ledRefreshSilent->setChecked(true);
    }
    else if(refreshVal == "All Off")
    {
        ledRefreshAllOff->setChecked(true);
    }

    //------------------------------------------------------ Help ------------------------------------------------------//

    QMenu* help = new QMenu("Help");

    //About Menu
    QAction* about = new QAction("About", file);
    connect(about, SIGNAL(triggered()), this, SLOT(slotOpenAboutScreen()));
    help->addAction(about);

    //Troubleshooting
    QAction* troubleShooting = new QAction("Troubleshoot Connection", help);
    connect(troubleShooting, SIGNAL(triggered()), this, SLOT(slotOpenTroubleshooting()));
    connect(ui->QuNexus_Connected_Label, SIGNAL(pressed()), this, SLOT(slotOpenTroubleshooting()));
    help->addAction(troubleShooting);

    //Bug Report
    QAction* bugReport = new QAction("Submit Bug Report", help);
    connect(bugReport, SIGNAL(triggered()), this, SLOT(slotSubmitBugReport()));
    help->addAction(bugReport);

    //Documentation
    QAction* doc = new QAction("Documentation", help);
    connect(doc, SIGNAL(triggered()), this, SLOT(slotOpenDocumentation()));
    help->addAction(doc);

    help->addSeparator();

    //Tool tips
    if(settings->contains("toolTipsEnabled"))
    {
        if(settings->value("toolTipsEnabled").toBool())
        {
            toolTipsEnable = new QAction("Hide Tool Tips", help);
        }
        else
        {
            toolTipsEnable = new QAction("Show Tool Tips", help);
        }
    }
    else
    {
        settings->setValue("toolTipsEnabled", true);
        toolTipsEnable = new QAction("Hide Tool Tips", help);
    }

    connect(toolTipsEnable, SIGNAL(triggered()), this, SLOT(slotEnableDisableToolTips()));

    help->addAction(toolTipsEnable);

//#ifdef Q_OS_WIN
//    // Scale Display
//    if(settings->contains("scaleDisplayUp"))
//    {
//        if(settings->value("scaleDisplayUp").toBool())
//        {
//            scaleDisplayUp = new QAction("Set Display Scale to Default", help);
//        }
//        else
//        {
//            scaleDisplayUp = new QAction("Set Display Scale to Large", help);
//        }
//    }
//    else
//    {
//        settings->setValue("scaleDisplayUp", false);
//        scaleDisplayUp = new QAction("Set Display Scale to Large", help);
//    }

//    connect(scaleDisplayUp, SIGNAL(triggered()), this, SLOT(slotScaleDisplayUp()));

//    help->addAction(scaleDisplayUp);
//#endif

    //Warn when loading and discarding unsaved changes
    if(settings->contains("unsavedWarningsEnabled"))
    {
        if(settings->value("unsavedWarningsEnabled").toBool())
        {
            unsavedWarningsEnabled = new QAction("Disable Unsaved Change Warnings", help);
        }
        else
        {
            unsavedWarningsEnabled = new QAction("Enable Unsaved Change Warnings", help);
        }
    }
    else
    {
        settings->setValue("unsavedWarningsEnabled", true);
        unsavedWarningsEnabled = new QAction("Disable Unsaved Change Warnings", help);
    }

    connect(unsavedWarningsEnabled, SIGNAL(triggered()), this, SLOT(slotEnableDisableUnsavedWarnings()));

    help->addAction(unsavedWarningsEnabled);

    QAction *updateCheckAct = new QAction("Check for Updates...", help);
    connect(updateCheckAct, SIGNAL(triggered()), checkUpdates, SLOT(slotManualCheckForUpdates()));
    help->addAction(updateCheckAct);
    updateCheckAct->setEnabled(true);

    menubar->addMenu(help);

}

//------------------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------- Menu Bar Functions --------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------------------------//
void MainWindow::slotOpenDocumentation()
{
    QString manualURL = "https://files.keithmcmillen.com/products/qunexus/Manuals/QuNexus_RED_Manual_V2.pdf";

    QDesktopServices::openUrl(QUrl(manualURL));

}

void MainWindow::slotSubmitBugReport()
{
    QString manualURL = "https://keithmcmillen.zendesk.com/hc/en-us/requests/new";

    QDesktopServices::openUrl(QUrl(manualURL));

}

void MainWindow::slotOpenTroubleshooting()
{
    troubleshootWindow->show();
    troubleshootWindow->slotScrollTroubleUp();
}

void MainWindow::slotShowHideControllerTab()
{
    QAction* action = reinterpret_cast<QAction *>(QObject::sender());

    //Store Controller Tab display state
    if(action->text().contains("Show"))
    {
        tabWidget->insertTab(1, controllerTab, "Controller ");
        tabWidget->setCurrentIndex(1);
        action->setText("Hide Controller Tab");
        settings->setValue("controllerTabVisible", 1);
    }
    else
    {
        tabWidget->removeTab(1);
        action->setText("Show Controller Tab");
        settings->setValue("controllerTabVisible", 0);
    }
}

void MainWindow::slotCopyControllerElements()
{
    //If only one sensor/element
    if(presetManager->controllerLayerSelectedComponents.count() == 1)
    {
        //Clear the copied map
        controllerElementCopiedMap.clear();

        //If selected button is not a bend pad
        if(!presetManager->controllerLayerSelectedComponents.at(0).contains("Bend"))
        {
            //Derive selected element's number/ID
            QString elementId = presetManager->controllerLayerSelectedComponents.at(0);
            elementId.remove(QString("Key_Highlight_"));
            qDebug() << "id" << elementId;

            //Store in generic controller copied map
            controllerElementCopiedMap.insert(QString("Channel"), presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Channel").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Keyboard_Layer_Enable_Disable"),presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Keyboard_Layer_Enable_Disable").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Controller_Layer_Enable_Disable"), presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Controller_Layer_Enable_Disable").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Mode_Pressure_Note"),presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Mode_Pressure_Note").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Note_Number"), presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Note_Number").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Note_Velocity"), presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Note_Velocity").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Pressure_Control_Number"),presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Pressure_Control_Number").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Pressure_Control_Value"),presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Pressure_Control_Value").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Tilt_Control_Number"),presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Tilt_Control_Number").arg(elementId)));
            controllerElementCopiedMap.insert(QString("Tilt_Control_Value"), presetManager->editBufferPresetMap.value(QString("Controller_Key_%1_Tilt_Control_Value").arg(elementId)));
        }

        //If selected button is a bend pad
        else
        {
            //Store in copied map (no ID needed for one-of-a-kind bend pad)
            controllerElementCopiedMap.insert(QString("Channel"), presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Channel")));
            controllerElementCopiedMap.insert(QString("Keyboard_Layer_Enable_Disable"),presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable")));
            controllerElementCopiedMap.insert(QString("Controller_Layer_Enable_Disable"), presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Controller_Layer_Enable_Disable")));
            controllerElementCopiedMap.insert(QString("Mode_Pressure_Note"),presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Mode_Pressure_Note")));
            controllerElementCopiedMap.insert(QString("Note_Number"), presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Note_Number")));
            controllerElementCopiedMap.insert(QString("Note_Velocity"), presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Note_Velocity")));
            controllerElementCopiedMap.insert(QString("Pressure_Control_Number"),presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Pressure_Control_Number")));
            controllerElementCopiedMap.insert(QString("Pressure_Control_Value"),presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Pressure_Control_Value")));
            controllerElementCopiedMap.insert(QString("Tilt_Control_Number"),presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Tilt_Control_Number")));
            controllerElementCopiedMap.insert(QString("Tilt_Control_Value"), presetManager->editBufferPresetMap.value(QString("Controller_Bend_Pad_00_Tilt_Control_Value")));
        }
    }

    pasteControllerSensor->setEnabled(true);

}

void MainWindow::slotPasteControllerElements()
{
    //Iterate through selected components
    for(int i = 0; i < presetManager->controllerLayerSelectedComponents.count(); i++)
    {

        qDebug() << "active?" << presetManager->controllerLayerSelectedComponents.at(i);

        //If a key (not bend)
        if(!presetManager->controllerLayerSelectedComponents.at(i).contains("Bend"))
        {
            //Get key instance / index
            QString elementId = presetManager->controllerLayerSelectedComponents.at(i);
            elementId.remove(QString("Key_Highlight_"));
            qDebug() << "ElementID: " << elementId;

            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Channel").arg(elementId), controllerElementCopiedMap.value(QString("Channel")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Keyboard_Layer_Enable_Disable").arg(elementId), controllerElementCopiedMap.value(QString("Keyboard_Layer_Enable_Disable")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Controller_Layer_Enable_Disable").arg(elementId), controllerElementCopiedMap.value(QString("Controller_Layer_Enable_Disable")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Mode_Pressure_Note").arg(elementId), controllerElementCopiedMap.value(QString("Mode_Pressure_Note")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Note_Number").arg(elementId), controllerElementCopiedMap.value(QString("Note_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Note_Velocity").arg(elementId), controllerElementCopiedMap.value(QString("Note_Velocity")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Pressure_Control_Number").arg(elementId), controllerElementCopiedMap.value(QString("Pressure_Control_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Pressure_Control_Value").arg(elementId), controllerElementCopiedMap.value(QString("Pressure_Control_Value")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Tilt_Control_Number").arg(elementId), controllerElementCopiedMap.value(QString("Tilt_Control_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Key_%1_Tilt_Control_Value").arg(elementId), controllerElementCopiedMap.value(QString("Tilt_Control_Value")));
        }

        //If bend button
        else
        {
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Channel"), controllerElementCopiedMap.value(QString("Channel")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable"), controllerElementCopiedMap.value(QString("Keyboard_Layer_Enable_Disable")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Controller_Layer_Enable_Disable"), controllerElementCopiedMap.value(QString("Controller_Layer_Enable_Disable")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Mode_Pressure_Note"), controllerElementCopiedMap.value(QString("Mode_Pressure_Note")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Note_Number"), controllerElementCopiedMap.value(QString("Note_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Note_Velocity"), controllerElementCopiedMap.value(QString("Note_Velocity")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Pressure_Control_Number"), controllerElementCopiedMap.value(QString("Pressure_Control_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Pressure_Control_Value"), controllerElementCopiedMap.value(QString("Pressure_Control_Value")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Tilt_Control_Number"), controllerElementCopiedMap.value(QString("Tilt_Control_Number")));
            presetManager->editBufferPresetMap.insert(QString("Controller_Bend_Pad_00_Tilt_Control_Value"), controllerElementCopiedMap.value(QString("Tilt_Control_Value")));
        }
    }

    qDebug() << "copied" << controllerElementCopiedMap;

    presetManager->jsonMasterMapEDITABLE.insert(presetManager->slotGetPresetNameFromIndex(presetManager->selectedLibraryPreset), presetManager->editBufferPresetMap);

    //Update UI
    slotUpdateControllerLayerHighlightUi(presetManager->controllerLayerSelectedComponents);
    slotUpdateLibraryPresetsListUi(QVariantMap(), SET_MODIFIED, 0);
}

void MainWindow::slotCopyLayer()
{
    //Reinterpret sender object to QAction to get name/layer
    QAction* action = reinterpret_cast<QAction *>(QObject::sender());
    QString layerKey = action->text();

    if(layerKey.contains("Controller"))
    {
        layerKey = QString("Controller");
        pasteControllerLayer->setEnabled(true);
    }

    else if(layerKey.contains("Keyboard"))
    {
        layerKey = QString("Keyboard");
        pasteKeyboardLayer->setEnabled(true);
    }

    else if(layerKey.contains("CV"))
    {
        layerKey = QString("CV");
        pasteCVLayer->setEnabled(true);
    }

    //Layer to store on the clipboard
    copiedLayerMap.clear();

    //Iterator to copy frome edit buffer
    QMapIterator<QString, QVariant> i(presetManager->editBufferPresetMap);

    while(i.hasNext())
    {
        i.next();

        if(i.key().startsWith(layerKey))
        {
            copiedLayerMap.insert(i.key(), i.value());
        }
    }
}

void MainWindow::slotPasteLayer()
{

    //Iterator to copy frome edit buffer
    QMapIterator<QString, QVariant> i(copiedLayerMap);

    while(i.hasNext())
    {
        i.next();

        qDebug() << "[key: " << i.key() << "] , [value : " << i.value() << "]";

        presetManager->editBufferPresetMap.insert(i.key(), i.value());
    }

    presetManager->jsonMasterMapEDITABLE.insert(presetManager->slotGetPresetNameFromIndex(presetManager->selectedLibraryPreset), presetManager->editBufferPresetMap);

    slotUpdateParametersUi(presetManager->editBufferPresetMap);
    slotUpdateControllerLayerHighlightUi(presetManager->controllerLayerSelectedComponents);
    slotUpdateLibraryPresetsListUi(presetManager->editBufferPresetMap, SET_MODIFIED, 0);

}


//---------------------------------------------------------------- Per Key Sensitivities --------------------------------------------------------------//
void MainWindow::slotOpenPerKeySensitivitiesWindow()
{
    slotDisableGlobalsWindows();
    pksWidget->show();
    //QTimer::singleShot(500, this, SLOT(slotSendGlobalsRequest()));
    slotSendGlobalsRequest();
}

void MainWindow::slotOpenTableInterface()
{
    qDebug() << "slotOpenTableInterface called";
    slotDisableGlobalsWindows();
    qDebug() << "show";
    tableInterface->show();
    //QTimer::singleShot(500, this, SLOT(slotSendGlobalsRequest()));
    qDebug() << "request globals";
    slotSendGlobalsRequest();
}

void MainWindow::slotOpenCvTrimsInterface()
{
    slotDisableGlobalsWindows();
    cvTrimsInterface->show();
    //QTimer::singleShot(500, this, SLOT(slotSendGlobalsRequest()));
    slotSendGlobalsRequest();
}

void MainWindow::slotDisableGlobalsWindows()
{
    qDebug() << "slotDisableGlobalsWindows called";
    //sensitivitiesUi->masterFrame->show();
    //qDebug() << "table disable";
    tableInterface->slotEnable(false);
    //qDebug() << "cvtrims disable";
    cvTrimsInterface->slotEnable(false);
}

void MainWindow::slotEnableGlobalsWindows()
{
    qDebug() << "slotEnableGlobalsWindows called";
    sensitivitiesUi->masterFrame->hide();
    tableInterface->slotEnable(true);
    cvTrimsInterface->slotEnable(true);
}

void MainWindow::slotSendGlobalsRequest()
{
    qDebug() << "slotSendGlobalsRequest called";

    //This Query also returns the user tables

    unsigned char sens[17] = {0xF0 , 0x00 , 0x01 , 0x5F , 0x7A , 0x19 , 0x00 , 0x01 , 0x00 , 0x02 , 0x50 , 0x01 , 0x74 , 0x3E , 0x00 , 0x10 , 0xF7};

    // MIDI overhaul
    QuNexus->slotSendSysEx(sens, 17);

    // connecting this to sysExEncDecode::slotGlobalsReceived
    //QTimer::singleShot(3000, this, SLOT(slotEnableGlobalsWindows()));
}

// --- Request Preset packet ------------------------------------------

// going to try this in mainWindow and avoid having to write it twice for win/mac
void MainWindow::slotRequestPreset(unsigned char preset)
{
    // MIDI Overhaul
    unsigned char request[25] = { 240, 0, 1, 95, 122, 25, 0, 1, 0, 2, 80, 2, 0, 0, 0, 0, preset, 0, 0, 0, 0, 0, 0, 0, 247};
    QuNexus->slotSendSysEx(&request[0], sizeof(request));
}

void MainWindow::slotLoadPresetSlot(QAction *menuItem)
{
    qDebug() << "slotLoadPresetSlot called - menuItem: " << menuItem->objectName();
    unsigned char thisPreset;

    if (menuItem->objectName() == "loadPresetSlotA")
    {
        thisPreset = 0;
    }
    else if (menuItem->objectName() == "loadPresetSlotB")
    {
        thisPreset = 1;
    }
    else if (menuItem->objectName() == "loadPresetSlotC")
    {
        thisPreset = 2;
    }
    else if (menuItem->objectName() == "loadPresetSlotD")
    {
        thisPreset = 3;
    }
    else
    {
        return;
    }
    slotRequestPreset(thisPreset);
}


void MainWindow::slotResetFactoryPresets()
{
    presetManager->presetLoading = true;
    presetManager->slotRevertSelectedPreset();

    presetManager->slotResetFactoryPresets();

    presetManager->slotRevertSelectedPreset();
    presetManager->presetLoading = false;

    presetManager->slotLoadLibraryPresetIntoEditBuffer(libraryPresetsList->item(0));
    libraryPresetsList->setCurrentItem(libraryPresetsList->item(0));

}

void MainWindow::slotHardwareFactoryReset()
{
    presetManager->presetLoading = true;
    presetManager->slotRevertSelectedPreset();
    ui->Globals_On_Thresh->setValue(20);
    ui->Globals_Off_Thresh->setValue(15);
    ui->Globals_On_Thresh->editingFinished();
    ui->Globals_Off_Thresh->editingFinished();
    presetManager->slotUpdateKeyOnOffThresholds();

    presetManager->presetLoading = false;
    slotResetPerKeySensitivities();
    cvTrimsInterface->slotResetVars();
    presetManager->slotSendFactoryPresetsToQuNexus();

    presetManager->slotRevertSelectedPreset();
    presetManager->slotLoadLibraryPresetIntoEditBuffer(libraryPresetsList->item(0));
    libraryPresetsList->setCurrentItem(libraryPresetsList->item(0));
    presetManager->slotRevertSelectedPreset();
}

void MainWindow::slotSetLedRefreshMode()
{
    presetManager->editBufferPresetMap.insert("Globals_LED_Refresh_Style", QObject::sender()->objectName());
    //settings->setValue("ledRefreshStyle", ledRefreshModeGroup->checkedAction()->objectName());

    //If a preset is selected
    if(presetManager->selectedLibraryPreset >= 0)
    {
        //Insert edit buffer into editable json map
        presetManager->jsonMasterMapEDITABLE.insert(presetManager->slotGetPresetNameFromIndex(presetManager->selectedLibraryPreset), presetManager->editBufferPresetMap);
    }

    slotUpdateLibraryPresetsListUi(QVariantMap(), SET_MODIFIED, 0);
}

void MainWindow::slotEnableDisableMidiFunctions(bool connected)
{
    QPushButton* syncButtons;

    if(connected)
    {
        //updateFirmware->setEnabled(true);
        ledRefreshMode->setEnabled(true);
        perKeySensitivityAction->setEnabled(true);
        cvTrims->setEnabled(true);
        viewAndEditTables->setEnabled(true);
        for(int i=0; i<4; i++)
        {
            syncButtons = this->findChild<QPushButton *>(QString("syncPreset%1").arg(i));
            syncButtons->setEnabled(true);
        }
    }
    else
    {
        pksWidget->close();

        //updateFirmware->setEnabled(false);
        ledRefreshMode->setEnabled(false);
        perKeySensitivityAction->setEnabled(false);
        cvTrims->setEnabled(false);
        viewAndEditTables->setEnabled(false);
        for(int i=0; i<4; i++)
        {
            syncButtons = this->findChild<QPushButton *>(QString("syncPreset%1").arg(i));
            syncButtons->setEnabled(false);
        }
    }
}

void MainWindow::slotEnableDisableToolTips()
{
    if(settings->value("toolTipsEnabled").toBool())
    {
        toolTipsEnable->setText("Show Tool Tips");
        settings->setValue("toolTipsEnabled", false);
    }
    else
    {
        toolTipsEnable->setText("Hide Tool Tips");
        settings->setValue("toolTipsEnabled", true);
    }
}

void MainWindow::slotEnableDisableUnsavedWarnings()
{

    if(settings->value("unsavedWarningsEnabled").toBool())
    {
        unsavedWarningsEnabled->setText("Enable Unsaved Change Warnings");
        settings->setValue("unsavedWarningsEnabled", false);
    }
    else
    {
        unsavedWarningsEnabled->setText("Disable Unsaved Change Warnings");
        settings->setValue("unsavedWarningsEnabled", true);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////// Key Filters ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() == Qt::ShiftModifier)
    {
        presetManager->shiftDown = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if(!event->modifiers().operator &(Qt::ShiftModifier))
    {
        presetManager->shiftDown = false;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

//    if (obj->objectName().contains("Global_Polyphony"))
//    {
//        qDebug() << "poly event: " << event->type();
//    }

    if(event->type() == QEvent::ToolTip)
    {
        if(settings->value("toolTipsEnabled").toBool())
        {
            //qDebug() << "show tooltip - object: " << reinterpret_cast<QWidget *>(obj)->objectName() << " tooltip: " << reinterpret_cast<QWidget *>(obj)->toolTip() << " Stylesheet: " << reinterpret_cast<QWidget *>(obj)->styleSheet();
            return QObject::eventFilter(obj, event);
        }
        else
        {
            //qDebug() << "hide tooltip - object: " << reinterpret_cast<QWidget *>(obj)->objectName() << " tooltip: " << reinterpret_cast<QWidget *>(obj)->toolTip() << " Stylesheet: " << reinterpret_cast<QWidget *>(obj)->styleSheet();
            return true;
        }
    }

    if(event->type() == QEvent::KeyPress){
        QKeyEvent *keysPressed;
        keysPressed = (QKeyEvent *)event;
        if(keysPressed->matches(QKeySequence::Copy)){
            slotCopyControllerElements();
        }
        if(keysPressed->matches(QKeySequence::Paste)){
            slotPasteControllerElements();
        }
        if (keysPressed->key() == Qt::Key_Enter ||
            keysPressed->key() == Qt::Key_Return ||
            keysPressed->key() == Qt::Key_Tab ||
            keysPressed->key() == Qt::Key_Escape)
        {
            //qDebug() << "eventFilter - key: " << keysPressed->key();
            slotClearFocus(); // get out of the number/entry box
        }
    }


    if (event->type() == QEvent::MouseButtonPress)
    {
        //slotClearFocus();
        //qDebug() << "Clicked: " << obj->objectName();

        if (obj->objectName() == "MainWindowWindow")
        {
            slotClearFocus(); // clear focus when clicking on any other part of the window
        }

    }

    if (event->type() == QEvent::FocusIn)
    {
        //qDebug() << "FocusIn: " << obj->objectName();
        if(QSpinBox* numberBox = this->findChild<QSpinBox *>(obj->objectName()))
        {
            //qDebug() << "focusin numbox: " << obj->objectName();
            QTimer::singleShot(3, [numberBox](){
                    numberBox->findChild<QLineEdit*>()->selectAll(); // select all contents on click
                });

            //numberBox->findChild<QLineEdit*>()->selectAll();
        }
        else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(obj->objectName()))
        {
            QTimer::singleShot(3, [floatBox](){
                floatBox->findChild<QLineEdit*>()->selectAll(); // select all contents on click
                });
        }
    }

    return false;
}

void MainWindow::slotToolTipHandler()
{
    // install eventfilter on all QuNexus parameters
    QMapIterator<QString, QVariant> i(presetManager->defaultParamMap);

    while(i.hasNext())
    {
        i.next();

        if(QComboBox* menu = this->findChild<QComboBox *>(i.key()))
        {
            menu->installEventFilter(this);
        }
        else if(QSpinBox* numberBox = this->findChild<QSpinBox *>(i.key()))
        {
            //qDebug() << "Install tooltip numberbox: " << numberBox->objectName();
            numberBox->installEventFilter(this);
            //numberBox->setMouseTracking(true);
        }
        else if(QDoubleSpinBox* floatBox = this->findChild<QDoubleSpinBox *>(i.key()))
        {
            floatBox->installEventFilter(this);
            floatBox->setMouseTracking(true);
        }
        else if(QCheckBox* checkBox = this->findChild<QCheckBox *>(i.key()))
        {
            checkBox->installEventFilter(this);
        }
    }

    // ui elements not in defaultParamList
    ui->midi_outputs->installEventFilter(this);
    ui->deletePreset->installEventFilter(this);
    ui->newPreset->installEventFilter(this);
    ui->revertButton->installEventFilter(this);
    ui->saveButton->installEventFilter(this);
    ui->presetLibraryList->installEventFilter(this);
    ui->tt_keyboardTab->installEventFilter(this);
    ui->tt_cvTab->installEventFilter(this);
    ui->tt_ctlTab->installEventFilter(this);
    ui->tt_settingsTab->installEventFilter(this);
    ui->tt_seqTab->installEventFilter(this);
    ui->Send_Frame->installEventFilter(this);
    ui->QuNexus_Connected_Label->installEventFilter(this);
}


void MainWindow::slotSendActiveSense()
{
    //qDebug() << "slotSendActiveSense called";
    if (sysExEncDecode->sx_xmit_active || !QuNexus->port_out_open)
    {
        //qDebug() << "activeSense called - sx_xmit_active: " << sysExEncDecode->sx_xmit_active << " connected: " << connected ;
        return; // do not interrupt sysex, only send if connected
    }

    QuNexus->slotSendMIDI(MIDI_RT_ACTIVE_SENSE);
}

void MainWindow::slotSyncQnToEditorNewPreset()
{
    qDebug() << "slotSyncQnToEditorNewPreset called, setting JSONLoading = true";
    presetManager->JSONLoading = true; // mute any updates to QuNexus
    presetManager->slotRevertSelectedPreset(); // revert without changing flags
    presetManager->slotSaveNewPreset();
    presetManager->JSONLoading = false; // unmute
    qDebug() << "setting JSONLoading = false)";
    slotSyncQnToEditor();
}

void MainWindow::slotSyncQnToEditor()
{
    qDebug() << "slotSyncQnToEditor called";
    // need to make the sysex connection here before we sync
    if (connected)
    {
        QuNexus->disconnect(SIGNAL(signalRxSysExBA(QByteArray)));
    }
    connected = true;

    // Connect non-fw sysex
    connect(QuNexus, SIGNAL(signalRxSysExBA(QByteArray)), sysExEncDecode, SLOT(slotProcessSysEx(QByteArray)));

    slotRequestPreset(99); // request the current RAM preset
    QTimer::singleShot(500, this, SLOT(slotSendGlobalsRequest()));
}

void MainWindow::slotSyncEditorToQn()
{
    qDebug() << "slotSyncEditorToQn called";
    // need to make the sysex connection here before we sync
    if (connected)
    {
        QuNexus->disconnect(SIGNAL(signalRxSysExBA(QByteArray)));
    }
    connected = true;

    // Connect non-fw sysex
    connect(QuNexus, SIGNAL(signalRxSysExBA(QByteArray)), sysExEncDecode, SLOT(slotProcessSysEx(QByteArray)));

    presetManager->slotSyncEditBufferToDevice();
    QTimer::singleShot(500, this, SLOT(slotEncodeGlobals()));
}

void MainWindow::slotPresetProcessed(unsigned char presetNum)
{
    qDebug() << "slotPresetProcessed called, presetNum: " << presetNum;

    slotUpdateParametersUi(presetManager->editBufferPresetMap); // update ui
    presetManager->presetLoading = false; // clear flag
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (presetManager->presetIsDirty)
    {
        slotShowGreyRect();

        // store this global pointer to be called if save or discard are called
        editorCloseEvent = event;

        qDebug() << "slotCloseEvent called";

        QDialog *msgBox = new QDialog(this);
        msgBox->setMinimumSize(300, 100);
        msgBox->setFixedSize(300, 100);

        QPoint centerparent(
                    this->x() + ((this->frameGeometry().width() - msgBox->frameGeometry().width()) /2),
                    this->y() + ((this->frameGeometry().height() - msgBox->frameGeometry().height()) /2));

        msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        msgBox->move(centerparent);

        msgBox->setStyleSheet("QWidget{background: rgba(120,120,120, 255);}");

        QString currentPresetText = libraryPresetsList->item(presetManager->selectedLibraryPreset)->text();
        QString dialogText = QString("Save Changes to %1?").arg(currentPresetText);
        QLabel* text = new QLabel(dialogText, msgBox, Qt::WindowFlags());
        text->setAlignment(Qt::AlignCenter);
        text->setMinimumSize(300, 50);
        text->setFixedSize(300, 50);
        text->move(0, 10);

    #ifdef Q_OS_MAC // EB attempt to make this cross platform
        text->setStyleSheet("font: 12pt;");
    #else
        text->setStyleSheet("font: 10pt;");
    #endif

        QPushButton* discardButton = new QPushButton(msgBox);
        discardButton->setStyleSheet(blueStyleString);
        discardButton->setText("Don't Save");
        discardButton->setGeometry(QRect(10, 60, 70, 28));
        connect(discardButton, SIGNAL(clicked()), msgBox, SLOT(close()));
        connect(discardButton, SIGNAL(clicked()), this, SLOT(slotDiscardAndCloseEditor()));

        QPushButton* cancelButton = new QPushButton(msgBox);
        cancelButton->setStyleSheet(grayStyleString);
        cancelButton->setText("Cancel");
        cancelButton->setGeometry(QRect(140, 60, 70, 28));
        connect(cancelButton, SIGNAL(clicked()), msgBox, SLOT(close()));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelCloseEditor()));

        QPushButton* saveButton = new QPushButton(msgBox);
        saveButton->setStyleSheet(blueStyleString);
        saveButton->setText("Save");
        saveButton->setGeometry(QRect(220, 60, 70, 28));
        connect(saveButton, SIGNAL(clicked()), msgBox, SLOT(close()));
        connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveAndCloseEditor()));

        msgBox->exec();
    }
    else
    {
        qDebug() << "accept close event";
        QApplication::quit();
        //editorCloseEvent->accept();
        //QApplication::quit();
    }
}

void MainWindow::cancelCloseEditor()
{
    editorCloseEvent->ignore();
    slotHideGreyRect();
}

void MainWindow::slotDiscardAndCloseEditor()
{
    qDebug() << "slotDiscardAndCloseEditor called";
    presetManager->JSONLoading = true; // block update
    presetManager->slotRevertSelectedPreset();
    qDebug() << "accept close event";
    editorCloseEvent->accept();
}

void MainWindow::slotSaveAndCloseEditor()
{
    qDebug() << "slotSaveAndCloseEditor called";
    presetManager->JSONLoading = true; // block update
    presetManager->slotSaveSelectedPreset();
    qDebug() << "accept close event";
    editorCloseEvent->accept();
}


// --------------------------------------------------------------------------------------
// ------ midi overhaul -----------------------------------------------------------------
// --------------------------------------------------------------------------------------

void MainWindow::slotMIDIPortChange(QString portName, uchar inOrOut, uchar messageType, int portNum)
{
    //qDebug() << "slotMIDIPortChange - " << kmiPorts->mType[messageType] << kmiPorts->inOut[inOrOut] << " portName:" << portName << " messageType: " << " portNum: " << portNum << "\n";
    QPushButton* connectedLabel = this->findChild<QPushButton *>("QuNexus_Connected_Label");
    QFrame* connectedFrame = this->findChild<QFrame *>("QuNexus_Connected_Frame");

    switch (messageType)
    {
    case PORT_CONNECT:

        // update dropdown
        if (inOrOut == PORT_OUT && portName != QUNEXUS_OUT_P1) // don't create feedback loop
        {
            ui->midi_outputs->addItem(portName); // update dropdown
            slotFixDropDownWidth(ui->midi_outputs);

            if (portName == settings->value(MIDI_THRU_KEY).toString()) // if this port matches the last selected port
            {
                recallMidiThruPortName = portName; // store name
                QTimer::singleShot(500, this, SLOT(slotRecallMIDIThru())); // wait, then set/update the port
            }

        }

        // **** QuNexus connect *****************************************
        if ((portName == QUNEXUS_IN_P1 || portName == QUNEXUS_OLD_IN_P1) && inOrOut == PORT_IN)
        {
            QuNexus->slotSetExpectedFW(thisFw);

            troubleshootWindow->slotDetected();
            connectedLabel->setText("Detected");
            connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background:rgb(255,125,0); border-radius:6;}");
            slotClearFocus();

            if (!QuNexus->slotUpdatePortIn(portNum))
            {
                slotCreateDialog(QString("ERROR: MIDI input port \"%1\"\nis currently being used by another program or process!").arg(portName));
            }
            else
            {
                fwUpdateWindow->slotAppendTextToConsole("\nQuNexus Connected\n");
            }

        }
        else if ((portName == QUNEXUS_OUT_P1 || portName == QUNEXUS_OLD_OUT_P1) && inOrOut == PORT_OUT)
        {
            if (!QuNexus->slotUpdatePortOut(portNum))
            {
                slotCreateDialog(QString("ERROR: MIDI output port \"%1\"\nis currently being used by another program or process!").arg(portName));
            }
            else
            {
                QuNexus->pollingStatus = true;
            }
        }

        break;
    case PORT_DISCONNECT:

        if (inOrOut == PORT_OUT)
        {
            // update dropdown
            if (ui->midi_outputs->currentText() == portName)
            {
                ui->midi_outputs->setCurrentIndex(0);
            }

            ui->midi_outputs->removeItem(ui->midi_outputs->findText(portName));
        }

        // **** QuNexus disconnect **************************************
        if (inOrOut == PORT_IN && portName == QuNexus->portName_in)
        {
            // close ports and stop polling
            QuNexus->slotCloseMidiIn(SIGNAL_SEND);
            fwUpdateWindow->slotAppendTextToConsole("\nQuNexus Disconnected\n");
            connectedLabel->setText("Not Connected");
            connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background: rgb(100,100,100); border-radius:6;}");
        }
        else if (inOrOut == PORT_OUT && portName == QuNexus->portName_out)
        {
            QuNexus->slotCloseMidiOut(SIGNAL_SEND);
            QuNexus->pollingStatus = false;
        }
        else if (inOrOut == PORT_OUT && (portName == QUNEXUS_OUT_P1 || portName == QUNEXUS_OLD_OUT_P1) )
        {
            troubleshootWindow->slotConnected(false);
            connectedLabel->setText("Not Connected");
            connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background: rgb(100,100,100); border-radius:6;}");
        }

        break;
    case PORT_CHANGED:
        //qDebug() << " PORT CHANGED - name: " << portName << portName << " inOrOut: " << kmiPorts->inOut[inOrOut] << " messageType: " << kmiPorts->mType[messageType] << " portNum: " << portNum << "\n";

        // **** QuNexus renumber ****************************************
        if ((portName == QUNEXUS_IN_P1 || portName == QUNEXUS_OLD_IN_P1) && inOrOut == PORT_IN)
        {
            QuNexus->slotUpdatePortIn(portNum);
        }
        else if ((portName == QUNEXUS_OUT_P1 || portName == QUNEXUS_OLD_OUT_P1) && inOrOut == PORT_OUT)
        {
            QuNexus->slotUpdatePortOut(portNum);
        }

        break;
    default:
        break;
    }
}

// close and then reopen the QuNexus ports
// this is needed when the bootloader and app port names do not match
//void MainWindow::slotRefreshConnection()
//{
//    qDebug() << "slotRefreshConnection called";
////#ifdef Q_OS_MAC
//    QuNexus->slotResetConnections(QUNEXUS_IN_P1, QUNEXUS_BL_PORT);
////#endif
//}


void MainWindow::slotBootloaderMode(bool fwUpdateRequested)
{
    qDebug() << "slotBootloaderMode called - fwUpdateRequested: "<< fwUpdateRequested;

    QPushButton* connectedLabel = this->findChild<QPushButton *>("QuNexus_Connected_Label");
    QFrame* connectedFrame = this->findChild<QFrame *>("QuNexus_Connected_Frame");

    connectedLabel->setText("BOOTLOADER");
    connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background:rgb(0,0,255); border-radius:6;}");

    troubleshootWindow->slotSetDevVersion(deviceFirmwareVersionString(), deviceBootloaderVersionString());
    troubleshootWindow->slotBootloaderMode();

    if (!fwUpdateRequested)
    {
        QMessageBox msgBox;
        msgBox.setText("Your device is in bootloader mode. Click OK to attempt a firmware update.");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Ok)
        {
            slotForceFirmwareUpdate();
        }
    }
}

void MainWindow::slotFwUpdateSuccessCloseDialog(bool success)
{
    qDebug() << "slotFwUpdateSuccessCloseDialog called - success: " << success;

    if (success)
    {
        slotUpdateMIDIThru();
        slotQuNexusConnected(true);
        slotEnableDisableMidiFunctions(true);
        slotSyncQuNexusDialog();
    }
    else
    {
        QuNexus->slotFirmwareUpdateReset();
        slotQuNexusConnected(false);
        slotEnableDisableMidiFunctions(false);
    }
    slotHideGreyRect();
}

void MainWindow::slotForceFirmwareUpdate()
{
    if (!QuNexus->port_out_open) return;

    slotFirmwareDetected(QuNexus, false); // act as if we received a firmware mismatch
    forceFirmwareUpdate = true;
}

void MainWindow::slotFirmwareDetected(MidiDeviceManager *thisMDM, bool matches)
{
    qDebug() << "slotFirmwareDetected called";

    troubleshootWindow->slotSetDevVersion(deviceFirmwareVersionString(), deviceBootloaderVersionString());
    if (forceFirmwareUpdate)
    {
        troubleshootWindow->slotRequestFwUpdate();
        forceFirmwareUpdate = false;
    }

    if (matches)
    {
        qDebug() << "FirmwareMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        //---------------------------------- Sync QuNexus Dialog
        // MIDI Overhaul
        slotSyncQuNexusDialog();
    }
    else
    {
        qDebug() << "Firmware MisMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        QPushButton* connectedLabel = this->findChild<QPushButton *>("QuNexus_Connected_Label");
        QFrame* connectedFrame = this->findChild<QFrame *>("QuNexus_Connected_Frame");

        connectedLabel->setText("FIRMWARE OUT OF DATE");
        connectedFrame->setStyleSheet("QFrame#QuNexus_Connected_Frame{border: 1px solid rgb(67,67,67); background:rgb(255,0,0); border-radius:6;}");

        // setup sysex connections to receive globals data
        QuNexus->disconnect(SIGNAL(signalRxSysExBA(QByteArray))); // disconnect to be safe
        connect(QuNexus, SIGNAL(signalRxSysExBA(QByteArray)), sysExEncDecode, SLOT(slotProcessSysEx(QByteArray)));

        fwUpdateWindow->slotClearText();
        fwUpdateWindow->slotAppendTextToConsole(deviceBootloaderVersionString());
        fwUpdateWindow->slotAppendTextToConsole(deviceFirmwareVersionString());

        fwUpdateWindow->show();
    }
}

// connect QuNexus midi input to to midi thru out
void MainWindow::slotUpdateMIDIThru()
{
    qDebug() << "slotUpdateMIDIThru called - connected: " << QuNexus->port_in_open;
    QString dropDownPortName = ui->midi_outputs->currentText();
    QuNexus->disconnect(SIGNAL(signalRxMidi_raw(uchar, uchar, uchar, uchar)));

    if (!QuNexus->port_in_open) return; // don't continue if we aren't connected

    settings->setValue(MIDI_THRU_KEY, dropDownPortName); // store this setting for the next time we run the editor

    if (dropDownPortName != "None")
    {
        qDebug() << "update thru port: " << dropDownPortName;

        // set and open the ports
        int thisOutPort = kmiPorts->getOutPortNumber(dropDownPortName);

        if (!midiTHRU->slotUpdatePortOut(thisOutPort)) // also opens the port
        {
            slotCreateDialog(QString("ERROR: MIDI output port \"%1\"\nis currently being used by another program or process!").arg(ui->midi_outputs->currentText()));
            ui->midi_outputs->blockSignals(true);
            ui->midi_outputs->setCurrentText("None");
            ui->midi_outputs->blockSignals(false);
            midiTHRU->slotCloseMidiIn();
            midiTHRU->slotCloseMidiOut();
        }
        else
        {
            recallMidiThruPortName = dropDownPortName;
            connect(QuNexus, SIGNAL(signalRxMidi_raw(uchar, uchar, uchar, uchar)), midiTHRU, SLOT(slotSendMIDI(uchar, uchar, uchar, uchar)));
        }
    }
    else
    {
        qDebug() << "close port, don't update";
        midiTHRU->slotCloseMidiOut(SIGNAL_NONE);
        recallMidiThruPortName = dropDownPortName;
    }
}

void MainWindow::slotRecallMIDIThru()
{
    qDebug() << "slotRecallMIDIThru called, connected: " << connected << " recallMidiThruPortName: " << recallMidiThruPortName;
    if (!connected || recallMidiThruPortName == "") return; // wait until connected to QuNexus and the previously saved port

    ui->midi_outputs->setCurrentText(recallMidiThruPortName);
    slotUpdateMIDIThru();
}

void MainWindow::slotClearMIDIThruDropdown()
{
    ui->midi_outputs->clear();
    ui->midi_outputs->addItem("None");
}

// --------------------------------------------------------------------------------------
// ------ end midi overhaul -------------------------------------------------------------
// --------------------------------------------------------------------------------------

void MainWindow::slotDebugSlot()
{
    qDebug() << "slotDebugSlot called";
    ui->centralWidget->setFocus();
}

void MainWindow::slotClearFocus()
{
    qDebug() << "slotClearFocus called";
    ui->centralWidget->setFocus();
}

// revert the currently selected preset and set/clear the JSONLoading flag
void MainWindow::slotRevertPreset()
{
    presetManager->JSONLoading = true;
    presetManager->slotRevertSelectedPreset();
    presetManager->JSONLoading = false;
    presetManager->slotSyncEditBufferToDevice();
}

//void MainWindow::slotFixDropDownWidth(QComboBox* thisDropDown)
//{
//    int i, length = 0, index = 0;

//    QString thisText;

//    for (i = 0; i < thisDropDown->count(); i++)
//    {
//        if (length < thisDropDown->itemText(i).length())
//        {
//            thisText = thisDropDown->itemText(i);
//            length = thisDropDown->itemText(i).length();
//            index = i;
//            //qDebug() << "this text: " << thisText << " index: " << index << " i: " << i << " length: " << length;
//        }
//    }

//#ifdef Q_OS_MAC
//    QFont font("Droid Sans", 12);
//    QFontMetrics fm(font);
//    int pixelsWide = fm.boundingRect(thisDropDown->itemText(index)).width() * 1.5;
//#else
//    QFont font("Droid Sans", 10);
//    QFontMetrics fm(font);
//    int pixelsWide = fm.boundingRect(thisDropDown->itemText(index)).width();
//#endif
//    int pixelsHeight = 14 * thisDropDown->count();
//    if (pixelsHeight > 500) pixelsHeight = 500;

//    QString color = "rgb(139,30,0);";
//    QString thisComboStyleString = comboStyleString.arg(pixelsWide).arg(pixelsHeight);
//    thisDropDown->setStyleSheet(thisComboStyleString);
//}

void MainWindow::slotFixDropDownWidth(QComboBox* thisDropDown)
{
    //return;
    //int IconWidth = thisDropDown->iconSize().width();
    int IconWidth = 15;
    int scroll = thisDropDown->count() <= thisDropDown->maxVisibleItems() ? 0 :
        QApplication::style()->pixelMetric(QStyle::PixelMetric::PM_ScrollBarExtent);

    int max = 0;

    for (int i = 0; i < thisDropDown->count(); i++)
    {
        int width = thisDropDown->view()->fontMetrics().horizontalAdvance(thisDropDown->itemText(i));
        if (max < width) max = width;
    }
    int thisWidth = scroll + max + IconWidth;
    if (thisDropDown->objectName() == "leddisplaymode")
    {
        thisWidth = 75; // hard code fix
    }
    thisDropDown->view()->setMinimumWidth(thisWidth);

    //QString thisComboStyleString = comboStyleString.arg(thisWidth - 10).arg(100);
    //thisDropDown->setStyleSheet(thisComboStyleString);

    //qDebug() << "slotFixDropDownWidth called - box: " << thisDropDown->objectName() << " width: " << thisWidth << " scroll: " << scroll << " max: " << max << " IconWidth: " << IconWidth;
}
