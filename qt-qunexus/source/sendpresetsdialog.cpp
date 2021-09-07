// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "sendpresetsdialog.h"
#include <QApplication>
#include <QLabel>

SendPresetsDialog::SendPresetsDialog(QWidget *parent, QString presetName, PresetManager *presetManager) :
    QDialog(parent)
{
    pM = presetManager;

    //StyleSheets for Preset Library
    QFile* blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    blueStyleString = QLatin1String(blueStyleFile->readAll());

    this->setStyleSheet("QDialog{ background: rgb(100,100,100); border-color:black; border:5px;} QCheckBox{ color: white; font: 10pt 'Arial';} QLabel{color: white; font: 12pt 'Arial';}");
    this->setFixedSize(500, 100);
    this->setMaximumWidth(500);
    this->setMaximumHeight(100);

    QPoint centerparent(
    parent->x() + ((parent->frameGeometry().width() - this->frameGeometry().width()) /2),
    parent->y() + ((parent->frameGeometry().height() - this->frameGeometry().height()) /2));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->move(centerparent);

    //this->setWindowModality(Qt::WindowModal);

    QLabel* label = new QLabel(QString("Preset \"%1\" saved. Send to Keyboard?").arg(presetName), this, Qt::WindowFlags());
    label->setGeometry(QRect(25,5,400,25));

    doNotShowAgain = new QCheckBox("Do not show this dialog again.", this);
    doNotShowAgain->move(25,30);
    connect(doNotShowAgain, SIGNAL(clicked()), this, SLOT(slotDoNotShow()));


    QPushButton* cancelButton = new QPushButton(this);
    cancelButton->setText("Don't Send");
    cancelButton->setGeometry(QRect(25, 60, 80, 28));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));


    QPushButton* sendA = new QPushButton(this);
    sendA->setStyleSheet(blueStyleString);
    sendA->setGeometry(QRect(125, 60, 80, 28));
    sendA->setObjectName("syncPreset0");
    sendA->setText("Send A");
    connect(sendA, SIGNAL(clicked()), presetManager, SLOT(slotSyncEditBufferToDevicePreset()));
    connect(sendA, SIGNAL(clicked()), this, SLOT(slotSendPresets()));

    QPushButton* sendB = new QPushButton(this);
    sendB->setStyleSheet(blueStyleString);
    sendB->setGeometry(QRect(215, 60, 80, 28));
    sendB->setObjectName("syncPreset1");
    sendB->setText("Send B");
    connect(sendB, SIGNAL(clicked()), presetManager, SLOT(slotSyncEditBufferToDevicePreset()));
    connect(sendB, SIGNAL(clicked()), this, SLOT(slotSendPresets()));

    QPushButton* sendC = new QPushButton(this);
    sendC->setStyleSheet(blueStyleString);
    sendC->setGeometry(QRect(305, 60, 80, 28));
    sendC->setObjectName("syncPreset2");
    sendC->setText("Send C");
    connect(sendC, SIGNAL(clicked()), presetManager, SLOT(slotSyncEditBufferToDevicePreset()));
    connect(sendC, SIGNAL(clicked()), this, SLOT(slotSendPresets()));

    QPushButton* sendD = new QPushButton(this);
    sendD->setStyleSheet(blueStyleString);
    sendD->setGeometry(QRect(395, 60, 80, 28));
    sendD->setObjectName("syncPreset3");
    sendD->setText("Send D");
    connect(sendD, SIGNAL(clicked()), presetManager, SLOT(slotSyncEditBufferToDevicePreset()));
    connect(sendD, SIGNAL(clicked()), this, SLOT(slotSendPresets()));

}

void SendPresetsDialog::slotSendPresets()
{

    this->close();
    this->destroy();
}

void SendPresetsDialog::slotCancel()
{
    this->close();
    this->destroy();
}

void SendPresetsDialog::slotDoNotShow()
{
    if(doNotShowAgain->isChecked())
    {
        QSettings settings;
        settings.setValue("hideSendPresets", 1);
    }

}

