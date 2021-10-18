#include "kmi_updates.h"

KMI_Updates::KMI_Updates(QWidget *p, QString an, QSettings *_sessionSettings, QByteArray av)
{
    // initialize
    sessionSettings = _sessionSettings;
    parent = p;

    applicationVersion.resize(3);
    applicationVersion[0] = uchar(av.at(0));
    applicationVersion[1] = uchar(av.at(1));
    applicationVersion[2] = uchar(av.at(2));

    networkAccessManager = new(QNetworkAccessManager);
    appName = an;

    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotUpdateCheckReply(QNetworkReply*)));

    manualUpdateCheck = false;
    slotCheckForUpdates();

}

void KMI_Updates::slotManualCheckForUpdates()
{
    manualUpdateCheck = true;
    slotCheckForUpdates();
}

void KMI_Updates::slotCheckForUpdates()
{
    qDebug() << "slotCheckForUpdates called";

    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    qDebug() << "checking for updates";
    networkAccessManager->get(QNetworkRequest(QUrl("https://files.keithmcmillen.com/products/qunexus/editor/softwareVersionCheck.json")));

}

void KMI_Updates::slotUpdateCheckReply(QNetworkReply *networkReply)
{
    QMessageBox m(QMessageBox::NoIcon, "Software Update", "", QMessageBox::NoButton, 0, Qt::Dialog);

    //m.setStyleSheet("QPushButton {font-family: \"Arial\";font-size: 10px;background-color: rgba(162, 0, 0, 255);border-style: outset;border-radius: 4.0;border-width: 1px;border-color: rgba(0, 51, 76, 255);color: white;} QPushButton:pressed {background-color: rgba(122, 0, 0, 255);} QPushButton:focus  outline: none;}");

    QString messageBoxText;

    if(networkReply->isFinished() && networkReply->error() == QNetworkReply::NoError)
    {
        QByteArray replyArray = networkReply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(replyArray);
        QVariantMap versionMap = jsonDoc.toVariant().toMap();

        QByteArray foundVersion = versionMap.value("Editor").toByteArray();
        //QString currentVersion = aboutDialogForm->editor_version->text();

        qDebug() << uchar((foundVersion[0]) - 48) << uchar((foundVersion[2]) - 48) << uchar((foundVersion[4]) - 48);
        qDebug() << uchar(applicationVersion[0]) << uchar(applicationVersion[1]) << uchar(applicationVersion[2]);
        qDebug() << "foundVersion " << foundVersion << " editor_version" << applicationVersion;

        if ( (uchar(foundVersion[0]) - 48) <= uchar(applicationVersion[0]) &&
             (uchar(foundVersion[2]) - 48) <= uchar(applicationVersion[1]) &&
             (uchar(foundVersion[4]) - 48) <= uchar(applicationVersion[2]) )
        {
            if(manualUpdateCheck) //only display the dialog in this situation if we're checking manually
            {
                messageBoxText = QString("%1 is Up To Date.").arg(appName);
                m.setText(messageBoxText);
                m.addButton("OK", QMessageBox::AcceptRole);
                m.exec();
                manualUpdateCheck = false;
            }
        }
        else
        {
            bool skipUpdate = slotReturnSkipUpdateBool(foundVersion);
            qDebug() << "manualUpdateCheck: " << manualUpdateCheck << "skipUpdate: " << skipUpdate;
            if(manualUpdateCheck || !skipUpdate)
            {
                QString updateMsg = versionMap.value("message").toString();
                messageBoxText = QString("%1 version %2.%3.%4 is available.").arg(appName).arg(foundVersion[0]).arg(foundVersion[2]).arg(foundVersion[4]);
                m.setDetailedText(updateMsg);
                m.setText(messageBoxText);
                m.addButton("Download", QMessageBox::YesRole);
                m.addButton("Skip This Version", QMessageBox::NoRole);
                m.addButton("Remind Me Later", QMessageBox::RejectRole);

                int exitCode = m.exec();

                if(exitCode == 0)
                {
                    slotGoToDownloadsPage();
                }
                else if(exitCode == 1)
                {
                    slotSkipVersion(foundVersion);
                }
                else //in order to get it to actually remind you later, I'm reverting the skip version to default
                {
                    slotSkipVersion("0");
                }
            }
        }
    }
    else
    {
        if(manualUpdateCheck) //only show the error if update is happening manually
        {
            messageBoxText = "An error occurred. Check your internet connection and try again.";
            qDebug() << "software update error:" << networkReply->error();
            m.setText(messageBoxText);
            m.addButton("OK", QMessageBox::AcceptRole);
            m.exec();
        }
    }

    manualUpdateCheck = false;
}

bool KMI_Updates::slotReturnSkipUpdateBool(QString editorVersionFound)
{
    bool skipVersion = false;
    QString skipThisVersionString;

    if(sessionSettings->contains("softwareUpdateSkipVersion"))
    {
        skipThisVersionString = sessionSettings->value("softwareUpdateSkipVersion").toString();
    }
    else
    {
        sessionSettings->setValue("softwareUpdateSkipVersion", "0");
    }

    if(skipThisVersionString == editorVersionFound)
    {
        skipVersion = true;
    }

    qDebug() << "skip this version?" << skipVersion << "editorVersionFound: " << editorVersionFound << "skipVersion: " << skipThisVersionString;

    return skipVersion;
}

void KMI_Updates::slotGoToDownloadsPage()
{
    QDesktopServices::openUrl(QUrl("http://www.keithmcmillen.com/downloads/#qunexus"));
}

void KMI_Updates::slotSkipVersion(QString versionToSkip)
{
    sessionSettings->setValue("softwareUpdateSkipVersion", versionToSkip);
}

