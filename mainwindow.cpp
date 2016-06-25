#include "mainwindow.h"
#ifdef Q_OS_OSX
#include "ui_mainwindow.h"
#else
#include "ui_mainwindowwin.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // gui independent initialization
    // app specific
    const QString appversion="2.0.1";
    const QString appdate="23.06.2016";
    const QString appname="PIHK";
    const QString appauthor="Frank Zimmermann";
    const QString appemail="fz@zenmeister.de";

    // Timer related
    isTimerStarted=false;
    timerValue=0;
    offset=0;
    const QString version =  appname + "   (V" +appversion +", vom " + appdate + ")";
    timer = new QTimer(this);


    ui->setupUi(this);

    // gui dependent initialization
#ifdef Q_OS_OSX
    // OSX---
    setWindowIcon(QIcon("pihk2.icns"));
#else
    // Windows Q_OS_WIN
    setWindowIcon(QIcon("pihk2.ico"));
#endif
    statusLabel = new QLabel(this);
    statusLabel->setText(appemail);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    ui->listViewPRFG->setStyleSheet("background-color:lightgray;");
    ui->listViewMEPR->setStyleSheet("background-color:lightgray;");

    // gui stuff
    ui->pDate->setDate(QDate::currentDate());   // set current Date
    ui->lcdNumber->setPalette(Qt::black);       // set color for LCD
    this->setWindowTitle(version);              // set title
    ui->statusBar->addPermanentWidget(statusLabel);
    makeFilename();                             // construct basic file name
    ui->labelGradeA->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeB->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeResultA->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeResultB->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeResult->setStyleSheet("QLabel { color : red; }");
    ui->spinboxGa1E->setEnabled(false);
    ui->spinboxGa1E->hide();
    ui->spinboxGa1E->setValue(0);
    ui->spinboxGa2E->setEnabled(false);
    ui->spinboxGa2E->hide();
    ui->spinboxGa2E->setValue(0);
    ui->spinboxWisoE->setEnabled(false);
    ui->spinboxWisoE->hide();
    ui->spinboxWisoE->setValue(0);
    ui->radioButton1->setEnabled(false);
    ui->radioButton2->setEnabled(false);
    ui->radioButton3->setEnabled(false);


    // Connections
    connect(timer,SIGNAL(timeout()),this,SLOT(updateProgressBar()));
    connect(ui->startTimer,SIGNAL(clicked()),this,SLOT(toggleStartStop()));
    connect(ui->resetTimer,SIGNAL(clicked()),this,SLOT(timerReset()));
    connect(ui->pname,SIGNAL(textChanged(QString)),this,SLOT(makeFilename()));
    connect(ui->pnummer,SIGNAL(textChanged(QString)),this,SLOT(makeFilename()));
    connect(ui->pDate,SIGNAL(dateChanged(QDate)),this,SLOT(makeFilename()));
    connect(ui->folder,SIGNAL(textChanged(QString)),this,SLOT(makeFilename()));
    connect(ui->spinboxDocumentation,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxExamination,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxGa1,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxGa2,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxWiso,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxGa1E,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxGa2E,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->spinboxWisoE,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
    connect(ui->radioButton1,SIGNAL(toggled(bool)),this,SLOT(writeResults()));
    connect(ui->radioButton2,SIGNAL(toggled(bool)),this,SLOT(writeResults()));
    connect(ui->radioButton3,SIGNAL(toggled(bool)),this,SLOT(writeResults()));
    connect(ui->saveFile,SIGNAL(clicked(bool)),this,SLOT(saveData()));
    connect(ui->buttonSimPRFG,SIGNAL(clicked()),this,SLOT(fillPRFG()));
    connect(ui->buttonSimMEPR,SIGNAL(clicked()),this,SLOT(fillMEPR()));
}

// each shot: increment timerValue and show in progressBar and LCD
void MainWindow::updateProgressBar(){
    timerValue++;
    if(timerValue<=15){
        ui->progressBar->setValue(timerValue);
    }
    else{
        offset=15;
        ui->lcdNumber->setPalette(Qt::red);
    }
    ui->lcdNumber->display(timerValue-offset);
}

// start or stop timer
void MainWindow::toggleStartStop(){
    if(isTimerStarted==false){
        ui->startTimer->setText("Stop");
        ui->resetTimer->hide();
        isTimerStarted = true;
        timer->start(60000);
    }
    else{
        ui->startTimer->setText("Start");
        isTimerStarted = false;
        ui->resetTimer->show();
        timer->stop();
    }
}

// resets progressBar and LCD
void MainWindow::timerReset(){
    if(isTimerStarted==false){
        timerValue=0;
        ui->progressBar->setValue(timerValue);
        offset=0;
        ui->lcdNumber->setPalette(Qt::black);
        ui->lcdNumber->display(timerValue);
    }
}

// make file name from home/folder/date/name/number.txt
void MainWindow::makeFilename(){
    QString fn="";
    QString currentdate = ui->pDate->date().toString("yyyyMMdd");
    QString currentName = ui->pname->text().replace(" ","_");
    QString currentNumber = ui->pnummer->text();

    if(ui->folder->text().isEmpty()){
        ui->path->setText(QDir::homePath() + QDir::separator() + currentdate + currentName + currentNumber + ".txt");
    }
    else{

        if(QDir(QDir::homePath() + QDir::separator()+ui->folder->text()).exists()){// folder exist
            ui->path->setText(QDir::homePath() + QDir::separator() + ui->folder->text() + QDir::separator() + currentdate + currentName + currentNumber + ".txt");
        }
        else{
            ui->path->setText(QDir::homePath() + QDir::separator() + currentdate + currentName + currentNumber + ".txt");
        }
    }
    ui->saveFile->setEnabled(true);
}

// calculates pointsA
quint32 MainWindow::calcA(qint32 docu, qint32 exam){
    quint32 pointsA=0;
    pointsA = round((docu + exam)/2.0);
    return pointsA;
}

// calculates pointsB
quint32 MainWindow::calcB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 epnr, quint32 mueergpr){
    quint32 pointsB =0;
    switch(epnr){
    case 0: // no oral
        pointsB = round((ga1*2.0+ga2*2.0+wiso)/5.0);
        break;
    case 1: // oral ga1
        pointsB = round( ( round((ga1*2.0+mueergpr)/3.0)*2.0+ga2*2.0+wiso)/5.0 );
        break;
    case 2: // oral ga2
        pointsB = round( ( round((ga2*2.0+mueergpr)/3.0)*2.0+ga1*2.0+wiso)/5.0 );
        break;
    case 3: // oral wiso
        pointsB = round( ( ga1*2.0+ga2*2.0 + round((wiso*2.0+mueergpr)/3.0))/5.0 );
        break;
    default:
        // error
        ;
    }
    return pointsB;
}

quint32 MainWindow::calcAll(quint32 pointsA, quint32 pointsB){
    quint32 pointsAll=0;
    pointsAll = round( (pointsA+pointsB)/2.0 );
    return pointsAll;
}

void MainWindow::writeResults(){
    // Part A
    quint32 docu = (quint32) ui->spinboxDocumentation->value();
    quint32 exam  = (quint32) ui->spinboxExamination->value();
    qint32 pointsA = calcA(docu,exam);
    QString gradeA = getGrade(pointsA);
    ui->labelPointsA->setText(QString::number(pointsA).rightJustified(3,' '));
    if(checkPassedA(docu,exam)){
        ui->labelGradeA->setStyleSheet("QLabel { color : green; }");
    }
    else{
        ui->labelGradeA->setStyleSheet("QLabel { color : red; }");
    }
    ui->labelGradeA->setText(getGrade(pointsA).rightJustified(12,' '));

    // Part B
    quint32 nr=0;
    quint32 points=0;
    quint32 ga1 = (quint32) ui->spinboxGa1->value();
    quint32 ga2  = (quint32) ui->spinboxGa2->value();
    quint32 wiso  = (quint32) ui->spinboxWiso->value();
//    quint32 ga1E = (quint32) ui->spinboxGa1E->value();
//    quint32 ga2E  = (quint32) ui->spinboxGa2E->value();
//    quint32 wisoE  = (quint32) ui->spinboxWisoE->value();

    if(checkMAllowed(ga1,ga2,wiso)){    // oral is possible
        if(round((round((ga1*2.0+100.0)/3.0)*2.0+ga2*2.0+wiso)/5.0)>=50 && ga1<50){ // oral in ga1
            ui->radioButton1->setEnabled(true);
            if(ui->radioButton1->isChecked()){ // oral in ga1
                ui->spinboxGa1E->setEnabled(true);
                ui->spinboxGa1E->show();
                nr=1;
                points=ui->spinboxGa1E->value();
            }
            else{
                ui->spinboxGa1E->setValue(0);
                ui->spinboxGa1E->setEnabled(false);
                ui->spinboxGa1E->hide();
            }
        }
        else{
            ui->radioButton1->setAutoExclusive(false);
            ui->radioButton2->setAutoExclusive(false);
            ui->radioButton3->setAutoExclusive(false);
            ui->radioButton1->setChecked(false);
            ui->radioButton1->setAutoExclusive(true);
            ui->radioButton2->setAutoExclusive(true);
            ui->radioButton3->setAutoExclusive(true);
           ui->radioButton1->setEnabled(false);
           ui->spinboxGa1E->setValue(0);
           ui->spinboxGa1E->hide();
        }
        if(round((round((ga2*2.0+100.0)/3.0)*2.0+ga1*2.0+wiso)/5.0)>=50 && ga2<50){ // oral in ga2
            ui->radioButton2->setEnabled(true);
            if(ui->radioButton2->isChecked()){ // oral in ga1
                 ui->spinboxGa2E->setEnabled(true);
                 ui->spinboxGa2E->show();
                 nr=2;
                 points=ui->spinboxGa2E->value();
             }
             else{
                 ui->spinboxGa2E->setValue(0);
                 ui->spinboxGa2E->setEnabled(false);
                 ui->spinboxGa2E->hide();
             }
        }
        else{
            ui->radioButton1->setAutoExclusive(false);
            ui->radioButton2->setAutoExclusive(false);
            ui->radioButton3->setAutoExclusive(false);
            ui->radioButton2->setChecked(false);
            ui->radioButton1->setAutoExclusive(true);
            ui->radioButton2->setAutoExclusive(true);
            ui->radioButton3->setAutoExclusive(true);
            ui->radioButton2->setEnabled(false);
            ui->spinboxGa2E->setValue(0);
            ui->spinboxGa2E->hide();
        }
        if(round((round((wiso*2.0+100.0)/3.0)+ga1*2.0+ga2*2.0)/5.0)>=50 && wiso<50){ // oral in wiso
            ui->radioButton3->setEnabled(true);
            if(ui->radioButton3->isChecked()){ // oral in ga1
                ui->spinboxWisoE->setEnabled(true);
                ui->spinboxWisoE->show();
                nr=3;
                points=ui->spinboxWisoE->value();
            }
            else{
                ui->spinboxWisoE->setValue(0);
                ui->spinboxWisoE->setEnabled(false);
                ui->spinboxWisoE->hide();
            }

        }
        else{
            ui->radioButton1->setAutoExclusive(false);
            ui->radioButton2->setAutoExclusive(false);
            ui->radioButton3->setAutoExclusive(false);
            ui->radioButton3->setChecked(false);
            ui->radioButton1->setAutoExclusive(true);
            ui->radioButton2->setAutoExclusive(true);
            ui->radioButton3->setAutoExclusive(true);
            ui->radioButton3->setChecked(false);
            ui->radioButton3->setEnabled(false);
            ui->spinboxWisoE->setValue(0);
            ui->spinboxWisoE->hide();
        }
    }
    else{   // oral not possible
        ui->radioButton1->setAutoExclusive(false);
        ui->radioButton2->setAutoExclusive(false);
        ui->radioButton3->setAutoExclusive(false);
        ui->radioButton1->setChecked(false);
        ui->radioButton1->setEnabled(false);
        ui->radioButton2->setChecked(false);
        ui->radioButton2->setEnabled(false);
        ui->radioButton3->setChecked(false);
        ui->radioButton3->setEnabled(false);
        ui->radioButton1->setAutoExclusive(true);
        ui->radioButton2->setAutoExclusive(true);
        ui->radioButton3->setAutoExclusive(true);
        ui->spinboxGa1E->setValue(0);
        ui->spinboxGa1E->hide();
        ui->spinboxGa2E->setValue(0);
        ui->spinboxGa2E->hide();
        ui->spinboxWisoE->setValue(0);
        ui->spinboxWisoE->hide();
    }
    if(checkPassedB(ga1,ga2,wiso,nr,points)){
        ui->labelGradeB->setStyleSheet("QLabel { color : green; }");
    }
    else{
        ui->labelGradeB->setStyleSheet("QLabel { color : red; }");
    }

    qint32 pointsB = calcB(ga1,ga2,wiso,nr,points);
    QString gradeB = getGrade(pointsB);
    ui->labelPointsB->setText(QString::number(pointsB).rightJustified(3,' '));

    ui->labelGradeB->setText(getGrade(pointsB).rightJustified(12,' '));


    // Results
    // Part A
    ui->labelResultA->setText(QString::number(pointsA).rightJustified(3,' '));
    if(checkPassedA(docu,exam)){
        ui->labelGradeResultA->setStyleSheet("QLabel { color : green; }");
    }
    else{
        ui->labelGradeResultA->setStyleSheet("QLabel { color : red; }");
    }
    ui->labelGradeResultA->setText(getGrade(pointsA).rightJustified(12,' '));

    // Part B
    ui->labelResultB->setText(QString::number(pointsB).rightJustified(3,' '));
    if(checkPassedB(ga1,ga2,wiso,nr,points)){
        ui->labelGradeResultB->setStyleSheet("QLabel { color : green; }");
    }
    else{
        ui->labelGradeResultB->setStyleSheet("QLabel { color : red; }");
    }
    ui->labelGradeResultB->setText(getGrade(pointsB).rightJustified(12,' '));

    // All
    quint32 pointsAll = calcAll(pointsA,pointsB);
    QString gradeAll = getGrade(pointsAll);
    ui->labelResultAll->setText(QString::number(pointsAll).rightJustified(3,' '));
    if(checkPassedB(ga1,ga2,wiso,nr,points) && checkPassedA(docu,exam)){
        ui->labelGradeResult->setStyleSheet("QLabel { color : green; }");
        hasPassed=true;
    }
    else{
        ui->labelGradeResult->setStyleSheet("QLabel { color : red; }");
        hasPassed=false;
    }
    ui->labelGradeResult->setText(getGrade(pointsAll).rightJustified(12,' '));

    fillPRFG();
    fillMEPR();
    ui->saveFile->setEnabled(true);
}

bool MainWindow::checkPassedA(quint32 docu, quint32 exam){
    bool retVal=true;
    if(calcA(docu,exam)<50 || docu<30 || exam<30){
        retVal = false;
    }
    return retVal;
}

bool MainWindow::checkPassedB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 nr, quint32 points){
    bool retVal=true;
    switch(nr){
    case 0:
        if(calcB(ga1,ga2,wiso)<50 || ga1<30 || ga2<30 || wiso<30){
            retVal = false;
        }
        break;
    case 1:
        if(calcB(ga1,ga2,wiso,nr,points)<50 || round((2.0*ga1+points)/3.0)<30 || ga2<30 || wiso<30){
            retVal = false;
        }
        break;
    case 2:
        if(calcB(ga1,ga2,wiso,nr,points)<50 || round((2.0*ga2+points)/3.0)<30 || ga1<30 || wiso<30){
            retVal = false;
        }
        break;
    case 3:
        if(calcB(ga1,ga2,wiso,nr,points)<50 || round((2.0*wiso+points)/3.0)<30 || ga2<30 || ga1<30){
            retVal = false;
        }
        break;
    default:
        ;
        // error
    }
    return retVal;
}

bool MainWindow::checkMAllowed(quint32 ga1,quint32 ga2,quint32 wiso){
    bool retVal=true;
    if(ga1<50 && ga2<50 && wiso<50){    // 3  5's
        retVal=false;
    }
    if(calcB(ga1,ga2,wiso)>=50){        // too good
        retVal=false;
    }
    if(ga1<30 || ga2<30 || wiso<30){        // at least one ungenügend
        retVal=false;
    }
    if(     (round((round((ga1*2.0+100.0)/3.0)*2.0+ga2*2.0+wiso)/5.0)<50) &&
            (round((round((ga2*2.0+100.0)/3.0)*2.0+ga1*2.0+wiso)/5.0)<50) &&
            (round((round((wiso*2.0+100.0)/3.0)+ga1*2.0+ga2*2.0)/5.0)<50)){    // no possibility
        retVal=false;
    }
    return retVal;
}

void MainWindow::saveData(){
   QFile outFile(ui->path->text());
   outFile.open(QIODevice::WriteOnly|QIODevice::Text);
   QTextStream out(&outFile);
   out<<"Datum     : " + ui->pDate->text()<<endl;
   out<<"Name      : " + ui->pname->text()<<endl;
   out<<"Id-Nummer : " + ui->pnummer->text()<<endl;
   out<<"Doku      : " + ui->spinboxDocumentation->text()<<endl;
   out<<"PRFG      : " + ui->spinboxExamination->text()<<endl;
   out<<"GA1       : " + ui->spinboxGa1->text()<<endl;
   out<<"GA2       : " + ui->spinboxGa2->text()<<endl;
   out<<"Wiso      : " + ui->spinboxWiso->text()<<endl;
   out<<"MEP-GA1   : " + ui->spinboxGa1E->text()<<endl;
   out<<"MEP-GA2   : " + ui->spinboxGa2E->text()<<endl;
   out<<"MEP-WISO  : " + ui->spinboxWisoE->text()<<endl;
   out<<"Ergebnis A: " + ui->labelResultA->text()+" ("+ui->labelGradeResultA->text()+")"<<endl;
   out<<"Ergebnis B: " + ui->labelResultB->text()+" ("+ui->labelGradeResultB->text()+")"<<endl;
   out<<"Ergebnis  : " + ui->labelResultAll->text()+" ("+ui->labelGradeResult->text()+")"<<endl<<endl;

   if(hasPassed){
       out<<"---BESTANDEN---"<<endl;
   }
   else{
       out<<"---NICHT bestanden---"<<endl;
   }
   outFile.close();
   ui->saveFile->setEnabled(false);
}

void MainWindow::fillPRFG(){
    // Create model
        model = new QStringListModel(this);

        // Make data
        QStringList List;
        quint32 docu=ui->spinboxDocumentation->value();
        quint32 resultB=(quint32)ui->labelPointsB->text().toInt();
        quint32 ga1 = ui->spinboxGa1->value();
        quint32 ga2 = ui->spinboxGa2->value();
        quint32 wiso = ui->spinboxWiso->value();
        QString atmp="";
        QString btmp="";
        QString gtmp="";
        quint32 b = resultB;
        QString bString = getGrade(b);
        quint32 nr=0;
        quint32 points=0;
        if(ui->radioButton1->isChecked()){
            nr=1;
            points=ui->spinboxGa1E->value();
        }
        if(ui->radioButton2->isChecked()){
            nr=2;
            points=ui->spinboxGa2E->value();
        }
        if(ui->radioButton3->isChecked()){
            nr=3;
            points=ui->spinboxWisoE->value();
        }
        bool passedtmp=false;
        bool passed=false;

        for(int i=0;i<=100;i++){
            quint32 a = round((i+docu)/2.0);
            quint32 g = round((a+resultB)/2.0);
            QString aString = getGrade(a);
            QString gString = getGrade(g);
            passed=(checkPassedA(docu,i) && checkPassedB(ga1,ga2,wiso,nr,points));
            if(atmp.compare(aString)!=0 || btmp.compare(bString)!=0 || gtmp.compare(gString)!=0 || (passed!=passedtmp)){
                QString item;
                if(checkPassedA(docu,i) && checkPassedB(ga1,ga2,wiso,nr,points)){
                    item = QString("%1: A=%2 B=%3 +G=%4").arg(i,3).arg(aString,-12).arg(bString,-12).arg(gString,-12);
                }else
                {
                    item = QString("%1: A=%2 B=%3 -G=%4").arg(i,3).arg(aString,-12).arg(bString,-12).arg(gString,-12);
                }
                List << item;
                atmp=aString;
                btmp=bString;
                gtmp=gString;
                passedtmp = passed;
            }
        }


        // Populate our model
        model->setStringList(List);

        // Glue model and view together
#ifdef Q_OS_OSX
        // OSX---
        QFont newFont("Courier", 12, QFont::Normal, true);
#else
        // Windows Q_OS_WIN
        QFont newFont("Courier", 10, QFont::Normal, true);
#endif
        ui->listViewPRFG->setStyleSheet("background-color:lightgray;");
        ui->listViewPRFG->setFont(newFont);
        ui->listViewPRFG->setModel(model);
}

void MainWindow::fillMEPR(){
    quint32 ga1=ui->spinboxGa1->value();
    quint32 ga2=ui->spinboxGa2->value();
    quint32 wiso=ui->spinboxWiso->value();

    // Create model
    model = new QStringListModel(this);

    // Make data
    QStringList List;
    quint32 nr=0;
    if(ui->radioButton1->isChecked()){
        nr=1;
    }
    if(ui->radioButton2->isChecked()){
        nr=2;
    }
    if(ui->radioButton3->isChecked()){
        nr=3;
    }

    if(checkMAllowed(ga1,ga2,wiso) && nr>0){
        quint32 docu=ui->spinboxDocumentation->value();
        quint32 exam=ui->spinboxExamination->value();
        quint32 a = (quint32) ui->labelPointsA->text().toInt();
        QString atmp="";
        QString btmp="";
        QString gtmp="";
        bool passedtmp=false;
        bool passed=false;
        for(int i=0;i<=100;i++){
            quint32 b=0;
            quint32 points=i;
            switch(nr){
            case 1:
                b = round((round((ga1*2.0+i)/3.0)*2.0 + ga2*2.0 + wiso)/5.0);
                break;
            case 2:
                b = round((round((ga2*2.0+i)/3.0)*2.0 + ga1*2.0 + wiso)/5.0);
                break;
            case 3:
                b = round((round((wiso*2.0+i)/3.0) + ga1*2.0 + ga2*2.0)/5.0);
                break;
            default:
                ; // error
            }
            quint32 g = round((a+b)/2.0);
            QString aString = getGrade(a);
            QString bString = getGrade(b);
            QString gString = getGrade(g);
            passed=(checkPassedA(docu,exam) && checkPassedB(ga1,ga2,wiso,nr,points));
            if(atmp.compare(aString)!=0 || btmp.compare(bString)!=0 || gtmp.compare(gString)!=0 || (passed!=passedtmp)){
                QString item;
                if(passed){
                    item = QString("%1: A=%2 B=%3 +G=%4").arg(i,3).arg(aString,-12).arg(bString,-12).arg(gString,-12);
                }
                else{
                    item = QString("%1: A=%2 B=%3 -G=%4").arg(i,3).arg(aString,-12).arg(bString,-12).arg(gString,-12);
                }
                List << item;
                atmp=aString;
                btmp=bString;
                gtmp=gString;
                passedtmp=passed;
            }
        }

    }
    else{
        List.clear();
    }

        // Populate our model
    model->setStringList(List);

    // Glue model and view together
#ifdef Q_OS_OSX
        // OSX---
        QFont newFont("Courier", 12, QFont::Normal, true);
#else
        // Windows Q_OS_WIN
        QFont newFont("Courier", 10, QFont::Normal, true);
#endif
    ui->listViewMEPR->setStyleSheet("background-color:lightgray;");
    ui->listViewMEPR->setFont(newFont);
    ui->listViewMEPR->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// transforms points to grades
QString MainWindow::getGrade(qint32 points){
    QString grade = "ungenügend";
    if(points>=30){
        grade="mangelhaft";
    }
    if(points>=50){
        grade="ausreichend";
    }
    if(points>=67){
        grade="befriedigend";
    }
    if(points>=81){
        grade="gut";
    }
    if(points>=92){
        grade="sehr gut";
    }
    return grade;
}
