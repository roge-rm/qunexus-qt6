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
VERSION = 2.2.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

#uncomment this to build a console version of the app. Do this once before deploying the app.
#BUILD_CONSOLE = 1

# Uncomment this line if you want to deploy the app (codesign, xxxDeployqt, copy content, and create installer/dmg etc
DEPLOY = 1

# Uncomment this line if your project includes QML, this will add the qmldir option to the qt deployment utility command
INCLUDE_QML = 1

# still holding onto support for High Sierra here, separate build
message("Building with Qt $${QT_VERSION}")

# build with Qt 5.11.3 to support El Capitan, Sierra, and High Sierra
lessThan(QT_MAJOR_VERSION, 6){
    message("Building legacy MacOS Intel Binary")
    macx{
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
    }
}

# build with Qt 6.2.1 to support Mojave and later
versionAtLeast(QT_VERSION, 6.2.1):!versionAtLeast(QT_VERSION, 6.9.0){
    macx{
        message("Building Apple M1/Intel Universal Binary for macOS 10.14+")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
        QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    }
}

# build with Qt 6.9+ for macOS 12.0+ (Qt 6.9.2 requires macOS 12.0)
versionAtLeast(QT_VERSION, 6.9.0){
    macx{
        message("Building Apple M1/Intel Universal Binary for macOS 12.0+")
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
        QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
        QMAKE_LFLAGS += -Wl,-w  # suppress harmless alignment warnings in Universal builds
        CONFIG+=sdk_no_version_check
    }
}

!isEmpty(BUILD_CONSOLE) {
    CONFIG += console
}

INCLUDEPATH +=  source \
                source/datamanagement \
                source/midiio



SOURCES +=  main.cpp\
    inc/KMI_MIDI_Device_Manager/KMI_SysexMessages.c \
    inc/KMI_MIDI_Device_Manager/fwupdate/fwupdate.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_fwupdate.cpp \
    inc/KMI_MIDI_Device_Manager/diagnosticlogger.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.cpp \
    inc/KMI_MIDI_Device_Manager/troubleshoot/troubleshoot.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_ports.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_updates.cpp \
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
    inc/KMI_MIDI_Device_Manager/KMI_fwupdate.h \
    inc/KMI_MIDI_Device_Manager/diagnosticlogger.h \
    inc/KMI_MIDI_Device_Manager/KMI_DevData.h \
    inc/KMI_MIDI_Device_Manager/KMI_FwVersions.h \
    inc/KMI_MIDI_Device_Manager/KMI_SysexMessages.h \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.h \
    inc/KMI_MIDI_Device_Manager/midi.h \
    inc/KMI_MIDI_Device_Manager/troubleshoot/troubleshoot.h \
    inc/KMI_MIDI_Device_Manager/KMI_ports.h \
    inc/KMI_MIDI_Device_Manager/KMI_updates.h \
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
    inc/KMI_MIDI_Device_Manager/KMI_mdm.qrc \
    resources.qrc \
    resources/fonts/fonts.qrc

## QML stuff

QML_IMPORT_PATH =
CONFIG += qtquickcompiler

#-------------------MIDI--------------------#
#-------------------------------------------#

INCLUDEPATH += \
    inc/rtmidi \
    inc/KMI_MIDI_Device_Manager \
    inc/KMI_MIDI_Device_Manager/fwupdate \
    inc/KMI_MIDI_Device_Manager/troubleshoot \

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
    WMS_SDK_ROOT = C:/PROGRA~2/WI3CF2~1/10

    WMS_SDK_VERSION = $$replace($$(WindowsSDKVersion), \\\\, /)
    WMS_SDK_VERSION = $$replace(WMS_SDK_VERSION, /$, )
    isEmpty(WMS_SDK_VERSION): WMS_SDK_VERSION = 10.0.26100.0

    WMS_CPPWINRT = $${WMS_SDK_ROOT}/Include/$${WMS_SDK_VERSION}/cppwinrt
    WMS_WINDOWS_WINMD = $${WMS_SDK_ROOT}/UnionMetadata/$${WMS_SDK_VERSION}/Windows.winmd
    WMS_CPPWINRT_EXE = $${WMS_SDK_ROOT}/bin/$${WMS_SDK_VERSION}/x64/cppwinrt.exe
    WMS_RUNTIME_WINMD = C:/Program Files/Windows MIDI Services/Desktop App SDK Runtime/Microsoft.Windows.Devices.Midi2.winmd
    WMS_PROJECTION_DIR = $$OUT_PWD/generated/winrt
    WMS_PROJECTION_HEADER = $${WMS_PROJECTION_DIR}/winrt/Microsoft.Windows.Devices.Midi2.h

    !exists($$WMS_CPPWINRT): error(Windows SDK cppwinrt headers not found at $$WMS_CPPWINRT)
    !exists($$WMS_WINDOWS_WINMD): error(Windows.winmd not found at $$WMS_WINDOWS_WINMD)
    !exists($$WMS_CPPWINRT_EXE): error(cppwinrt.exe not found at $$WMS_CPPWINRT_EXE)
    !exists($$WMS_RUNTIME_WINMD): error(Windows MIDI Services runtime winmd not found at $$WMS_RUNTIME_WINMD)

    INCLUDEPATH += $$WMS_CPPWINRT $$WMS_PROJECTION_DIR
    # Both WinMM and WMS compiled in — backend is selected at runtime based on SDK availability.
    DEFINES += __WINDOWS_MM__=1
    DEFINES += __WINDOWS_MIDI_SERVICES__=1
    LIBS += -lwinmm
    LIBS += -lole32
    LIBS += -lruntimeobject
    LIBS += -lwindowsapp

    wms_projection.target = $$WMS_PROJECTION_HEADER
    export(wms_projection.target)

    wms_projection.commands = if not exist $$shell_path(\"$$WMS_PROJECTION_DIR\") mkdir $$shell_path(\"$$WMS_PROJECTION_DIR\") $$escape_expand(\n\t) \
                              $$shell_path(\"$$WMS_CPPWINRT_EXE\") -input $$shell_path(\"$$WMS_RUNTIME_WINMD\") -reference $$shell_path(\"$$WMS_WINDOWS_WINMD\") -output $$shell_path(\"$$WMS_PROJECTION_DIR\")
    export(wms_projection.commands)

    PRE_TARGETDEPS += $$WMS_PROJECTION_HEADER
    export(PRE_TARGETDEPS)

    first.depends += $(first) wms_projection
    export(first.depends)

    QMAKE_EXTRA_TARGETS += first wms_projection
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

# copy SSL DLLs for checking kmi.com for updates
win32 {
    LIBCRYPTO_SRC = $$PWD/inc/KMI_MIDI_Device_Manager/ssl/libcrypto-1_1-x64.dll
    LIBSSL_SRC = $$PWD/inc/KMI_MIDI_Device_Manager/ssl/libssl-1_1-x64.dll

    LIBCRYPTO_DST = $$replace(LIBCRYPTO_SRC, '/', '\\')
    LIBSSL_DST = $$replace(LIBSSL_SRC, '/', '\\')

    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += copy /y \"$$LIBCRYPTO_DST\" \"$$OUT_PWD\\debug\\\" && \
                           copy /y \"$$LIBSSL_DST\" \"$$OUT_PWD\\debug\\\" && \
                           echo "Copied OpenSSL DLLs to debug directory."
    } else {
        QMAKE_POST_LINK += copy /y \"$$LIBCRYPTO_DST\" \"$$OUT_PWD\\release\\\" && \
                           copy /y \"$$LIBSSL_DST\" \"$$OUT_PWD\\release\\\" && \
                           echo "Copied OpenSSL DLLs to release directory."
    }
}

# Release packaging (signing, windeployqt, installer creation) lives in
# .vscode/make-installer.ps1, not here — see SoftStep's equivalent .pro
# (no deploy target at all) for the pattern this follows.



