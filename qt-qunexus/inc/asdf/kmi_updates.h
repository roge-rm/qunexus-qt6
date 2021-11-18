#ifndef KMI_UPDATES_H
#define KMI_UPDATES_H

#include <QtWidgets>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class KMI_Updates : public QWidget
{
    Q_OBJECT

public:
    explicit KMI_Updates(QWidget *p = 0, QString an = "", QSettings *sessionSettings = 0, QByteArray av = 0);

    QWidget *parent;
    QSettings *sessionSettings;
    QNetworkAccessManager *networkAccessManager;
    QByteArray applicationVersion;
    QString appName;

    bool manualUpdateCheck;

public slots:

    void slotManualCheckForUpdates();
    void slotCheckForUpdates();
    void slotUpdateCheckReply(QNetworkReply *networkReply);
    bool slotReturnSkipUpdateBool(QString editorVersionFound);
    void slotGoToDownloadsPage();
    void slotSkipVersion(QString versionToSkip);
};

#endif // KMI_UPDATES_H
