#include "ihk.h"
#include "ui_ihk.h"

Ihk::Ihk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ihk)
{
    ui->setupUi(this);
}

void Ihk::fillIhk(qint32 t1){
    // Teil 1
    ui->label_17->setText(QString::number(t1) + " x 20 =");
    ui->label_16->setText(QString::number(t1*20));
    ui->label_19->setText(QString::number(t1*20) + " : 20 =");
    ui->label_2->setText(QString::number(t1));
    ui->label_20->setText(getGrade(t1));
    ui->label_120->setText(QString::number(t1*20) );

}

// transforms points to grades
// duplicate code with MainWindow::getGrade // dirty !
QString Ihk::getGrade(qint32 points){

    QString grade = "6";
    if(points>=30){
        grade = "5";
    }
    if(points>=50){
        grade = "4";
    }
    if(points>=67){
        grade = "3";
    }
    if(points>=81){
        grade = "2";
    }
    if(points>=92){
        grade = "1";
    }
    return grade;
}

Ihk::~Ihk()
{
    delete ui;
}
