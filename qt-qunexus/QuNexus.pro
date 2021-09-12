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

macx{
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
}

INCLUDEPATH +=  source \
                source/datamanagement \
                source/midiio



SOURCES +=  main.cpp\
    fwupdate.cpp \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.cpp \
    inc/KMI_Ports/kmi_ports.cpp \
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
    fwupdate.h \
    inc/KMI_MIDI_Device_Manager/KMI_DevData.h \
    inc/KMI_MIDI_Device_Manager/KMI_FwVersions.h \
    inc/KMI_MIDI_Device_Manager/KMI_SysexMessages.h \
    inc/KMI_MIDI_Device_Manager/KMI_mdm.h \
    inc/KMI_MIDI_Device_Manager/midi.h \
    inc/KMI_Ports/kmi_ports.h \
    inc/rtmidi/RtMidi.h \
            source/datamanagement/presetmanager.h \
            source/midiio/Qn_preset_struct.h \
            source/datamanagement/presetnamedialog.h \
            source/midiio/midi.h \
            source/midiio/sysexencdecode.h \
            source/sendpresetsdialog.h \
            source/syncqunexusdialog.h \
            source/tableinterface.h \
            source/cvtrimsinterface.h \
            source/midiio/Qn_preset_struct.h

FORMS    += mainwindow.ui \
            fwupdate.ui \
            perkeysensitivities.ui \
            tableviewerform.ui \
            cvtrimsviewerform.ui


RESOURCES += \
    resources.qrc

#-------------------QJson-------------------#
#-------------------------------------------#
static{
DEFINES += STATIC_BUILD
}

INCLUDEPATH += qjson/src

SOURCES += \
  #qjson/src/json_parser.cc \
  #qjson/src/json_scanner.cpp \
  #qjson/src/parser.cpp \
  #qjson/src/qobjecthelper.cpp \
  #qjson/src/serializer.cpp


#-------------------MIDI--------------------#
#-------------------------------------------#
#macx {
#INCLUDEPATH += ./source/midiio/mac

## source/midiio/mac/mididevice.h \
#HEADERS +=  \
#    source/midiio/mac/qunexusmidi.h
#SOURCES +=  \
#    source/midiio/mac/qunexusmidi.cpp
#LIBS += -framework CoreMIDI
#LIBS += -framework CoreFoundation
#LIBS += -framework Cocoa
#LIBS += -framework CoreServices
#}

#win32{
#INCLUDEPATH += ./source/midiio/windows
#HEADERS += ./source/midiio/windows/mididevicemanager.h
#SOURCES  += ./source/midiio/windows/mididevicemanager10.cpp
#LIBS += -lwinmm
#}

INCLUDEPATH += \
    inc/rtmidi \
    inc/KMI_Ports \
    inc/KMI_MIDI_Device_Manager

#DEFINES += \
    #MDM_DEBUG_ENABLED

# These defines set RtMidi to the correct OS API
macx{
    DEFINES += __MACOSX_CORE__=1
    DEFINES += __MACOSX_SYX_XMIT_SIZE__=1023 #32767
    LIBS += -framework CoreMidi
    LIBS += -framework CoreAudio
    LIBS += -framework CoreFoundation
}

linux{
    DEFINES += define __LINUX_ALSA__=1
    LIBS += -lasound
}

win32{
    DEFINES += __WINDOWS_MM__=1
        LIBS += -lwinmm
}
# end rtmidi defines



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
    resources/stylesheets/GeneralStyles.qss \
    resources/stylesheets/GeneralStylesWindows.qss


#--------------- contents/resources --------

macx{
#    softStepSysEx.files = $$PWD/../../shared/SoftStep.syx
#    softStepSysEx.path = Contents/Resources
#    QMAKE_BUNDLE_DATA += softStepSysEx

    qunexusPresets.files = $$PWD/presets
    qunexusPresets.path = Contents/Resources
    QMAKE_BUNDLE_DATA += qunexusPresets
}

win32{
#    pedalTable.commands = $(COPY_DIR) $$shell_path(\"$$PWD/resources/pedalTable.txt\") $$shell_path(\"$$OUT_PWD/Resources/\")
#    export(pedalTable.commands)

#    sysex.commands = $(COPY_DIR) $$shell_path(\"$$PWD/../../shared/SoftStep.syx\") $$shell_path(\"$$OUT_PWD/Resources\")
#    export(sysex.commands)

    presets.commands = $(COPY_DIR) $$shell_path(\"$$PWD/presets\") $$shell_path(\"$$OUT_PWD/Resources/presets\")
    export(presets.commands)

#    first.depends += $(first) pedalTable
    first.depends += $(first) presets
#    first.depends += $(first) sysex
    export(first.depends)

#    QMAKE_EXTRA_TARGETS += first pedalTable
    QMAKE_EXTRA_TARGETS += first presets
#    QMAKE_EXTRA_TARGETS += first sysex
}
