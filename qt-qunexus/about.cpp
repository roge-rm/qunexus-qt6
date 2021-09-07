// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    QString AppVersion = QString("QuNexus Editor v%1.%2.%3")
                               .arg(uchar(applicationVersion.at(0)))
                               .arg(uchar(applicationVersion.at(1)))
                               .arg(uchar(applicationVersion.at(2)));

    title->setMinimumSize(300, 50);
    title->setFixedSize(300, 50);
    title->setStyleSheet("font: bold 16pt;");
    title->move(0, 15);
    title->setAlignment(Qt::AlignCenter);


    QString thisLabelString;
    if (connected)
    {
        thisLabelString = deviceBootloaderVersionString();

        thisLabelString.append(applicationFirmwareVersionString());

        thisLabelString.append(deviceFirmwareVersionString());
    }
    else
    {
        thisLabelString = applicationFirmwareVersionString();

        thisLabelString.append(QString("Device Not Connected"));
    }

    qDebug() << "thisLabelString: " << thisLabelString;
    QLabel* text = new QLabel(thisLabelString, msgBox, Qt::WindowFlags());
    text->setAlignment(Qt::AlignCenter);
    text->setMinimumSize(300, 100);
    text->setFixedSize(300, 100);
    text->move(0, 50);


#ifdef Q_OS_MAC // EB attempt to make this cross platform
    text->setStyleSheet("font: 12pt;");
#else
    text->setStyleSheet("font: 10pt;");
#endif


    QPoint centerparent(
                this->x() + ((this->frameGeometry().width() - msgBox->frameGeometry().width()) /2),
                this->y() + ((this->frameGeometry().height() - msgBox->frameGeometry().height()) /2));

    msgBox->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    msgBox->move(centerparent);

    msgBox->setStyleSheet("QWidget{background: rgba(120,120,120, 255);}");

    QLabel* aboutInfo = new QLabel(QString()
                               , msgBox, Qt::WindowFlags());

    aboutInfo->setMinimumSize(300, 90);
    aboutInfo->setFixedSize(300, 90);
    aboutInfo->setStyleSheet("font:  12pt;");
    aboutInfo->setGeometry(0, 160, 275, 90);
    aboutInfo->setAlignment(Qt::AlignCenter);

    QLabel* sugarImage = new QLabel("asdf", msgBox, Qt::WindowFlags());

    QPixmap sugarPix (":/resources/images/cat_sugar.png");
    sugarImage->setPixmap(sugarPix);
    sugarImage->setScaledContents(true);
    sugarImage->setStyleSheet("QWidget{background-color: transparent;}");
    sugarImage->setGeometry(0, 245, 60, 80);

    aboutInfo->setPixmap(sugarPix);
    aboutInfo->setScaledContents(true);

    QPushButton* updateButton = new QPushButton(msgBox);
    updateButton->setStyleSheet(blueStyleString);
    updateButton->setText("Ok");
    updateButton->setGeometry(QRect(115, 275, 70, 28));
    connect(updateButton, SIGNAL(clicked()), msgBox, SLOT(close()));

    msgBox->exec();

}

about::~about()
{
    delete ui;
}
