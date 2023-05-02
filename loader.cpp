#include "loader.h"
#include "qnetwork.h"
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <qeventloop.h>
#include <qfile.h>
#include <QFile>
#include <qiodevice.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMessageBox>

//void loader::Download(QUrl downloadUrl, QString downloadPath)
//{
//    QNetworkAccessManager manager;
//    QNetworkReply *reply = manager.get(QNetworkRequest(downloadUrl));
//    QEventLoop loop;
//    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    loop.exec();
//    if (reply->error() == QNetworkReply::NoError) {
//        QFile file(downloadPath);
//        if (file.open(QIODevice::WriteOnly)) {
//            file.write(reply->readAll());
//            file.close();
//        }
//    }
//    reply->deleteLater();
//}

QString loader::fetchWork(QWidget *parent, QUrl UserLink)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(UserLink);
    QNetworkReply* reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QString Result = reply->readAll();
    reply->deleteLater();
    QRegularExpression re("\"(.*\\.mview\\?\\d+)\"");
    QRegularExpressionMatch match = re.match(Result);
    if (match.hasMatch()) {
        qDebug() << match.captured(1);
        return match.captured(1);
    }
    else {
        QMessageBox::critical(parent, "Ошибка поиска данных!", "Эта ссылка не поддерживается для конвертации.\nВыберите другую работу, где встроен Marmoset Viewer! или имеется Sketchfab View.");
        qDebug() << "Cannot load this work! Return void";
        return QString();
    }
}

//QUrl loader::ExtractMviewLink(QString Target)
//{
//    QRegularExpression re("\"(.*\\.mview\\?\\d+)\"");
//    QRegularExpressionMatch match = re.match(Target);
//    if (match.hasMatch()) {
//        return match.captured(1);
//    }
//    else {
//        return QUrl();
//    }
//}

