// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef PRESETNAMEDIALOG_H
#define PRESETNAMEDIALOG_H

#include <QDialog>
#include <QtWidgets> // was QT Gui
#include <QLineEdit>

class PresetNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PresetNameDialog(QWidget *parent = 0);

   QLineEdit* lineEdit;
    
signals:
    void signalCancel();
    void signalSaveNewPreset(QString presetName);
    
public slots:
    void slotCancel();
    void slotSave();
    
};

#endif // PRESETNAMEDIALOG_H
