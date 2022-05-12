#-------------------------------------------------
#
# Project created by QtCreator 2012-07-17T14:53:02
#
#-------------------------------------------------

QT       += core gui \
            #declarative \
            widgets \
            qml \
            quick \
            quickwidgets

TARGET = "QuNexus Editor"
TEMPLATE = app

# Uncomment to create a Windows console version of the app
#win32{
#    CONFIG += console
#}

# still holding onto support for High Sierra here, separate build

message("Building with Qt $${QT_VERSION}")

# build with Qt 5.11.3 to support El Capitan, Sierra, and High Sierra
lessThan(QT_MAJOR_VERSION, 6){
    macx{
        message("Building legacy MacOS Intel Binary")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
    }
}

# build with Qt 6.2.1 to support Mojave and later
versionAtLeast(QT_VERSION, 6.2.1){
    macx{
        message("Building Apple M1/Intel Universal Binary")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
        QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    }
}

INCLUDEPATH +=  source \
                source/datamanagement \
                source/midiio



SOURCES +=  main.cpp\
    inc/KMI_MIDI_Device_Manager/fwupdate/fwupdate.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.cpp \
    inc/KMI_MIDI_Device_Manager/troubleshoot/troubleshoot.cpp \
    inc/KMI_Ports/kmi_ports.cpp \
    inc/KMI_Updates/kmi_updates.cpp \
    inc/rtmidi/RtMidi.cpp \
            mainwindow.cpp \
            source/datamanagement/presetmanager.cpp \
            source/datamanagement/presetnamedialog.cpp \
            source/midiio/sysexencdecode.cpp \
            source/sendpresetsdialog.cpp \
            source/syncqunexusdialog.cpp \
            source/tableinterface.cpp \
            source/cvtrimsinterface.cpp

HEADERS  += mainwindow.h \
    inc/KMI_MIDI_Device_Manager/fwupdate/fwupdate.h \
    inc/KMI_MIDI_Device_Manager/KMI_DevData.h \
    inc/KMI_MIDI_Device_Manager/KMI_FwVersions.h \
    inc/KMI_MIDI_Device_Manager/KMI_SysexMessages.h \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.h \
    inc/KMI_MIDI_Device_Manager/midi.h \
    inc/KMI_MIDI_Device_Manager/troubleshoot/troubleshoot.h \
    inc/KMI_Ports/kmi_ports.h \
    inc/KMI_Updates/kmi_updates.h \
    inc/rtmidi/RtMidi.h \
            source/datamanagement/presetmanager.h \
            source/datamanagement/presetnamedialog.h \
            source/sendpresetsdialog.h \
            source/syncqunexusdialog.h \
            source/tableinterface.h \
            source/cvtrimsinterface.h \
            source/midiio/Qn_preset_struct.h \
            source/midiio/sysexencdecode.h \
            source/midiio/Qn_preset_struct.h

FORMS    += mainwindow.ui \
            inc/KMI_MIDI_Device_Manager/fwupdate/fwupdate.ui \
            inc/KMI_MIDI_Device_Manager/troubleshoot/troubleshoot.ui \
            perkeysensitivities.ui \
            tableviewerform.ui \
            cvtrimsviewerform.ui


RESOURCES += \
    inc/KMI_MIDI_Device_Manager/fwupdate/fw_stylesheets.qrc \
    resources.qrc \
    resources/fonts/fonts.qrc

## QML stuff

QML_IMPORT_PATH =
CONFIG += qtquickcompiler

#-------------------MIDI--------------------#
#-------------------------------------------#

INCLUDEPATH += \
    inc/rtmidi \
    inc/KMI_Ports \
    inc/KMI_MIDI_Device_Manager \
    inc/KMI_MIDI_Device_Manager/fwupdate \
    inc/KMI_MIDI_Device_Manager/troubleshoot \
    inc/KMI_Updates

#DEFINES += \
#    MDM_DEBUG_ENABLED # enable deeper deebugging for KMI Midi Device Manager

# These defines set RtMidi to the correct OS API
macx{
    DEFINES += __MACOSX_CORE__=1
    LIBS += -framework CoreMidi
    LIBS += -framework CoreAudio
    LIBS += -framework CoreFoundation
}

linux{
    DEFINES += __LINUX_ALSA__
}

#ios{
#    DEFINES += TARGET_OS_IPHONE=1
#    LIBS += -framework CoreMidi
#    LIBS += -framework CoreAudio
#    LIBS += -framework CoreFoundation

#}

#VERSION = 1.2.3

#android {
#    androidmanifestupdate.commands =  sed -i \'\' -E -e \'s/(versionName=)(\"([0-9]\.?)+\")/\\1\"$$VERSION\"/g\' $$ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml
#    QMAKE_EXTRA_TARGETS += androidmanifestupdate
#    PRE_TARGETDEPS += androidmanifestupdate
#}

#ios {
#    plistupdate.commands = /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $$VERSION\" $$QMAKE_INFO_PLIST
#    QMAKE_EXTRA_TARGETS += plistupdate
#    PRE_TARGETDEPS += plistupdate
#}


linux{
    DEFINES += define __LINUX_ALSA__=1
    LIBS += -lasound
}

win32{
    DEFINES += __WINDOWS_MM__=1
        LIBS += -lwinmm
}
# end rtmidi defines

# SSL support for checking for updates on windows
win32{
#    LIBS += -LC:\Qt6\Tools\OpenSSL\Win_x64\lib -llibcrypto
#    INCLUDEPATH+= C:\Qt6\Tools\OpenSSL\Win_x64\include\openssl
    LIBS += -LC:\Qt\Tools\OpenSSL\Win_x64\lib -llibcrypto
    INCLUDEPATH+= C:\Qt\Tools\OpenSSL\Win_x64\include\openssl
}


OTHER_FILES += \
    resources/icons/quNexusIcon.rc.txt \
    resources/icons/quNexusIcon.ico \
    resources/icons/quNexusIcon.icns \
    resources/state/appState.txt \
    resources/stylesheets/BlueButtonStyleSheet.qss \
    resources/stylesheets/GrayButtonStyleSheet.qss \
    resources/stylesheets/LibrarySelectedModifiedStyleSheet.qss \
    resources/stylesheets/LibrarySelectedCleanStyleSheet.qss \
    resources/stylesheets/LibrarySelectedCleanStyleSheetWindows.qss \
    resources/stylesheets/LibrarySelectedModifiedStyleSheetWindows.qss \
    resources/stylesheets/TabWidgetStyleSheetWindows.qss \
    resources/stylesheets/GeneralStyles.qss \
    resources/stylesheets/GeneralStylesWindows.qss \
    source/TableEditor.qml

#-------------------Icons--------------------#
#--------------------------------------------#

win32{
RC_FILE = resources/icons/quNexusIcon.rc.txt
}

macx{
ICON = resources/icons/quNexusIcon.icns
}


DISTFILES += \
    resources/icons/QuNexusIcons/quNexusIcon-1.png \
    resources/icons/QuNexusIcons/quNexusIcon-2.png \
    resources/icons/QuNexusIcons/quNexusIcon-3.png \
    resources/icons/QuNexusIcons/quNexusIcon-4.png \
    resources/icons/QuNexusIcons/quNexusIcon-5.png \
    resources/icons/QuNexusIcons/quNexusIcon-6.png \
    resources/icons/QuNexusIcons/quNexusIcon-7.png \
    resources/icons/QuNexusIcons/quNexusIcon-8.png \
    resources/icons/QuNexusIcons/quNexusIcon-9.png \
    resources/stylesheets/GeneralStyles.qss \
    resources/stylesheets/GeneralStylesWindows.qss


#--------------- contents/resources --------

macx{

    qunexusPresets.files = $$PWD/presets
    qunexusPresets.path = Contents/Resources
    QMAKE_BUNDLE_DATA += qunexusPresets
}

win32{

    presets.commands = $(COPY_DIR) $$shell_path(\"$$PWD/presets\") $$shell_path(\"$$OUT_PWD/release/presets\")
    export(presets.commands)

    first.depends += $(first) presets
    export(first.depends)

    QMAKE_EXTRA_TARGETS += first presets
}

linux{
    presets.commands = $(COPY_DIR) $$shell_path(\"$$PWD/presets\") $$shell_path(\"$$OUT_PWD/presets\")
    export(presets.commands)

    first.depends += $(first) presets
    export(first.depends)

    QMAKE_EXTRA_TARGETS += first presets
}
