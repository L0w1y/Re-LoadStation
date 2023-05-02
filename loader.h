#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QUrl>
#include <QWidget>

class loader
{
public:
    loader();
//    void Download(QUrl downloadUrl, QString downloadPath);
    QString fetchWork(QWidget *parent, QUrl UserLink);
//    QUrl ExtractMviewLink(QString Target);

private:
};

#endif // LOADER_H
