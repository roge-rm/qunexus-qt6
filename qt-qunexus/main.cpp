// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    // set highdpi scale factor rounding
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication a(argc, argv);

    // this sets the correct OpenGL api depending on which version of Qt we are using
#if QT_VERSION >= 0x060000
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#else
    //QSGRendererInterface::GraphicsApi(QSGRendererInterface::OpenGL);
#endif



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
