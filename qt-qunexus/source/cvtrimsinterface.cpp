// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "cvtrimsinterface.h"

#include "QDebug"

CvTrimsInterface::CvTrimsInterface(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    this->setFixedSize(160, 350);
    this->setWindowTitle("CV Trims");
    //-------- Setup Ui
    QWidget *uiWidget = new QWidget(this);
    cvViewerForm.setupUi(uiWidget);


    connect(cvViewerForm.voOffset, SIGNAL(valueChanged(int)), this, SLOT(slotSetVars()));
    connect(cvViewerForm.hzVoltOffset, SIGNAL(valueChanged(int)), this, SLOT(slotSetVars()));
    connect(cvViewerForm.oneTwoVoOffset, SIGNAL(valueChanged(int)), this, SLOT(slotSetVars()));

    connect(cvViewerForm.voTrim, SIGNAL(valueChanged(double)), this, SLOT(slotSetVars()));
    connect(cvViewerForm.hzVoltTrim, SIGNAL(valueChanged(double)), this, SLOT(slotSetVars()));
    connect(cvViewerForm.oneTwoVoTrim, SIGNAL(valueChanged(double)), this, SLOT(slotSetVars()));

    timeout = new QTimer(this);
    connect(timeout, SIGNAL(timeout()), this, SLOT(slotTimeout()));

}


void CvTrimsInterface::slotSetVars()
{
    qDebug() << "object name" << QObject::sender()->objectName();
    timeout->stop();

    //qDebug() << "slot set vars called in cv interface................";

    if(QObject::sender()->objectName().contains("voTrim"))
    {
        voltPerOctaveTrim  = cvViewerForm.voTrim->value();
    }
    else if(QObject::sender()->objectName().contains("voOffset"))
    {
        voltPerOctaveOffset = cvViewerForm.voOffset->value();
    }
    else if(QObject::sender()->objectName().contains("oneTwoVoTrim"))
    {
        _1_2_voltPerOctaveTrim = cvViewerForm.oneTwoVoTrim->value();
    }
    else if(QObject::sender()->objectName().contains("oneTwoVoOffset"))
    {
        _1_2_voltPerOctaveOffset = cvViewerForm.oneTwoVoOffset->value();
    }
    else if(QObject::sender()->objectName().contains("hzVoltTrim"))
    {
        hzPerVoltTrim = cvViewerForm.hzVoltTrim->value();

    }
    else if(QObject::sender()->objectName().contains("hzVoltOffset"))
    {
        hzPerVoltOffset = cvViewerForm.hzVoltOffset->value();
    }

    timeout->start(250);
}

void CvTrimsInterface::slotShowTrims()
{
    cvViewerForm.voTrim->setValue(voltPerOctaveTrim);
    cvViewerForm.oneTwoVoTrim->setValue(_1_2_voltPerOctaveTrim);
    cvViewerForm.hzVoltTrim->setValue(hzPerVoltTrim);
}

void CvTrimsInterface::slotShowOffsets()
{
    cvViewerForm.voOffset->setValue(voltPerOctaveOffset);
    cvViewerForm.oneTwoVoOffset->setValue(_1_2_voltPerOctaveOffset);
    cvViewerForm.hzVoltOffset->setValue(hzPerVoltOffset);
}

void CvTrimsInterface::slotTimeout()
{
    timeout->stop();
    emit signalSendToBoard();
}

void CvTrimsInterface::slotEnable(bool state)
{
    if(state)
    {
      cvViewerForm.masterFrame->hide();
    }
    else
    {
       cvViewerForm.masterFrame->show();
    }
}
