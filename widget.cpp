#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <iostream>
#include <QDebug>

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
    if (QFileInfo::exists(absolPath) and QFileInfo(absolPath).isDir()) {
        ui->path_to_folder_le->setText(QFileInfo(Files[0]).absoluteDir().path());
        if (ui->file_listwidget->count() > 0 ){
            ui->call_worker_btn->setEnabled(true);
        }
        else {
            ui->call_worker_btn->setEnabled(false);
        }
        }

}

void Widget::on_path_to_folder_le_textChanged()
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

void Widget::on_about_programm_btn_clicked()
{
    QMessageBox::about(this, "LoadStation - About us", "<h4>LoadStation - Версия 0.134.12</h4>\n"
                                                       "<p>Программа предоставляет доступ к некоторым 3д моделям ArtStation</p>\n"
                                                       "<p>Для технической поддержки, рекомендуем обратиться к <a href=\"https://vk.com/Lowly_on\">основателю проекта</a>.</p>\n\n"
                                                       "Все права защищены и пренадлежат их правообладателям.");
}


void Widget::on_artstation_load_btn_clicked()
{
    ui->file_listwidget->clear();
    ui->path_to_folder_le->clear();
//    loader loader;
//    qDebug() << loader.fetchWork(this, QUrl("https://www.artstation.com/artwork/RY2KdE"));

}


void Widget::on_call_worker_btn_clicked()
{
    qDebug() << "Вы нажали кнопку!!!";
    for (int i = 0; i < ui->file_listwidget->count(); i++) {
        qDebug() << ui->file_listwidget->takeItem(i)->text();
    }
}

