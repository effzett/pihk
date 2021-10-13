#include "about.h"
#include "ui_about.h"
#include "mainwindow.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    MainWindow  *main= ((MainWindow*)parent);
    ui->label_version->setText(main->app.versionLong);
    ui->label_author->setText(main->app.author + " (" + main->app.email+")");
}


About::~About()
{
    delete ui;
}
