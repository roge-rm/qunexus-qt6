# set current directory to where the script was called from
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

pwd

path_to_ex="./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor"

install_name_tool -change @loader_path/../Frameworks/QtQuickWidgets.framework/Versions/5/QtQuickWidgets @rpath/../Frameworks/QtQuickWidgets.framework/Versions/5/QtQuickWidgets ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor

install_name_tool -change @loader_path/../Frameworks/QtQuickWidgets.framework/Versions/5/QtQuickWidgets @rpath/../Frameworks/QtQuickWidgets.framework/Versions/5/QtQuickWidgets ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets @rpath/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtQuick.framework/Versions/5/QtQuick @rpath/../Frameworks/QtQuick.framework/Versions/5/QtQuick ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtGui.framework/Versions/5/QtGui @rpath/../Frameworks/QtGui.framework/Versions/5/QtGui ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtQmlModels.framework/Versions/5/QtQmlModels @rpath/../Frameworks/QtQmlModels.framework/Versions/5/QtQmlModels ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtQml.framework/Versions/5/QtQml @rpath/../Frameworks/QtQml.framework/Versions/5/QtQml ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork @rpath/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
install_name_tool -change @loader_path/../Frameworks/QtCore.framework/Versions/5/QtCore @rpath/../Frameworks/QtCore.framework/Versions/5/QtCore ./dmg/QuNexus\ Editor.app/Contents/MacOS/QuNexus\ Editor
