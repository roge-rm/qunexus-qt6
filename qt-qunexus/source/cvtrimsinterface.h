// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef CVTRIMSINTERFACE_H
#define CVTRIMSINTERFACE_H

#include "ui_cvtrimsviewerform.h"
#include <QWidget>
#include <QTimer>

class CvTrimsInterface : public QWidget
{
    Q_OBJECT
public:
    explicit CvTrimsInterface(QWidget *parent = 0);

    double voltPerOctaveTrim;
    int voltPerOctaveOffset;

    double _1_2_voltPerOctaveTrim;
    int _1_2_voltPerOctaveOffset;

    double hzPerVoltTrim;
    int hzPerVoltOffset;

    QTimer *timeout;
    
signals:
    void signalSendToBoard();
    
public slots:
    void slotResetVars();
    void slotSetVars();
    void slotShowTrims();
    void slotShowOffsets();
    void slotTimeout();
    void slotEnable(bool state);
    
private:
   Ui::CvTrimsViewerForm cvViewerForm;
};

#endif // CVTRIMSINTERFACE_H
