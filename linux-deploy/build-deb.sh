#!/usr/bin/env bash
# Builds build/qunexus-editor_<version>-1_<arch>.deb. Set NO_BUILD=1 to reuse an existing build.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

pkg=qunexus-editor
upstream=$(sed -n 's/^VERSION *= *\([0-9][0-9.]*\).*/\1/p' qt-qunexus/QuNexus.pro | head -n1)
[ -n "$upstream" ] || { echo "No VERSION found in QuNexus.pro" >&2; exit 1; }
version="$upstream-1"
arch=$(dpkg --print-architecture)
maint="${MAINTAINER:-Dan <dan@hunke.ws>}"
name="${pkg}_${version}_${arch}"
stage="$root/build/deb/$name"
deb="$root/build/$name.deb"

if [ "${NO_BUILD:-0}" != 1 ]; then
    mkdir -p build
    (cd build && qmake6 ../qt-qunexus/QuNexus.pro CONFIG+=release && make -j"$(nproc)")
fi
[ -f "build/QuNexus Editor" ] || { echo "build/QuNexus Editor not found" >&2; exit 1; }

rm -rf "$stage"
install -d -m 0755 "$stage/DEBIAN" "$stage/usr/bin" "$stage/usr/share/applications" \
    "$stage/usr/share/icons/hicolor/256x256/apps" "$stage/usr/share/doc/$pkg"

install -m 0755 -s "build/QuNexus Editor" "$stage/usr/bin/$pkg"
install -m 0644 qt-qunexus/resources/images/quNexusIcon-9.png \
    "$stage/usr/share/icons/hicolor/256x256/apps/$pkg.png"

# Qt on X11 reports the application name ("QuNexus Editor") as the window class
cat > "$stage/usr/share/applications/$pkg.desktop" <<DESKTOP
[Desktop Entry]
Type=Application
Name=QuNexus Editor
Comment=Editor for the Muse Kinetics QuNexus keyboard
Exec=$pkg
Icon=$pkg
Terminal=false
Categories=AudioVideo;Audio;Midi;
StartupWMClass=QuNexus Editor
DESKTOP
chmod 0644 "$stage/usr/share/applications/$pkg.desktop"
if command -v desktop-file-validate >/dev/null; then
    desktop-file-validate "$stage/usr/share/applications/$pkg.desktop"
fi

doc="$stage/usr/share/doc/$pkg"
cp -r Content/Manual Content/SysEx "Content/DAW Support" "$doc/"
find "$doc" \( -name __pycache__ -o -name '*.pyc' -o -name .DS_Store \) -prune -exec rm -rf {} +
find "$doc" -type d -exec chmod 0755 {} +
find "$doc" -type f -exec chmod 0644 {} +

cat > "$doc/copyright" <<COPYRIGHT
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: QuNexus Editor
Source: https://github.com/roge-rm/qunexus-qt6

Files: *
Copyright: KMI Music, Inc.
License: MPL-2.0

License: MPL-2.0
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at https://mozilla.org/MPL/2.0/.
 .
 On Debian systems, the full text of the Mozilla Public License
 version 2.0 can be found in /usr/share/common-licenses/MPL-2.0.
COPYRIGHT
chmod 0644 "$doc/copyright"

# shared library deps from dpkg-shlibdeps, run in a throwaway source tree
sd="$root/build/shlibdeps"
rm -rf "$sd"
mkdir -p "$sd/debian"
printf 'Source: %s\n\nPackage: %s\nArchitecture: any\n' "$pkg" "$pkg" > "$sd/debian/control"
shlibs=$(cd "$sd" && dpkg-shlibdeps -O -e"$stage/usr/bin/$pkg" | sed -n 's/^shlibs:Depends=//p')
rm -rf "$sd"
[ -n "$shlibs" ] || { echo "dpkg-shlibdeps found no dependencies" >&2; exit 1; }
# QML modules are loaded at run time, so shlibdeps can't see them
depends="$shlibs, qml6-module-qtqml, qml6-module-qtquick, qt6-qpa-plugins"

size=$(du -sk --exclude=DEBIAN "$stage" | cut -f1)
cat > "$stage/DEBIAN/control" <<CONTROL
Package: $pkg
Version: $version
Architecture: $arch
Maintainer: $maint
Installed-Size: $size
Depends: $depends
Section: sound
Priority: optional
Homepage: https://github.com/roge-rm/qunexus-qt6
Description: Editor for the Muse Kinetics QuNexus keyboard
 Edit the settings of a QuNexus MIDI keyboard over USB, update its
 firmware and manage presets. The manual and DAW support files are
 in /usr/share/doc/$pkg.
CONTROL
chmod 0644 "$stage/DEBIAN/control"

dpkg-deb --root-owner-group --build "$stage" "$deb"
echo "Built $deb"
