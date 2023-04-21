#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QUrl>

class loader
{
public:
    loader();
    void Download(QUrl downloadUrl, QString downloadPath);

private:
    QString fetchWork(QUrl UserLink);
    QUrl ExtractMviewLink(QString Target);
};

#endif // LOADER_H
