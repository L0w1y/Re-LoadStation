#ifndef CONVERTER_H
#define CONVERTER_H
#include <QFile>
#include <QVector3D>
#include <QVector2D>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

class converter
{
public:
    converter();
    ~converter();
    void convert(QString folder);

private:
    void mkDIR(const QString &dir);
};

#endif // CONVERTER_H
