#include "worker.h"
#include <QMessageBox>


worker::worker(QWidget* parent, QFile input)
{
    if (!(input.isReadable() or input.exists())) {
        QMessageBox::critical(parent, "Ощибка!", "Ощибка! Файл который вы пытаетесь открыть - недоступен.");
        return;
    }
    else {
        worker::filename = input.fileName();
        worker::fileExists = input.exists();
        worker::doDeleteTempFiles = false;
    }
}

QString worker::getCurrent() {
    return worker::filename;
}

void worker::convertCurrent() {

}

void worker::extractCurrent() {

}
