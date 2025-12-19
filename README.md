# QuNexus Editor

A comprehensive editor application for the QuNexus MIDI keyboard controller, enabling users to configure presets, CV trims, sensitivity settings, and advanced performance parameters.

## Overview

The QuNexus Editor is a cross-platform Qt application that provides complete control over your QuNexus keyboard controller. Configure everything from individual key sensitivities to global CV trim settings, manage preset libraries, and update firmware.

## Features

- **Preset Management**: Create, edit, and organize performance presets
- **Key Sensitivity Configuration**: Individual per-key velocity and pressure settings
- **CV Trim Controls**: Precise calibration of continuous voltage outputs
- **Global Settings**: System-wide configuration options
- **Firmware Updates**: Built-in firmware update capability
- **Factory Reset Options**: Reset CV trims, key sensitivities, and restore factory presets
- **Cross-Platform Support**: Native builds for macOS and Windows

## System Requirements

### Minimum Requirements
- **macOS**: 10.15 or later
- **Windows**: Windows 10 or later
- **Qt Framework**: 6.3.2 or later

### Development Requirements
- Qt 6.3.2+ with Qt Creator
- C++17 compatible compiler
- CMake 3.16+ or qmake

## Installation

### Pre-built Releases
Download the latest release for your platform from the [Muse Kinetics downloads page](https://www.musekinetics.com/downloads).

### Building from Source

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Muse-Kinetics/qunexus-qt6.git
   cd qunexus-qt6
   ```

2. **Install Qt 6.3.2+**:
   - Download from [Qt.io](https://www.qt.io/download)
   - Ensure Qt Creator and development tools are installed

3. **Build the application**:
   ```bash
   # Using Qt Creator (recommended)
   # Open the .pro file in Qt Creator and build

   # Using command line with qmake
   qmake
   make
   ```

4. **Windows-specific build steps**:
   - Additional deployment steps are required for Windows
   - See the deployment section in the .PRO file for detailed instructions
   - Ensure proper Qt DLL deployment for distribution

## Usage

### Connecting Your QuNexus
1. Connect your QuNexus keyboard via USB
2. Launch the QuNexus Editor
3. The application will automatically detect your device

### Basic Operation
- **Load Presets**: Access factory and user presets from the preset library
- **Edit Settings**: Modify key sensitivities, CV trims, and global parameters
- **Save Changes**: Store your modifications to device memory
- **Backup/Restore**: Export and import preset configurations

### Advanced Features
- **Factory Reset**: Hardware menu → Factory Reset (resets CV trims, sensitivities, globals)
- **Firmware Updates**: Load new firmware files through the File menu
- **Preset Library Management**: Reset factory presets to original defaults

## Development

### Project Structure
```
qt-qunexus/                 # Main application source
├── resources/              # Application resources
│   ├── firmware/          # Firmware files
│   └── images/            # UI graphics and icons
├── Signing and Notarization/  # macOS code signing scripts
└── win-build/             # Windows build configurations
```

### Code Signing (macOS)
The project includes automated code signing and notarization scripts for macOS distribution. Ensure you have:
- Valid Apple Developer ID certificate
- App-specific password for notarization
- Configured environment variables for signing

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on both platforms
5. Submit a pull request

## Support

QuNexus is currently available for sale through Muse Kinetics with a one year warranty that includes customer support, however this source code is provided as-is with no warranty. For product support please visit https://support.musekinetics.com. For feature requests, or bug reports, please create an issue in this repository and encourage other open source developers to assist you.

## License
The QuNexus Editor is distributed under the Mozilla Public License 2.0 (MPL-2.0). You may use, modify, and redistribute the source as long as each modified source file continues to carry the MPL notice and the file is made available under the MPL. See [LICENSE](LICENSE) for the complete terms.

Unless otherwise noted, all contributions are accepted under the MPL-2.0 with the header:

```
// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


## Third-Party Licenses

### Qt Framework

This application is built using the Qt framework, which is available under the GNU Lesser General Public License (LGPL) v3.

- **Qt Website**: https://www.qt.io/
- **Qt Open Source License**: LGPL v3
- **Qt Source Code**: https://www.qt.io/download-open-source
- **Qt License Details**: https://www.qt.io/licensing/open-source-lgpl-obligations

The QuNexus Editor application dynamically links with Qt libraries and does not modify the Qt framework source code. Users have the right to obtain, modify, and redistribute the Qt libraries under the terms of the LGPL v3 license.

### Qt LGPL v3 Summary

The Qt framework is licensed under LGPL v3, which allows:
- Use in both open source and commercial applications
- Dynamic linking without affecting your application's license
- Distribution of Qt libraries alongside your application

For complete LGPL v3 license terms, see: https://www.gnu.org/licenses/lgpl-3.0.html

