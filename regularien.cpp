#include "regularien.h"
#include "ui_regularien.h"

Regularien::Regularien(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Regularien)
{
    ui->setupUi(this);
    ui->textBrowserRegularien->setOpenLinks(false);
    ui->textBrowserRegularien->setSource(QUrl(QLatin1String("qrc:/pordBund.html")));
}

Regularien::~Regularien()
{
    delete ui;
}
