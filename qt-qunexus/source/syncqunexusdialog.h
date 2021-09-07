// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef SYNCQUNEXUSDIALOG_H
#define SYNCQUNEXUSDIALOG_H

//#include <QMessageBox>
#include <QtWidgets> // was QT Gui
#include "presetmanager.h"
#include <QCheckBox>

//#include <mainwindow.h>
// had to duplicate this from mainwindow.h to prevent build error
#ifdef Q_OS_MAC
    #define APP_WIDTH 850
    #define APP_HEIGHT 465
#else
    #define APP_WIDTH 850
    #define APP_HEIGHT 490
#endif

class SyncQuNexusDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SyncQuNexusDialog(QWidget *parent = 0);

    QWidget* mainWindowPtr;
    QWidget* thisWindowPtr;

    QString blueStyleString;
    QString grayStyleString;
    
signals:
    void signalCancel();
    void signalCloseSyncQuNexusDialog(unsigned char);

public slots:
    void slotCancel();
    void slotCenterDialog();
    
};

#endif // SYNCQUNEXUSDIALOG_H
