// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#ifndef TABLEINTERFACE_H
#define TABLEINTERFACE_H

#include <QWidget>
#include <QtGui>
#include <QLayout>
//#include <QDeclarativeView>
//#include <QDeclarativeContext>
#include <QWidget>
#include <qqmlengine.h>
#include <qquickview.h>
#include <QQmlContext>
#include <QDebug>
#include <qobject.h>
#include <QQuickWindow>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlProperty>


#include "ui_tableviewerform.h"


class TableInterface : public QWidget
{
    Q_OBJECT
public:
    explicit TableInterface(QWidget *parent = 0);

    unsigned char userTable1[128];
    unsigned char userTable2[128];
    QQuickItem* rootObject;

    QQuickWidget* tableQuickWidget;

    QWidget *qmlWidget;

    QMap<QString, unsigned char *> tableMap;

    //Buttons
    QFile* blueStyleFile;
    QString blueStyleString;

signals:
    void signalSetStep();
    void signalGetUserTables();
    void signalSendUserTables();

    void signalUpdateCol(int col, int val);
    void signalSetTableColorWriteEnabled(bool);

public slots:
    void slotDisplayUserTable(int userTableNum);
    void slotLoadTableTemplate();
    void slotMenuChanged(QString);
    void slotSendTablesToBoard();
    void slotEditUserTable(int x, int y);

    void slotSetCustomTemplate();

    //--------------------------
    void slotDisplayTable(QString tableName);

    void slotEnable(bool state);
    void slotMouseClicked(int x, int y);

private:
    Ui::TableViewerForm tableViewerForm;
    
};

#endif // TABLEINTERFACE_H
