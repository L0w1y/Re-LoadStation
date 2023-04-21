
#include "loader.h"
#include "qnetwork.h"

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <qeventloop.h>
#include <qfile.h>
#include <QFile>
#include <qiodevice.h>

loader::loader()
{

}

void loader::Download(QUrl downloadUrl, QString downloadPath)
{
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(downloadUrl));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        QFile file(downloadPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
        }
    }
    reply->deleteLater();
}

QString loader::fetchWork(QUrl UserLink)
{
    return QString();
}

QUrl loader::ExtractMviewLink(QString Target)
{
    return QUrl();
}

