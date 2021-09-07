// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "presetnamedialog.h"
#include <QLabel>

PresetNameDialog::PresetNameDialog(QWidget *parent) :
    QDialog(parent)
{    

    //StyleSheets for Preset Library
    QFile* blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    QString blueStyleString = QLatin1String(blueStyleFile->readAll());

    this->setStyleSheet("QDialog{ background: rgba(120,120,120,255);} QCheckBox{ color: white; font: 10pt 'Arial';} QLabel{color: white; font: 10pt 'Arial';}");
    this->setFixedSize(200, 100);
    this->setMaximumWidth(200);
    this->setMaximumHeight(100);

    QPoint centerparent(
    parent->x() + ((parent->frameGeometry().width() - this->frameGeometry().width()) /2),
    parent->y() + ((parent->frameGeometry().height() - this->frameGeometry().height()) /2));

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->move(centerparent);

    QLabel* label = new QLabel("Preset Name:", this, Qt::WindowFlags());
    label->setGeometry(QRect(25,25,150,25));

    lineEdit = new QLineEdit(this);
    lineEdit->setLayoutDirection(Qt::RightToLeft);
    lineEdit->setStyleSheet("background: rgba(67,67,67,255); color: white; border-radius:4.0;");
    lineEdit->setGeometry(QRect(25, 25, 150, 25));
    lineEdit->setMaxLength(18);
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(slotSave()));

    QPushButton* cancelButton = new QPushButton(this);
    cancelButton->setText("Cancel");
    cancelButton->setGeometry(QRect(25, 60, 70, 28));
    //cancelButton->move(25, 60);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));

    QPushButton* saveButton = new QPushButton(this);
    saveButton->setStyleSheet(blueStyleString);
    saveButton->setText("Save");
    saveButton->setGeometry(QRect(105, 60, 70, 28));
    //saveButton->move(200 - saveButton->width(), 60);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSave()));

    //connect(this, SIGNAL(signalSaveNewPreset(QString)), parent, SLOT(slotSaveEditBufferToLibrary(QString)));
}

void PresetNameDialog::slotCancel()
{
    this->close();
    this->destroy();
}

void PresetNameDialog::slotSave()
{

    if(lineEdit->text().count() > 0)
    {
        emit signalSaveNewPreset(lineEdit->text());
        this->close();
        this->destroy();
    }
    else
    {

    }
}
