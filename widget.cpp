#include "widget.h"
#include "converter.h"
#include "ui_widget.h"
#include <extractor.h>
#include <QMessageBox>
#include <iostream>
#include <QDebug>

QStringList ListToExtract;

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
    ListToExtract = Files;
    QString absolPath = QFileInfo(Files[0]).absoluteDir().path();
    if (QFileInfo::exists(absolPath) and QFileInfo(absolPath).isDir()) {
        ui->path_to_folder_le->setText(QFileInfo(Files[0]).absoluteDir().path());
        for (QString file : Files) {
            ui->file_listwidget->addItem(file);
        }
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
    if (ui->path_to_folder_le->text() == QString()) {
        ui->file_listwidget->clear();
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
    extractor *E = new extractor();
    for (QString file : ListToExtract) {
        qDebug() << QString("Going to extract %1 file!").arg(file);
        E->extract(file);
    }
    converter *C = new converter();
    for (QString file : ListToExtract) {
        qDebug() << QString("Extracting %1 file!").arg(file);
        C->convert(file.split('.').first());
    }
    delete E;
    delete C;
}

