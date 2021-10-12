#include "about.h"
#include "ui_about.h"
#include "mainwindow.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label_version->setText(((MainWindow*)parent)->version);
}


About::~About()
{
    delete ui;
}
