#include "ihk.h"
#include "ui_ihk.h"

Ihk::Ihk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ihk)
{
    ui->setupUi(this);
}

void Ihk::fillIhk(qint32 t1, qint32 t21, qint32 t22, qint32 t23, qint32 doku, qint32 prfg){
    // Teil 1
    ui->label_l11->setText(QString::number(t1) + " x 20 =");
    ui->label_l12->setText(QString::number(t1*20));
    ui->label_l21->setText(QString::number(t1*20) + " : 20 =");
    ui->label_l22->setText(QString::number(t1));
    ui->label_l23->setText(getGrade(t1));
    ui->label_l101->setText(QString::number(t1*20) );

    // Teil 2
    ui->label_l31->setText(QString::number(t21) + " x 2 +");
    ui->label_l41->setText(QString::number(t22) + " x 2 +");
    ui->label_l51->setText(QString::number(t23) + " x 2 +");

}

// transforms points to grades
// Attention!: duplicate code with MainWindow::getGrade dirty ! perhaps with static function?
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
