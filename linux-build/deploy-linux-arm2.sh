#!/bin/bash

# This script packages the application 

# app_name is the name of the dmg volume
this_year=2024
version=2.2.0
app_name="QuNexus Editor"
app_name_fp=QuNexus\ Editor
app_source=../build-linux/Desktop_Qt_6_7_0-Release/QuNexus\ Editor
package_path=./$app_name_fp
content_source=../Content
content_dest=$package_path/Content
appdir_path=$package_path/$app_name_fp.AppDir
app_dest_path=$appdir_path/bin/$app_name_fp

app_path=$package_path/$app_name
path_to_appimgtool=./linuxdeployqt/appimagetool-855-aarch64.AppImage

path_to_gcc_arm=~/Qt/6.7.0/gcc_arm64/bin/

zip_name="QuNexus Editor Linux aarch64 v$version.zip"

# set path
export PATH=$path_to_gcc_arm:$PATH

# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "$app_name Builder v$version"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

echo "Delete $app_dest_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# delete dir
rm -rf "$app_dest_path"
rm -rf "$content_path"


echo "Copy $app_source to $package_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""


# copy the .app and content
\cp -R "$app_source" "$app_dest_path"
\cp -R "$content_source" "$content_dest"


echo Run: VERSION=$version $path_to_appimgtool -s deploy "$appdir_path"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""


#run appimgtool
VERSION=$version $path_to_appimgtool -s deploy "$appdir_path"

rm -rf "$zip_name"

echo Run: zip -r $zip_name $package_path
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# create zip

zip -r "$zip_name" "$package_path"



