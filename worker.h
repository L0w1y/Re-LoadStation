#include <QFile>
#include <QDir>
#include <QWidget>
#include <converter.h>
#ifndef WORKER_H
#define WORKER_H




class worker
{
private:
    QString filename;
    bool fileExists;
    bool fileIsConverted;
    bool doDeleteTempFiles;

public:
    worker(QWidget* parent, QFile input);
    void convertCurrent();
    void extractCurrent();
    QString getCurrent();
};

#endif // WORKER_H
