:: app specific variables - adjust these to match your system and the script should do the rest
set app_name=QuNexus Editor
set app_version=2.0.0
set output_dir=%app_name% %app_version%
set qunexus_binary="C:\Users\eric\Dropbox (KMI)\00_Editors\00_Windows\Build\build-QuNexus-Desktop_Qt_6_0_1_MSVC2019_64bit-Release\release\QuNexus Editor.exe"
set package_dir="C:\Users\eric\Dropbox (KMI)\00_Editors\00_Windows\QuNexus\win-build"
set path_to_qtwindeploy="C:\Qt6\6.0.1\msvc2019_64\bin\windeployqt.exe"
set path_to_qml_files="C:\Users\eric\Dropbox (KMI)\00_Editors\00_Windows\QuNexus\qt-qunexus\source"
set path_to_vcvarsall="c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
:: end variables



@echo off

cls
@echo Please pause dropbox sync before continuing (avoids zip errors)
pause

echo Setting up environment for Qt usage...
set PATH=C:\Qt6\6.0.1\msvc2019_64\bin;%PATH%
cd /D C:\Qt6\6.0.1\msvc2019_64
call %path_to_vcvarsall% x86_amd64

@echo on

cd %package_dir%

mkdir "%output_dir%"

xcopy /S /Y .\presets "%output_dir%\presets\"

%path_to_qtwindeploy% --qmldir %path_to_qml_files% --dir "%output_dir%" %qunexus_binary%

xcopy /S /Y %qunexus_binary% "%output_dir%"

echo "Wait 5 seconds for dropbox to catch up"

TIMEOUT 5

powershell compress-archive "'%output_dir%' '%output_dir%.zip'" 

rmdir /S/Q "%output_dir%"

pause





