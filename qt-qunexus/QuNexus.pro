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
VERSION = 2.2.1
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


# ********************************************************************************************************
# NOTE for the deployment process to work you MUST add an additional "make" build step, arguments: deploy
# ********************************************************************************************************

isEmpty(DEPLOY) {
    # Define a dummy deploy target that does nothing
    QMAKE_EXTRA_TARGETS += deploy
    deploy.commands = @echo "Deploy is disabled"
} else {


    win32 {
        package_dir = $$shell_path($$absolute_path("..\\win-deploy\\packages\\com.musekinetics.qunexuseditor\\data\\$${TARGET}", $$PWD))
        content_dir = $$shell_path($$absolute_path("..\\win-deploy\\packages\\com.musekinetics.qunexuseditor\\data\\Content", $$PWD))
        repo_root_dir = $$shell_path($$absolute_path("..", $$PWD))

        changelog_src = "$${repo_root_dir}\\CHANGELOG.md"
        content_src = "$${repo_root_dir}\\Content"

        LIBCRYPTO_SRC = $$PWD/inc/KMI_Updates/ssl/libcrypto-1_1-x64.dll
        LIBSSL_SRC = $$PWD/inc/KMI_Updates/ssl/libssl-1_1-x64.dll

        LIBCRYPTO_DST = $$replace(LIBCRYPTO_SRC, '/', '\\')
        LIBSSL_DST = $$replace(LIBSSL_SRC, '/', '\\')

        path_to_signtool = C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.22000.0\\x64\\signtool.exe
        path_to_qtwindeploy = $$[QT_INSTALL_BINS]\\windeployqt.exe

        path_to_bincreate = C:\\Qt6\\QtIFW-4.6.0\\bin\\binarycreator.exe
        path_to_installerbase = C:\\Qt6\\QtIFW-4.6.0\\bin\\installerbase.exe
        path_to_install = $$shell_path($$absolute_path("..\\win-deploy", $$PWD))

        # Corrected variable assignments without escaped quotes
        app_name = $${TARGET}
        installer_name = \"$$app_name v$$VERSION Windows Installer\"
        installer_file = \"$$app_name v$$VERSION Windows Installer.exe\"


        message("path_to_bincreate is: " $$quote($$path_to_bincreate))
        message("installer_file is: " $$quote($$installer_file))

        build_subdir = release  # Default to release
        CONFIG(debug, debug|release): build_subdir = debug

        temp_out_pwd = $$replace(OUT_PWD, "/", "\\")
        binary_src = "$${temp_out_pwd}\\$${build_subdir}\\$${TARGET}.exe"
        binary_dest = "$${package_dir}\\$${TARGET}.exe"
        app_name = $${TARGET}

        debug_src = "$${temp_out_pwd}\\$${build_subdir}\\$${TARGET} (debug console).exe"
        debug_dest = "$${package_dir}\\$${TARGET} (debug console).exe"



        deploy_opts = "--compiler-runtime"
        !isEmpty(INCLUDE_QML) {
            deploy_opts += " --qmldir \"$$PWD\""
        }


        # Define custom deployment commands after all variables have been defined
        DEPLOY_COMMANDS = \
            echo Deploying for Windows && \
            echo Copying executable to package_dir && \
            copy /y \"$$binary_src\" \"$$binary_dest\" && \
            copy /y \"$$debug_src\" \"$$debug_dest\" && \
            \
            echo Signing App Executable && \
            \"$$path_to_signtool\" sign /v /debug /a /tr http://timestamp.digicert.com /td SHA256 /fd certHash \"$$binary_dest\" && \
            \"$$path_to_signtool\" sign /v /debug /a /tr http://timestamp.digicert.com /td SHA256 /fd certHash \"$$debug_dest\" && \
            \
            echo Running qtwindeploy: \"$$package_dir\" && \
            \"$$path_to_qtwindeploy\" $$deploy_opts --dir \"$$package_dir\" \"$$binary_dest\" && \
            \
            echo Copying SSL dlls to package_dir && \
            copy /y \"$$LIBCRYPTO_DST\" \"$$package_dir\" && \
            copy /y \"$$LIBSSL_DST\" \"$$package_dir\" && \
            \
            echo Clearing content && \
            (if exist \"$$content_dir\" rmdir /s /q \"$$content_dir\" || echo Directory not found, skipping deletion) && \
            mkdir \"$$content_dir\" && \
            \
            echo Copying content && \
            Robocopy \"$$content_src\" \"$$content_dir\" /MIR && \
            echo Robocopy Exit Code: %ERRORLEVEL% & \
            \
            echo Copying changelog && \
            copy /y \"$$changelog_src\" \"$$content_dir\" && \
            \
            echo Creating Installer && \
            cd \"$$path_to_install\" && \
            \"$$path_to_bincreate\" --verbose --offline-only -c config/config.xml -p packages $$installer_name && \
            \
            echo Signing Installer && \
            \"$$path_to_signtool\" sign /v /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash $$installer_file

        # Define a phony target for deployment
            QMAKE_EXTRA_TARGETS += deploy
            deploy.commands = $$DEPLOY_COMMANDS
            deploy.depends = first  # ensures this runs after the first build

    }
}



