// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    // fonts
    //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication a(argc, argv);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    MainWindow w;
    //w.resize(800, 600);
    w.setWindowTitle("QuNexus Editor");

#ifdef Q_OS_MAC

#else
        w.setFixedHeight(w.size().height() + 25);
#endif

    w.setFixedSize(w.size());
    w.show();

    return a.exec();
}
