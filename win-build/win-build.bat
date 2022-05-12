:: app specific variables - adjust these to match your system and the script should do the rest
set app_name=QuNexus Editor
set app_version=2.1.0B
set output_dir=%app_name% WIN %app_version%
set build_dir=..\build-QuNexus-Desktop_Qt_6_2_4_MSVC2019_64bit-Release\release
set qunexus_binary="%build_dir%\%app_name%.exe"
set qunexus_binary_console="%build_dir%\%app_name% (Debug Console).exe"
set qunexus_output_binary="%output_dir%\%app_name%.exe"
set qunexus_console_output_binary="%output_dir%\%app_name% (Debug Console).exe"

set path_to_qt=C:\Qt\6.2.4

set path_to_qtwindeploy="%path_to_qt%\msvc2019_64\bin\windeployqt.exe"
set path_to_qml_files="..\qt-qunexus\source"
set path_to_vcvarsall="c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
set path_to_signtool="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe"
set "current_dir=%cd%"
:: end variables



@echo off

cls
@echo Please pause dropbox sync before continuing (avoids zip errors)
pause

echo Setting up environment for Qt usage...
set PATH=%path_to_qt%\msvc2019_64\bin;%PATH%
cd /D %path_to_qt%\msvc2019_64
call %path_to_vcvarsall% x86_amd64

@echo on

cd %current_dir%

mkdir "%output_dir%"

xcopy /S /Y .\presets "%output_dir%\presets\"

%path_to_qtwindeploy% --qmldir %path_to_qml_files% --dir "%output_dir%" %qunexus_binary%

xcopy /S /Y %qunexus_binary% "%output_dir%"
xcopy /S /Y %qunexus_binary_console% "%output_dir%"
xcopy /S /Y .\ssl\*.* "%output_dir%"

%path_to_signtool% sign /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash %qunexus_output_binary%
%path_to_signtool% sign /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash %qunexus_console_output_binary%

echo "Wait 5 seconds for dropbox to catch up"

TIMEOUT 5

del /Q "%output_dir%.zip"
powershell compress-archive "'%output_dir%' '%output_dir%.zip'" 

rmdir /S/Q "%output_dir%"

pause





