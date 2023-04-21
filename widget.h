
#ifndef WIDGET_H
#define WIDGET_H
#include <QFileDialog>
#include <QFileInfo>
#include <QWidget>



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_browse_files_btn_clicked();

    void on_path_to_folder_le_textChanged(const QString &arg1);

    void on_about_programm_btn_clicked();

private:
    Ui::Widget *ui;
    void Download(QUrl LinkToFile, QString PathToFile);
};

#endif // WIDGET_H
