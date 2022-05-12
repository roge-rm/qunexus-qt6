// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "syncqunexusdialog.h"
#include <QApplication>
#include <QLabel>

#define SYNC_WINDOW_WIDTH 600
#define SYNC_WINDOW_HEIGHT 100

SyncQuNexusDialog::SyncQuNexusDialog(QWidget *parent) :
    QDialog(parent)
{    
    mainWindowPtr = parent;
    thisWindowPtr = this;

    //StyleSheets for red button
    QFile* blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    blueStyleString = QLatin1String(blueStyleFile->readAll());

    //StyleSheets for grey button
    QFile* grayStyleFile = new QFile(":stylesheets/resources/stylesheets/GrayButtonStyleSheet.qss");
    grayStyleFile->open(QFile::ReadOnly);
    grayStyleString = QLatin1String(grayStyleFile->readAll());

    this->setStyleSheet("QDialog{ background: rgb(100,100,100); border-color:black; border:5px;} QCheckBox{ color: white; font: 10pt 'Arial';} QLabel{color: white; font: 12pt 'Arial';}");
    this->setFixedSize(SYNC_WINDOW_WIDTH, SYNC_WINDOW_HEIGHT);
    this->setMaximumWidth(SYNC_WINDOW_WIDTH);
    this->setMaximumHeight(SYNC_WINDOW_HEIGHT);

    slotCenterDialog();

    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint );
    this->setWindowModality(Qt::WindowModal);

    QLabel* label = new QLabel(QString("QuNexus Connected. Choose Sync Method:"), this, Qt::WindowFlags());
    label->setGeometry(QRect(0,15,SYNC_WINDOW_WIDTH,SYNC_WINDOW_HEIGHT/4));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("background-color: rgba(0,0,0,0);");

    QPushButton* syncEditorToQn = new QPushButton(this);
    syncEditorToQn->setStyleSheet(grayStyleString);
    syncEditorToQn->setGeometry(QRect(25, 60, 150, 28));
    syncEditorToQn->setObjectName("syncEditorToQn");
    syncEditorToQn->setText("Editor to QuNexus");
    connect(syncEditorToQn, SIGNAL(clicked()), parent, SLOT(slotSyncEditorToQn()));
    connect(syncEditorToQn, SIGNAL(clicked()), this, SLOT(slotCancel()));

    QPushButton* syncQnToEditor = new QPushButton(this);
    syncQnToEditor->setStyleSheet(blueStyleString);
    syncQnToEditor->setGeometry(QRect(225, 60, 150, 28));
    syncQnToEditor->setObjectName("syncQnToEditor");
    syncQnToEditor->setText("QuNexus to Editor");
    connect(syncQnToEditor, SIGNAL(clicked()), parent, SLOT(slotSyncQnToEditor()));
    connect(syncQnToEditor, SIGNAL(clicked()), this, SLOT(slotCancel()));

    QPushButton* syncQnToEditorNew = new QPushButton(this);
    syncQnToEditorNew->setStyleSheet(blueStyleString);
    syncQnToEditorNew->setGeometry(QRect(425, 60, 150, 28));
    syncQnToEditorNew->setObjectName("syncQnToEditor");
    syncQnToEditorNew->setText("QuNexus to Editor (new)");
    connect(syncQnToEditorNew, SIGNAL(clicked()), parent, SLOT(slotSyncQnToEditorNewPreset()));
    connect(syncQnToEditorNew, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

void SyncQuNexusDialog::slotCancel()
{
    emit signalCloseWindow();
    this->done(1);
}

void SyncQuNexusDialog::slotCenterDialog()
{
//#ifdef Q_OS_MAC
    int mainX = mainWindowPtr->x();
    int mainY = mainWindowPtr->y();
//#else
//    int mainX = 0;
//    int mainY = 0;
//#endif
//    int thisX = thisWindowPtr->x();
//    int thisY = thisWindowPtr->y();

    QPoint centerparent(
    mainX + ( (APP_WIDTH/2) - (SYNC_WINDOW_WIDTH/2)),
    mainY + 55 );

    thisWindowPtr->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    thisWindowPtr->setWindowModality(Qt::NonModal);
    //qDebug() << "Sync QuNexus Dialog centerparent: " << centerparent << " mx: " << mainX << " my: " << mainY << " tx: " << thisX << " ty: " << thisY;
    thisWindowPtr->move(centerparent);
}


