// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef SENDPRESETSDIALOG_H
#define SENDPRESETSDIALOG_H

//#include <QMessageBox>
#include <QtWidgets> // was QT Gui
#include "presetmanager.h"
#include <QCheckBox>

class SendPresetsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SendPresetsDialog(QWidget *parent = 0, QString presetName = "None", PresetManager* presetManager = 0);

    QCheckBox* doNotShowAgain;
    QString blueStyleString;
    PresetManager *pM;
    
signals:
    void signalSysExEncodePreset(QVariantMap, unsigned char);
    
public slots:
    void slotSendPresets();
    void slotCancel();
    void slotDoNotShow();
    
};

#endif // SENDPRESETSDIALOG_H
