#include "lizenz.h"
#include "ui_lizenz.h"


Lizenz::Lizenz(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Lizenz)
{
    ui->setupUi(this);
    ui->textBrowserLicense->setSource(QUrl(QLatin1String("qrc:/lgpl3.html")));
}

Lizenz::~Lizenz()
{
    delete ui;
}
