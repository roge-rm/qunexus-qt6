// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "tableinterface.h"
#include "ui_tableviewerform.h"
#include "qquickview.h"
#include <QQmlContext>
#include <qqmlengine.h>
#include <qqmlcomponent.h>
#include <qquickitem.h>

unsigned char linear[128] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127};
unsigned char exponential[128] = {0, 3, 7, 10, 13, 16, 19, 22, 24, 27, 29, 32, 34, 36, 38, 40, 42, 43, 45, 47, 49, 50, 52, 53, 55, 56, 58, 59, 60, 62, 63, 64, 65, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 81, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99, 100, 101, 101, 102, 103, 103, 104, 104, 105, 106, 106, 107, 107, 108, 108, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 123, 124, 124, 125, 125, 125, 126, 126, 127, 127, 127};
unsigned char sine[128] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 26, 27, 28, 30, 31, 32, 34, 35, 37, 38, 40, 41, 43, 44, 46, 47, 49, 50, 52, 53, 55, 56, 58, 60, 61, 63, 64, 66, 67, 69, 71, 72, 74, 75, 77, 78, 80, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 97, 99, 100, 101, 103, 104, 105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 117, 118, 119, 120, 121, 121, 122, 123, 123, 124, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127};
unsigned char cosine[128] = {127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 124, 123, 123, 122, 121, 121, 120, 119, 118, 117, 117, 116, 115, 114, 113, 112, 111, 110, 108, 107, 106, 105, 104, 103, 101, 100, 99, 97, 96, 95, 93, 92, 90, 89, 87, 86, 84, 83, 81, 80, 78, 77, 75, 74, 72, 71, 69, 67, 66, 64, 63, 61, 60, 58, 56, 55, 53, 52, 50, 49, 47, 46, 44, 43, 41, 40, 38, 37, 35, 34, 32, 31, 30, 28, 27, 26, 24, 23, 22, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 10, 9, 8, 7, 6, 6, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char logarithmic[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 41, 42, 43, 44, 45, 47, 48, 49, 50, 52, 53, 54, 55, 57, 58, 60, 61, 62, 64, 65, 67, 68, 70, 71, 73, 74, 76, 77, 79, 80, 82, 84, 85, 87, 89, 90, 92, 94, 96, 97, 99, 101, 103, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 125, 127};
unsigned char invert[128] = {127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

TableInterface::TableInterface(QWidget *parent) :
    QWidget(parent)
{
    //Make Window a Tool Window, Set Size
    this->setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
    this->setFixedSize(540,600);
    this->setWindowTitle("Tables");

    //Store tables in Map
    tableMap.insert("Linear", linear);
    tableMap.insert("Logarithmic", logarithmic);
    tableMap.insert("Sine", sine);
    tableMap.insert("Cosine", cosine);
    tableMap.insert("Exponential", exponential);
    tableMap.insert("Invert", invert);
    tableMap.insert("Custom 1", userTable1);
    tableMap.insert("Custom 2", userTable2);

    //Init User Tables
    for(int i = 0; i < 128; i++)
    {
        userTable1[i] = linear[i];
        userTable2[i] = linear[i];
    }

    //-------- Setup Ui
    QWidget *uiWidget = new QWidget(this);
    tableViewerForm.setupUi(uiWidget);

    //-------- Setup QML Ui

    rootObject = tableViewerForm.quickWidget->rootObject(); // root object to find other objects with

    tableViewerForm.quickWidget->rootContext()->setContextProperty("TableInterface", this); // connects QML class references to this object

    // style buttons

    //StyleSheets for "blue" (red) pushbuttons
    blueStyleFile = new QFile(":stylesheets/resources/stylesheets/BlueButtonStyleSheet.qss");
    blueStyleFile->open(QFile::ReadOnly);
    blueStyleString = QLatin1String(blueStyleFile->readAll());

    tableViewerForm.Cosine->setStyleSheet(blueStyleString);
    tableViewerForm.Sine->setStyleSheet(blueStyleString);
    tableViewerForm.Logarithmic->setStyleSheet(blueStyleString);
    tableViewerForm.Invert->setStyleSheet(blueStyleString);
    tableViewerForm.Linear->setStyleSheet(blueStyleString);
    tableViewerForm.Exponential->setStyleSheet(blueStyleString);



    qDebug() << "connect";
    //Connect Buttons
    connect(tableViewerForm.Cosine, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));
    connect(tableViewerForm.Sine, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));
    connect(tableViewerForm.Logarithmic, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));
    connect(tableViewerForm.Invert, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));
    connect(tableViewerForm.Linear, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));
    connect(tableViewerForm.Exponential, SIGNAL(clicked()), this, SLOT(slotSetCustomTemplate()));

    //Connect Menu
    connect(tableViewerForm.TableMenu, SIGNAL(currentTextChanged(QString)), this, SLOT(slotMenuChanged(QString)));

    //Init Viewer to User Table 1
    slotMenuChanged("Linear");
    tableViewerForm.TableMenu->setCurrentIndex(tableViewerForm.TableMenu->findText("Linear"));

    //qDebug() << qmlView->rootObject()->property("editable");
}

// load one of the pre-made tables into a custom slot
void TableInterface::slotLoadTableTemplate()
{

    qDebug() << "slotLoadTableTemplate called";
    //Get Table to load into viewer based on input, either template button or menu change
    QString tableName;

    if(!QObject::sender()->objectName().contains("TableMenu"))
    {
        tableName = QObject::sender()->objectName();
    }
    else
    {
        tableName = tableViewerForm.TableMenu->currentText();
    }

    qDebug() << "tableName" << tableName;


    //Displays the custom user table
    if(!tableName.contains("Custom"))
    {
        unsigned char *table;

        table = tableMap.value(tableName);

        for(int i = 0; i < 128; i++)
        {
            emit signalUpdateCol(i, (int)table[i]);
        }
    }
}

// the dropdown selects which table we are looking at
void TableInterface::slotMenuChanged(QString itemName)
{

    if(!itemName.contains("Custom"))
    {
        rootObject->setProperty("editable", 0);

        tableViewerForm.Cosine->hide();
        tableViewerForm.Sine->hide();
        tableViewerForm.Logarithmic->hide();
        tableViewerForm.Invert->hide();
        tableViewerForm.Linear->hide();
        tableViewerForm.Exponential->hide();

//        tableViewerForm.Cosine->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");
//        tableViewerForm.Sine->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");
//        tableViewerForm.Logarithmic->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");
//        tableViewerForm.Invert->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");
//        tableViewerForm.Linear->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");
//        tableViewerForm.Exponential->setStyleSheet("QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: rgb(10,10,10); font:10px \"Arial\"; color: rgb(100,100,100);}");

        for(int i = 0; i < 128; i++)
        {
            emit signalSetTableColorWriteEnabled(false); // set colors to red
        }
        slotDisplayTable(itemName);
    }
    else
    {
        rootObject->setProperty("editable", 1);

        tableViewerForm.Cosine->show();
        tableViewerForm.Sine->show();
        tableViewerForm.Logarithmic->show();
        tableViewerForm.Invert->show();
        tableViewerForm.Linear->show();
        tableViewerForm.Exponential->show();

//        tableViewerForm.Cosine->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");
//        tableViewerForm.Sine->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");
//        tableViewerForm.Logarithmic->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");
//        tableViewerForm.Invert->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");
//        tableViewerForm.Linear->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");
//        tableViewerForm.Exponential->setStyleSheet("QPushButton:pressed {background-color: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(67, 67, 67, 255), stop:1 rgba(150, 150, 150, 255));}QPushButton{border-radius: 4.0; border: 1px solid rgb(50,50,50); background: qlineargradient(spread:repeat, x1:1, y1:0, x2:1, y2:1, stop:0 rgb(255, 255, 255), stop:1 rgb(150, 150, 150)); font:10px \"Arial\";}");

        for(int i = 0; i < 128; i++)
        {
            emit signalSetTableColorWriteEnabled(true); // set color to green
        }
        slotDisplayUserTable(!itemName.contains("1"));
    }
}

void TableInterface::slotEditUserTable(int x, int y)
{
    qDebug() << "edit user table - x:" << x << "y:" << y;

    if(tableViewerForm.TableMenu->currentText().contains("Custom 1"))
    {
        userTable1[x] = y;
    }
    else
    {
        userTable2[x] = y;
    }
}

void TableInterface::slotSendTablesToBoard()
{
    if(tableViewerForm.TableMenu->currentText().contains("Custom"))
    {
        qDebug() << "Send Tables to Board--- update globals";
        emit signalSendUserTables();
    }
}

// display one of the stock tables
void TableInterface::slotDisplayTable(QString tableName)
{
    unsigned char *table;

    table = tableMap.value(tableName);

    for(int i = 0; i < 128; i++)
    {
        emit signalUpdateCol(i, (int)table[i]);
    }
}

// display a user table on the table ui
void TableInterface::slotDisplayUserTable(int userTableNum)
{
    qDebug() << "slotDisplayUserTable called - userTableNum: " << userTableNum;
    for(int i = 0; i < 128; i++)
    {
        emit signalUpdateCol(i, ((userTableNum) ? (int)userTable2[i] : (int)userTable1[i] )); // send the correct table's value
    }
}

// load one of the standard curves into a custom slot
void TableInterface::slotSetCustomTemplate()
{
    QString tableName = QObject::sender()->objectName();

    unsigned char *table;

    table = tableMap.value(tableName);

    for(int i = 0; i < 128; i++)
    {
        emit signalUpdateCol(i, (int)table[i]);

        if(tableViewerForm.TableMenu->currentText().contains("Custom 1"))
        {
            userTable1[i] = table[i];
        }
        else
        {
            userTable2[i] = table[i];
        }
    }

    slotSendTablesToBoard();
}

void TableInterface::slotEnable(bool state)
{
    if(state)
    {
        tableViewerForm.masterFrame->hide();
        //qmlWidget->show();

    }
    else
    {
        tableViewerForm.masterFrame->show();
        //qmlWidget->hide();
    }
}

void TableInterface::slotMouseClicked(int x, int y)
{
    qDebug() << "Mouse x: " << x << " y: " << y;

    if(!tableViewerForm.TableMenu->currentText().contains("Custom")) return; // only allow editing when a custom table is loaded

    // connect mouse clicks to table interface
    emit signalUpdateCol(x, y);

    // edit user table, invert Y
    slotEditUserTable(x, y);
    //slotSendTablesToBoard(); // handled on mouse release
}
