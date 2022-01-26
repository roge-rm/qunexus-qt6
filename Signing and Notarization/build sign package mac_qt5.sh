#!/bin/bash

# This script packages the K-Mix mac application and handles the Apple signing/notarization process.
# - You must use the entitlements included in this directory.

# app_name is the name of the dmg volume
this_year=2022
app_name="QuNexus Editor"
app_name_fp=QuNexus\ Editor
app_source=../../Build\ Directories/build-QuNexus-Desktop_Qt_5_14_2_clang_64bit-Release/QuNexus\ Editor.app

version=2.0.9
bundle_id="com.keithmcmillen.$app_name"
app_path=./dmg/QuNexus\ Editor.app
package_path=./dmg/
path_to_dqt=~/Qt/5.14.2/clang_64/bin/macdeployqt

qml_dir=../qt-qunexus/source/
developer_id="Developer ID Application: Kesumo, LLC (${APPLE_TEAM_ID})"

temp_dmg_name=./QuNexus\ Editor\ Mac\ v$version.p.dmg
final_dmg_name=./QuNexus\ Editor\ Mac\ v$version.dmg


# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "$app_name Builder v$version"
echo "$temp_dmg_name"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

# copy the .app and resources
\cp -R "$app_source" "$package_path"

# update info.plist
plutil -insert "App Category" -string "Music" "$app_path/Contents/Info.plist"
plutil -replace "Get Info string" -string "$version, Copyright 2021 Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "CFBundleLongVersionString" -string "$app_name $version, Copyright $this_year Keith McMillen Instruments" "$app_path/Contents/Info.plist"
plutil -replace "Bundle versions string, short" -string "$version" "$app_path/Contents/Info.plist"
plutil -replace "Bundle version" -string "$version" "$app_path/Contents/Info.plist"


# clean dmg attributes
xattr -cr "$package_path"

#run macdeployqt
$path_to_dqt "$app_path" -verbose=2 -codesign="$developer_id" -qmldir="$qml_dir" -executable="$app_path/Contents/MacOS/$app_name"
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path/Contents/Frameworks/QtPrintSupport.framework/Versions/5/Resources/QtPrintSupport.prl"
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path/Contents/Frameworks/QtQuickWidgets.framework/Versions/5/Resources/QtQuickWidgets.prl"
# sign the code
# QT frameworks plists copied. See: http://barenka.blogspot.com/2014/04/signing-qt5-520-applications-in-mac-osx.html
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$app_path"

# create dmg
hdiutil create "$temp_dmg_name" -volname "$app_name" -format UDRW -fs HFS -srcfolder "$package_path"

# mount image to adjust background etc

echo "..."
echo "Pressing enter will open the temporary DMG."
echo "1) Organize the icons, 2) set the background as needed, and 3) then close the window."
echo
read -n 1 -s -r -p ""
open "$temp_dmg_name"
sleep 4 # wait for image to mount
open /Volumes/"$app_name_fp"

echo "..."
echo "Once the volume is ejected, press enter again and it will be re-opened."
echo "Verify that everything looks correct."
echo
read -n 1 -s -r -p ""

# eject
hdiutil detach /Volumes/$app_name_fp/
open "$temp_dmg_name"
sleep 4 # wait for image to mount
open /Volumes/"$app_name_fp"


echo "If everything looks correct, eject the volume and press enter to finalize."
echo
read -n 1 -s -r -p ""

# convert to read only
hdiutil convert -format UDZO -o "$final_dmg_name" "$temp_dmg_name"
rm -rf "$temp_dmg_name"

# clean dmg attributes
xattr -cr "$final_dmg_name"

echo "signing...."
echo

# sign the dmg
codesign -s "$developer_id" --options runtime --timestamp --force --deep --entitlements ./entitlements.mac.plist -f "$final_dmg_name"

echo "notorizing..."
echo
# submit to notarize
xcrun altool --notarize-app -f "$final_dmg_name" -t osx -u ${APPLE_ID_EMAIL} -p ${APPLE_APP_PASSWORD} -primary-bundle-id $bundle_id



