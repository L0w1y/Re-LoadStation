#include <QFile>
#include <QDir>
#include <QDataStream>
#ifndef EXTRACTOR_NEW_H
#define EXTRACTOR_NEW_H

class extractor_new
{
public:
    extractor_new();
    void extract(QString filename);

private:
    void decompress(QByteArray &data, QByteArray &output, quint32 size);
    void mkDIR(const QString &dir);
    QString readcstr(QFile &file);
    quint32 readuint32(QFile &file);
};

#endif // EXTRACTOR_NEW_H
