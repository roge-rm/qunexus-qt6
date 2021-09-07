// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef SYSEXDECODE_H
#define SYSEXDECODE_H

#include <QWidget>
#include "Qn_preset_struct.h"

class SysExDecode : public QWidget
{
    Q_OBJECT
public:
    explicit SysExDecode(QWidget *parent = 0);

    struct QUNEXUS qn;
    
signals:
    void signalSendGlobals(QString, QByteArray);
    void signalPresetReceived(unsigned char);
    
public slots:
    void slotProcessGlobals(QByteArray sysExMessageByteArray);
    void slotDecodeGlobals(QByteArray globalsByteArray);
    bool slotIsGlobals(QByteArray ba);
    bool slotIsPreset(QByteArray ba);
    void slotProcessPreset(QByteArray presetPacket);
    void slotDecodePreset(QVariantMap * preset);
};

#endif // SYSEXDECODE_H
