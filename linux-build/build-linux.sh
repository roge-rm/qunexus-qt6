#!/bin/bash

# This script packages the application 

# app_name is the name of the dmg volume
this_year=2021
version=2.0.6
app_name="QuNexus Editor"
app_name_fp=QuNexus\ Editor
app_source=../build-QuNexus-Desktop_Qt_5_15_2_GCC_64bit-Release/QuNexus\ Editor
package_path=./$app_name_fp\ $version
app_path=$package_path/$app_name
path_to_dqt=./linuxdeployqt/linuxdeployqt-5-x86_64.AppImage

zip_name="QuNexus Editor Linux v$version.zip"

# set path
export PATH=/home/osboxes/Qt/5.14.2/gcc_64/bin/:$PATH

# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "$app_name Builder v$version"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

echo "Delete $package_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# delete dir
rm -rf "$package_path"

echo "Make $package_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# make target directory
mkdir "$package_path"

echo "Copy $app_source to $package_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""


# copy the .app and resources
\cp -R "$app_source" "$package_path"
\cp -R ./default.desktop "$package_path"
\cp -R ./QuNexus.png "$package_path"


echo Run: $path_to_dqt "$app_path" -verbose=2 -appimage -qmldir=../qt-qunexus/source/ -qmake=/home/osboxes/Qt/5.15.2/gcc_64/bin/qmake
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""


#run linuxdeployqt
$path_to_dqt "$app_path" -verbose=2 -appimage -qmldir=../qt-qunexus/source/ -qmake=/home/osboxes/Qt/5.15.2/gcc_64/bin/qmake

echo "Rename files"
echo mv "$package_path/Application-x86_64.AppImage" "$package_path/QuNexus.AppImage"
echo mv "$package_path/AppRun" "$package_path/QuNexusRun"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

#rename files
mv "$package_path/AppRun" "$package_path/QuNexusRun"

rm -rf "$zip_name"

echo Run: zip -r $zip_name $package_path
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# create zip

zip -r "$zip_name" "$package_path"



