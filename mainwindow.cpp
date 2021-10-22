#include "mainwindow.h"
#include "about.h"
#include "preferences.h"
#include "lizenz.h"
#include "regularien.h"
#include "treemodel.h"
#include <mypihk.h>

#ifdef Q_OS_OSX
#include "ui_mainwindow.h"
#elif defined(Q_OS_WIN)
#include "ui_mainwindowwin.h"
#else
#error  "Is not supported!"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // gui independent initialization
    // app specific
    app.version="3.0.0";
    app.date=QDate::currentDate().toString("dd.MM.yyyy");
    // oder QDate::currentDate().toString("dd.MM.yyyy");
    app.name="PIHK";
    app.author="Frank Zimmermann";
    app.email="fz@zenmeister.de";
    app.versionLong = app.name + "   (V" +app.version +", vom " + app.date + ")";

    // Timer related
    isTimerStarted=false;
    timerValue=0;
    offset=0;
    timer = new QTimer(this);
    maxMinutes=15;

    QCoreApplication::setOrganizationName("zenmeister");
    QCoreApplication::setOrganizationDomain("zenmeister.de");
    QCoreApplication::setApplicationName("PIHK");
    QSettings settings;

    ui->setupUi(this);

    // Read in Tester model
    const QStringList headers({tr("Prüfer"),tr("K1"),tr("K2"),tr("Anw")});
    QFile file(":/tree.txt");
    file.open(QIODevice::ReadOnly);
    treeModel = new TreeModel(headers, file.readAll());
    file.close();

    // gui dependent initialization
#ifdef Q_OS_OSX
    // OSX---
    setWindowIcon(QIcon("myLogoPIHK3a.icns"));
#else
    // Windows Q_OS_WIN
    setWindowIcon(QIcon("myLogoPIHK3a.ico"));
#endif

    // Initialization
    // Basic
    // Setting statusbar and fix geometry
    QString sp= "          ";
    QString grades = "0-29=ungenügend,"+sp+"30-49=mangelhaft,"+sp+"50-66=ausreichend,"+sp+"67-80=befriedigend,"+sp+"81-91=gut,"+sp+"92-100=sehr gut";
    statusLabel = new QLabel(grades);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { color : darkgray; }");
    ui->statusBar->insertPermanentWidget(0,statusLabel,1);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    this->setWindowTitle(app.versionLong);              // set title

    // gui stuff (new candidate)
    ui->lcdNumber->setPalette(Qt::black);       // set color for LCD
    ui->pDate->setDate(QDate::currentDate());   // set current Date
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
    ui->folder->setPlaceholderText(QDir::homePath());
    ui->folder->setText(QDir::homePath());


    // Table View
    // tableWidget initialisieren
    ui->tableView->setModel(treeModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHeaderView* header=ui->tableView->verticalHeader();
    header->setDefaultSectionSize(20); // 20 px height
    header->sectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0,260);
    ui->tableView->setColumnWidth(1,40);
    ui->tableView->setColumnWidth(2,40);
    ui->tableView->setColumnWidth(3,40);
    ui->tableView->horizontalScrollBar()->setDisabled(true);
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(0,QHeaderView::Fixed);
    headerView->setSectionResizeMode(1,QHeaderView::Fixed);
    headerView->setSectionResizeMode(2,QHeaderView::Fixed);
    headerView->setSectionResizeMode(3,QHeaderView::Fixed);
//    QSortFilterProxyModel proxyModel;
//    proxyModel.setSourceModel( treeModel );
//    ui->tableView->setModel( &proxyModel );

    ui->comboBoxExam->setModel(treeModel);
    ui->comboBoxExam_2->setModel(treeModel);

    emit ui->comboBoxExam->currentIndexChanged(0);
    emit ui->comboBoxExam_2->currentIndexChanged(0);

    mypref = new Prefs(maxMinutes,DATUM,MINUS,NAME,MINUS,NUMMER,UNDERSCORE);

    fileName = makeFilename();                             // construct basic file name

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
    connect(ui->buttonSimPRFG,SIGNAL(clicked()),this,SLOT(fillPRFG()));
    connect(ui->buttonSimMEPR,SIGNAL(clicked()),this,SLOT(fillMEPR()));
    connect(ui->listViewPRFG,SIGNAL(clicked(const QModelIndex &)),this,SLOT(setPointsPRFG(const QModelIndex &)));
    connect(ui->listViewMEPR,SIGNAL(clicked(const QModelIndex &)),this,SLOT(setPointsMEPR(const QModelIndex &)));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
}

// Timer and progressbar stuff
// each shot: increment timerValue and show in progressBar and LCD
void MainWindow::updateProgressBar(){
    timerValue++;
    if(timerValue<=maxMinutes){
        offset=0;
        ui->progressBar->setValue(timerValue);
    }
    else{
        offset=maxMinutes;
        ui->lcdNumber->setSegmentStyle(QLCDNumber::Filled);
        ui->lcdNumber->setPalette(QColor(255,165,0,255));
        //ui->lcdNumber->setPalette(Qt::red);
    }
    ui->lcdNumber->display(timerValue-offset);
    qDebug()<<timerValue;
    qDebug()<<offset;
    qDebug()<<maxMinutes;
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
//        ui->listViewPRFG->setStyleSheet("background-color:lightgray;");
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
//    ui->listViewMEPR->setStyleSheet("background-color:lightgray;");
    ui->listViewMEPR->setFont(newFont);
    ui->listViewMEPR->setModel(model);
}

void MainWindow::setPointsPRFG(const QModelIndex &index){
    if(index.isValid()){
        QString exam = index.data().toString();
        QString subString = exam.mid(0,3);
        ui->spinboxExamination->setValue(subString.toDouble());
    }
}

void MainWindow::setPointsMEPR(const QModelIndex &index){
    if(index.isValid()){
        QString mepr = index.data().toString();
        QString subString = mepr.mid(0,3);
        if(ui->radioButton1->isChecked()){
            ui->spinboxGa1E->setValue(subString.toDouble());
        }
        if(ui->radioButton2->isChecked()){
            ui->spinboxGa2E->setValue(subString.toDouble());
        }
        if(ui->radioButton3->isChecked()){
            ui->spinboxWisoE->setValue(subString.toDouble());
        }
    }
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


void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

QJsonObject MainWindow::packQJD(){
    QJsonObject json;
    QJsonArray anwesend;

    json["PIHKVersion"] = app.version;
    json["Fachrichtung"] = ui->comboBoxExam->currentText();
    json["Ausschuss"] = ui->comboBoxExam_2->currentText();
    json["Datum"] = ui->pDate->text();
    json["Name"] = ui->pname->text();
    json["Id-Nummer"] = ui->pnummer->text();
    json["Doku"] = ui->spinboxDocumentation->text();
    json["PRFG"] = ui->spinboxExamination->text();
    json["GA0"] = ui->spinboxGa0->text();
    json["GA1"] = ui->spinboxGa1->text();
    json["GA2"] = ui->spinboxGa2->text();
    json["Wiso"] = ui->spinboxWiso->text();
    json["MEP-GA1"] = ui->spinboxGa1E->text();
    json["MEP-GA2"] = ui->spinboxGa2E->text();
    json["MEP-WISO"] = ui->spinboxWisoE->text();
    json["Ergebnis A"] = ui->labelResultA->text()+" ("+ui->labelGradeResultA->text()+")";
    json["Ergebnis B"] = ui->labelResultB->text()+" ("+ui->labelGradeResultB->text()+")";
    json["Ergebnis"] = ui->labelResultAll->text()+" ("+ui->labelGradeResult->text()+")";
    json["Prüfungsergebnis"] = (hasPassed)?"BESTANDEN":"NICHT bestanden";
    json["Prüfungszeit"] = ui->lcdNumber->value();

    QModelIndex parent = ui->comboBoxExam_2->rootModelIndex();
    qint32 i = ui->comboBoxExam_2->currentIndex();
    QModelIndex start = ui->tableView->model()->index(i,0,parent);
    for( int row = 0; row < ui->tableView->model()->rowCount(start); ++row ) {
        QString name = QVariant(ui->tableView->model()->index(row,0,start).data(Qt::ItemIsEditable)).toString();
        for ( int col = 1; col < ui->tableView->model()->columnCount(start); ++col ) {
            if(ui->tableView->model()->index(row,col,start).data(Qt::CheckStateRole).toUInt()>0){
                switch(col){
                case 1: // 1.Korr
                    json.insert( "1.Korr",name);
                    break;
                case 2: // 2.Korr
                    json.insert( "2.Korr",name);
                    break;
                case 3: // Anwesend
                    anwesend.append(name);
                default:
                    break;
                }
            }
        }
    }
    json.insert("Anwesend",anwesend);
    
    return json;
}

void MainWindow::unpackQJO(QJsonObject json ){
    app.version =  json["PIHKVersion"].toString();
    ui->comboBoxExam->currentText() = json["Fachrichtung"].toString();
    ui->comboBoxExam_2->currentText() = json["Ausschuss"].toString();
    ui->pDate->setDate(QDate::fromString(json.value("Datum").toString(),"dd.MM.yyyy"));
    ui->pname->setText(json.value("Name").toString());
    ui->pnummer->setText(json.value("Id-Nummer").toString());
    ui->spinboxDocumentation->setValue(json.value("Doku").toString().toInt());
    ui->spinboxExamination->setValue(json.value("PRFG").toString().toInt());
    ui->spinboxGa0->setValue(json.value("GA0").toString().toInt()); 
    ui->spinboxGa1->setValue(json.value("GA1").toString().toInt());
    ui->spinboxGa2->setValue(json.value("GA2").toString().toInt());
    ui->spinboxWiso->setValue(json.value("Wiso").toString().toInt());
    ui->spinboxGa1E->setValue(json.value("MEP-GA1").toString().toInt());
    ui->spinboxGa2E->setValue(json.value("MEP-GA2").toString().toInt());
    ui->spinboxWisoE->setValue(json.value("MEP-WISO").toString().toInt());
    ui->comboBoxExam->setCurrentText(json.value("Pruefung").toString());
    ui->lcdNumber->display((qint32)json["Prüfungszeit"].toInteger());

    // Wird automatisch ermittelt:
    //    json["Ergebnis A"] = ui->labelResultA->text()+" ("+ui->labelGradeResultA->text()+")";
    //    json["Ergebnis B"] = ui->labelResultB->text()+" ("+ui->labelGradeResultB->text()+")";
    //    json["Ergebnis"]   = ui->labelResultAll->text()+" ("+ui->labelGradeResult->text()+")";
}

QJsonObject MainWindow::loadJson(QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    QJsonDocument doc =  QJsonDocument().fromJson(jsonFile.readAll());
    return doc.object();
}

void MainWindow::saveJson(QJsonObject json, QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(QJsonDocument(json).toJson());
}


void MainWindow::on_actionOeffnen_triggered()
{
    QString f = QFileDialog::getOpenFileName(this,tr("Öffnen"),".",tr("JSON (*.json)"));
    if(f.length() != 0){
        QJsonObject json = loadJson(f);
        unpackQJO(json);
    }
}


void MainWindow::on_actionSichernAls_triggered()
{
    if(checkModel()){
        QString f = QFileDialog::getSaveFileName(this,tr("Sichern"),".",tr("JSON (*.json)"));
        if(f.length() != 0){
            QJsonObject json = packQJD();
            saveJson(json,f);
        }
    }
}


void MainWindow::on_pushButton_DeleteAll_clicked()
{
    ui->pname->setText("");
    ui->pnummer->setText("");
    ui->spinboxDocumentation->setValue(0);
    ui->spinboxExamination->setValue(0);
    ui->spinboxGa0->setValue(0);
    ui->spinboxGa1->setValue(0);
    ui->spinboxGa2->setValue(0);
    ui->spinboxWiso->setValue(0);
    ui->spinboxGa1E->setValue(0);
    ui->spinboxGa2E->setValue(0);
    ui->spinboxWisoE->setValue(0);
    ui->saveFile->setEnabled(true);
    ui->lcdNumber->display(0);
    if(isTimerStarted){
        toggleStartStop();
        timerReset();
    }
    clearModelCheckboxes(false);
    // model checkboxen löschen
}


void MainWindow::on_actionPreferences_triggered()
{
    Preferences *pp = new Preferences(this);
    pp->exec();
    fileName = makeFilename();
    makeNewTimer();
}


void MainWindow::on_pushButton_SelectDir_clicked()
{
        QString folder = QFileDialog::getExistingDirectory(this,tr("Default Ordner"),QDir::homePath(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
        if(folder.length() != 0){
            ui->folder->setText(folder);
        }
}

void MainWindow::about(){
    About *ap = new About(this);
    ap->show();
}


void MainWindow::on_actionLizenz_triggered(){
    Lizenz *lp = new Lizenz(this);
    lp->show();
}


void MainWindow::on_actionRegularien_triggered(){
    Regularien *rp = new Regularien(this);
    rp->show();

}


void MainWindow::on_comboBoxExam_currentIndexChanged(int index)
{
    QModelIndex midx = ui->comboBoxExam->model()->index(index,0);
    ui->comboBoxExam_2->setRootModelIndex(midx);
    ui->comboBoxExam_2->setCurrentIndex(0);
}


void MainWindow::on_comboBoxExam_2_currentIndexChanged(int index)
{
    QModelIndex midx = ui->comboBoxExam_2->model()->index(index,0);
    ui->tableView->setRootIndex(treeModel->index(midx.row(),midx.column(),ui->comboBoxExam_2->rootModelIndex()));
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if(!index.isValid()){
        return;
    }
    
    if(index.column() == 0){
        return;
    }
    
    if(index.data(Qt::CheckStateRole)==Qt::Checked){
        treeModel->setData(index,Qt::Checked,Qt::CheckStateRole); 
    }
    else{
        treeModel->setData(index,Qt::Unchecked,Qt::CheckStateRole);
    } 
    ui->saveFile->setEnabled(true);
}

QString MainWindow::makeFilePart(qint32 index, QString filler){
    QString retVal = "";
    switch(index){
    case DATUM: retVal = ui->pDate->date().toString("yyyyMMdd");break;
    case NAME: retVal = ui->pname->text().replace(" ",filler);break;
    case NUMMER: retVal = ui->pnummer->text();break;
    case FACHRICHTUNG: retVal = ui->comboBoxExam->currentText().replace(" ",filler);break;
    case AUSSCHUSS: retVal = ui->comboBoxExam_2->currentText().replace(" ",filler);break;
    case LEER: retVal = "" ;break;
    }
    return retVal;
}

QString MainWindow::makeFileDelim(qint32 index){
    QString retVal = "";
    switch(index){
    case MINUS: retVal = "-";break;
    case PLUS: retVal = "+" ;break;
    case PUNKT: retVal = "." ;break;
    case UNTERSTRICH: retVal = "_" ;break;
    case EMPTY: retVal = "";break;
    }
    return retVal;
}

// make file name from selected categories TODO
QString MainWindow::makeFilename(){
    QString fnd1="";
    QString fnd2="";
    QString fnd3="";
    QString fnt1="";
    QString fnt2="";
    QString fn="";

    QString filler;

    // baue Filenamen aus mypref auf...
    // filler ermitteln
    switch(mypref->space()){
    case UNDERSCORE: filler = "_"; break;
    case ADD: filler = "+"; break;
    case SUB: filler = "-"; break;
    case ORIGINAL: filler = " "; break;
    case DELETE: filler = ""; break;
    }

    fnd1 = makeFilePart(mypref->d1(),filler);
    fnd2 = makeFilePart(mypref->d2(),filler);
    fnd3 = makeFilePart(mypref->d3(),filler);

    if(fnd1.isEmpty()){
        fnt1="";
    }
    else{
        fnt1 = makeFileDelim(mypref->t1());
    }

    if(fnd3.isEmpty()){
        fnt2="";
    }
    else{
        fnt2 = makeFileDelim(mypref->t2());
    }

    if(     (fnd1.isEmpty() && fnd2.isEmpty()) ||
            (fnd2.isEmpty() && fnd3.isEmpty()) ||
            (fnd1.isEmpty() && fnd3.isEmpty())){
        fnt1="";
        fnt2="";
    }

    fn = fnd1 + fnt1 + fnd2 + fnt2 + fnd3 + ".json";

    ui->path->setText(fn);
    ui->saveFile->setEnabled(true);
    this->fileName=fn;
    return fn;
}


void MainWindow::on_saveFile_clicked()
{
    if(checkModel()){
        QString filepath="";
        filepath = ui->folder->text() + QDir::separator() + fileName;
        QJsonObject json = packQJD();
        saveJson(json,filepath);
        qDebug()<<"Gesichert unter " + filepath;
        ui->saveFile->setEnabled(false);
    }    
}


void MainWindow::on_actionSichern_triggered()
{
        on_saveFile_clicked();
}

void MainWindow::makeNewTimer(){
    bool runflag=false;
    qint32 newValue = mypref->minutes();
    if (newValue == maxMinutes)
        return;
    if(isTimerStarted){
        toggleStartStop();
        runflag = true;
    }
    timerReset();
    ui->progressBar->setMaximum(newValue);
    maxMinutes = newValue;
    ui->label_minutes->setText(QString::number(newValue) +"min");
    if(runflag){
        toggleStartStop();
    }
}

bool MainWindow::checkModel(){
    // checkt ob mehrere 1. oder 2. Korrektoren  oder weniger als 3 Prüfer anwesend sind
    bool b1=false,b2=false;
    bool b3=false,b4=false;
    bool b5=false;
    bool retVal=true;
    QString Err1String="";
    QString Err2String="";
    QString Err3String="";
    QString Err4String="";
    QString ErrString="";
    qint32 checkKorr1 = 0;
    qint32 checkKorr2 = 0;
    qint32 checkCount = 0;
    qint32 countKorr= 0;
    QModelIndex parent = ui->comboBoxExam_2->rootModelIndex();
    qint32 i = ui->comboBoxExam_2->currentIndex();
    QModelIndex start = ui->tableView->model()->index(i,0,parent);
    for( int row = 0; row < ui->tableView->model()->rowCount(start); ++row ) {
        QString name = QVariant(ui->tableView->model()->index(row,0,start).data(Qt::ItemIsEditable)).toString();
        countKorr=0;
        for ( int col = 1; col < ui->tableView->model()->columnCount(start); ++col ) {
            if(ui->tableView->model()->index(row,col,start).data(Qt::CheckStateRole).toUInt()>0){
                switch(col){
                case 1: // 1.Korr
                    checkKorr1  += 1;
                    countKorr++;
                    break;
                case 2: // 2.Korr
                    checkKorr2 +=1;
                    countKorr++;
                    break;
                case 3: // Anwesend
                    checkCount += 1;
                default:
                    break;
                }
            }
        }
        if(countKorr>1){
            b5 = true;
        }
    }
    b1 = (checkKorr1 > 1);
    b2 = (checkKorr2 > 1);
    if(b1 || b2){
        Err1String = "Zuviele ";
        if(b1 && b2){
            Err1String += "Erst und Zweitkorrektoren";
        }
        else{
            Err1String += (b1)?"Erstkorrektoren":"Zweitkorrektoren";
        }
        Err1String +="!";
    }
    if(checkCount<3){
        Err4String = "Weniger als 3 anwesende Prüfer!";
    }
    
    b3 = (checkKorr1 < 1);
    b4 = (checkKorr2 < 1);
    if(b3 || b4){
        Err2String = "Zuwenig ";
        if(b3 && b4){
            Err2String += "Erst und Zweitkorrektoren";
        }
        else{
            Err2String += (b3)?"Erstkorrektoren":"Zweitkorrektoren";
        }
        Err2String +="!";
    }
    
    if(b5){
        Err3String = "Korrektor mit 1. UND 2. Korrektur!";
    }
    Err1String += (!Err1String.isEmpty())?"\n":"";
    Err2String += (!Err2String.isEmpty())?"\n":"";
    Err3String += (!Err3String.isEmpty())?"\n":"";
    ErrString = Err1String + Err2String + Err3String + Err4String;
  
    
    
    if(!ErrString.isEmpty()){
        ErrString += "\n\nTrotzdem weitermachen?";
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,tr("Warnung"),ErrString,QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            retVal = true;
        }
        else{
            retVal= false;
        }
    }
    return retVal;
}

void MainWindow::clearModelCheckboxes(bool all){
    QModelIndex parent = ui->comboBoxExam_2->rootModelIndex();
    qint32 iAusschuss = ui->comboBoxExam_2->currentIndex();
    QModelIndex start = ui->tableView->model()->index(iAusschuss,0,parent);
    for( int row = 0; row < ui->tableView->model()->rowCount(start); ++row ) {
        for ( int col = 1; col < ui->tableView->model()->columnCount(start); ++col ) {
            qDebug()<<row<<"("<< ui->tableView->model()->rowCount(start)<<"),"<<col<<"("<< ui->tableView->model()->columnCount(start)<<")";
            if( ui->tableView->model()->index(row,col,start).data(Qt::CheckStateRole).toUInt() > 0 ){
                switch(col){
                case 1: // 1.Korr
                    treeModel->setData(ui->tableView->model()->index(row,col,start),Qt::Unchecked,Qt::CheckStateRole);
                    break;
                case 2: // 2.Korr
                    treeModel->setData(ui->tableView->model()->index(row,col,start),Qt::Unchecked,Qt::CheckStateRole);
                    break;
                case 3: // Anwesend
                    if(all){
                        treeModel->setData(ui->tableView->model()->index(row,col,start),Qt::Unchecked,Qt::CheckStateRole);                       
                    }
                default:
                    break;
                }
            }
        }
    }
    ui->saveFile->setEnabled(true);
}
