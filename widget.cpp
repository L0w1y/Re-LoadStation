#include "widget.h"
#include "ui_widget.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_browse_files_btn_clicked()
{
    QStringList Files(QFileDialog::getOpenFileNames(this, "Выберите один или несколько .mview файлов!", QString(), QString("*.mview")));
    QString absolPath = QFileInfo(Files[0]).absoluteDir().path();
    if (QFileInfo(absolPath).exists() and QFileInfo(absolPath).isDir()) {
        ui->path_to_folder_le->setText(QFileInfo(Files[0]).absoluteDir().path());
    }
}

void Widget::on_path_to_folder_le_textChanged(const QString &arg1)
{
    QFileInfo path(ui->path_to_folder_le->text());
    QDir dir;
    QStringList filesInDir;
    if (path.isDir() and path.exists()) {
        dir = (ui->path_to_folder_le->text());
        filesInDir = dir.entryList(QStringList() << "*.mview", dir.Files);
        ui->file_listwidget->clear();
        for (auto file : filesInDir) {
            ui->file_listwidget->addItem(QFileInfo(file).fileName());
        }
//        qDebug() << "Mview files in dir:" << filesInDir;
    }
    if (ui->path_to_folder_le->text() == QString()) {
        ui->file_listwidget->clear();
    }
    if (path.isDir() and path.exists() and (filesInDir.length() > 0)) {
        ui->information_label->setText("*Если указать путь - программа конвертирует все файлы в папке.");
    }
    else {
        ui->information_label->setText("Введите путь до папки, либо выберите файлы вручную.");
    }
}

void Widget::Download(QUrl LinkToFile, QString PathToFile)
{
    if (PathToFile.isEmpty() or LinkToFile.isEmpty()) {
        return;
    }
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(LinkToFile));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        QFile file(PathToFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
        }
    }
}


void Widget::on_about_programm_btn_clicked()
{
    QMessageBox::about(this, "LoadStation - About us", "<h4>Developed by Sergey Lowly</h4>\n\n"
                                                       "Copyright 2022-2023 Lowlydevelopment.are. "
                                                       "For technical support, write to <a href=\"about.lowly@vk.com\">Email</a>\n"
                                                       "<a href=\"http://www.such-and-such.com\">WebPage</a>");
}

