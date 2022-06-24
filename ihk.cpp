#include "ihk.h"
#include "ui_ihk.h"

Ihk::Ihk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ihk)
{
    ui->setupUi(this);
}

void Ihk::fillIhk(qint32 t1, qint32 t21, qint32 t22, qint32 t23, qint32 doku, qint32 prfg, qint32 mepr, qint32 meprnr){
    qint32 t21x=t21;
    qint32 t22x=t22;
    qint32 t23x=t23;
    // Teil 1
    ui->label_l11->setText(QString::number(t1) + " x 20 =");
    ui->label_l12->setText(QString::number(t1*20));
    ui->label_l21->setText(QString::number(t1*20) + " : 20 =");
    ui->label_l22->setText(QString::number(t1));
    ui->label_l23->setText(getGrade(t1));
    ui->label_l101->setText("( "+QString::number(t1*20) );

    // Teil 2
    ui->label_l31->setText(QString::number(t21) + " x 2 +");
    ui->label_l41->setText(QString::number(t22) + " x 2 +");
    ui->label_l51->setText(QString::number(t23) + " x 2 +");

    if(meprnr == 0){
        ui->label_l32->setText(" =");
        ui->label_l42->setText(" =");
        ui->label_l52->setText(" =");
    }
    if(meprnr == 1){
        ui->label_l32->setText(QString::number(mepr) + " =");
        ui->label_l33->setText(QString::number(t21*2+mepr) + " : 3 =");
        ui->label_l34->setText(QString::number(qRound((t21*2+mepr)/3.0)) + "  x 10 =");
        ui->label_l35->setText(QString::number(qRound((t21*2+mepr)/3.0)*10));
        ui->label_l36->setText(QString::number(qRound((t21*2+mepr)/3.0)));
        ui->label_l37->setText(getGrade(qRound((t21*2+mepr)/3.0)));
        // 2+3
        ui->label_l44->setText(QString::number(t22) + "  x 10 =");
        ui->label_l45->setText(QString::number(t22*10));
        ui->label_l46->setText(QString::number(t22));
        ui->label_l47->setText(getGrade(t22));

        ui->label_l54->setText(QString::number(t23) + "  x 10 =");
        ui->label_l55->setText(QString::number(t23*10));
        ui->label_l56->setText(QString::number(t23));
        ui->label_l57->setText(getGrade(t23));
    }
    if(meprnr == 2){
        ui->label_l42->setText(QString::number(mepr) + " =");
        ui->label_l43->setText(QString::number(t22*2+mepr) + " : 3  =");
        ui->label_l44->setText(QString::number(qRound((t22*2+mepr)/3.0)) + "  x 10 =");
        ui->label_l45->setText(QString::number(qRound((t22*2+mepr)/3.0)*10));
        ui->label_l46->setText(QString::number(qRound((t22*2+mepr)/3.0)));
        ui->label_l47->setText(getGrade(qRound((t22*2+mepr)/3.0)));
        //1+3
        ui->label_l34->setText(QString::number(t21) + "  x 10 =");
        ui->label_l35->setText(QString::number(t21*10));
        ui->label_l36->setText(QString::number(t21));
        ui->label_l37->setText(getGrade(t21));

        ui->label_l54->setText(QString::number(t23) + "  x 10 =");
        ui->label_l55->setText(QString::number(t23*10));
        ui->label_l56->setText(QString::number(t23));
        ui->label_l57->setText(getGrade(t23));
    }
    if(meprnr == 3){
        ui->label_l52->setText(QString::number(mepr) + " =");
        ui->label_l53->setText(QString::number(t23*2+mepr) + " : 3  =");
        ui->label_l54->setText(QString::number(qRound((t23*2+mepr)/3.0)) + "  x 10 =");
        ui->label_l55->setText(QString::number(qRound((t23*2+mepr)/3.0)*10));
        ui->label_l56->setText(QString::number(qRound((t23*2+mepr)/3.0)));
        ui->label_l57->setText(getGrade(qRound((t23*2+mepr)/3.0)));
        //1+2
        ui->label_l34->setText(QString::number(t21) + "  x 10 =");
        ui->label_l35->setText(QString::number(t21*10));
        ui->label_l36->setText(QString::number(t21));
        ui->label_l37->setText(getGrade(t21));

        ui->label_l44->setText(QString::number(t22) + "  x 10 =");
        ui->label_l45->setText(QString::number(t22*10));
        ui->label_l46->setText(QString::number(t22));
        ui->label_l47->setText(getGrade(t22));
    }


    ui->label_l61->setText(QString::number(doku) + " x 2 =");
    ui->label_l71->setText(QString::number(prfg) + " x 2 =");

    ui->label_l62->setText(QString::number(doku*50) );
    ui->label_l72->setText(QString::number(prfg*50) );
    ui->label_l81->setText(QString::number(doku*50+prfg*50) );
    
    ui->label_l82->setText(QString::number(qRound((doku*50+prfg*50)/100.0))+ " x 50 =");
    ui->label_l83->setText(QString::number(qRound((doku*50+prfg*50)/100.0)*50));    
    ui->label_l91->setText(QString::number(qRound((doku*50+prfg*50)/100.0)+t21x*10+t22x*10+t23x*10)); //
    ui->label_l92->setText(QString::number(qRound((((doku*50+prfg*50)/100.0)+t21x*10+t22x*10+t23x*10)/80.0)));
    ui->label_l93->setText(getGrade(qRound((((doku*50+prfg*50)/100.0)+t21x*10+t22x*10+t23x*10)/80.0)));
    
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
