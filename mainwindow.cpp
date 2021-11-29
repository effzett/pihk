#include "mainwindow.h"
#include "about.h"
#include "preferences.h"
#include "lizenz.h"
#include "regularien.h"
#include "treemodel.h"
#include "ihk.h"

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
    // app specific (initial data from app.h)
    app.version=APP_VERSION;
    app.name=APP_NAME;
    app.author=APP_AUTHOR;
    app.email= APP_EMAIL;
    app.domain = APP_DOMAIN;
    app.organization = APP_ORGANIZATION;
    app.date = getBuildDate();
    app.versionLong = app.name + "   (V" +app.version +", vom " + app.date + ")"; // for printing as window title 

    // other needful variables
    QCoreApplication::setOrganizationName(app.organization);
    QCoreApplication::setApplicationName(app.name);
    QCoreApplication::setOrganizationDomain(app.domain);
    QCoreApplication::setApplicationVersion(app.version);
    
    // Timer related
    isTimerStarted=false;
    timerValue=0;
    offset=0;
    timer = new QTimer(this);


    ui->setupUi(this);
    ui->pushButtonIhk->setVisible(false);
    //ui->menuAnsicht->menuAction()->setVisible(false);
    ui->folder->setPlaceholderText(QDir::homePath());
    ui->folder->setText(QDir::homePath());
    ui->lcdNumber->setPalette(Qt::black);

    // start values will be overwritten by loadSettings(), just for first use
    mypref = new Prefs(maxMinutes,FILEPARTS::DATUM,FILEPARTSDELIM::MINUS,FILEPARTS::NAME,FILEPARTSDELIM::MINUS,FILEPARTS::NUMMER,FILESPACECHAR::UNDERSCORE);
    const QStringList headers({tr("Prüfer"),tr("K1"),tr("K2"),tr("Anw")});
    loadSettings(true,headers);  // setzt auch das Model fuer Tree und comboboxen
    // From resource file (1. Nutzung)  or from Qsettings data (Einmal Werte abgespeichert.)...


    
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
    ui->labelGradeT21->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeT2->setStyleSheet("QLabel { color : red; }");
    ui->labelGradeResultT2->setStyleSheet("QLabel { color : red; }");
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
//    ui->folder->setPlaceholderText(QDir::homePath());
//    ui->folder->setText(QDir::homePath());


    // Table View
    // tableWidget initialisieren

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHeaderView* header=ui->tableView->verticalHeader();
    header->setDefaultSectionSize(20); // 20 px height
    header->sectionResizeMode(QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0,290);
    ui->tableView->setColumnWidth(1,30);
    ui->tableView->setColumnWidth(2,30);
    ui->tableView->setColumnWidth(3,30);
    ui->tableView->horizontalScrollBar()->setDisabled(true);
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setSectionResizeMode(0,QHeaderView::Fixed);
    headerView->setSectionResizeMode(1,QHeaderView::Fixed);
    headerView->setSectionResizeMode(2,QHeaderView::Fixed);
    headerView->setSectionResizeMode(3,QHeaderView::Fixed);



    //emit ui->comboBoxExam->currentIndexChanged(0);
    //emit ui->comboBoxExam_2->currentIndexChanged(0);


    
    // current preference values are now loaded

    maxMinutes=mypref->minutes();   // for convenience as member variable    
    fileName = makeFilename();      // construct basic file name from preference values

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
    connect(ui->spinboxGa0,SIGNAL(valueChanged(double)),this,SLOT(writeResults()));
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
    connect(ui->pushButtonIhk,SIGNAL(clicked()),ui->actionAusgabeblatt,SIGNAL(triggered()));
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
        ui->lcdNumber->setPalette(QColor(255,165,0,255));
    }
    ui->lcdNumber->display(timerValue-offset);
}
// start or stop timer
void MainWindow::toggleStartStop(){
    if(isTimerStarted==false){
        ui->startTimer->setText("Stop");
        ui->resetTimer->hide();
        isTimerStarted = true;
        timer->start(60000);    // every minute
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


qint32 MainWindow::calcAll(qint32 epnr,qint32 mueergpr){
    qint32 pointsAll=0;
    qint32 pointst1 = ui->spinboxGa0->text().toInt();
    switch(epnr){
    case 0:
        pointsAll = qRound((pointst1*2 + t21()*5 +t22() +t23() +t24())/10.0);
        break;
    case 1:
        pointsAll = qRound((pointst1*2 + t21()*5 +t22(mueergpr) +t23() +t24())/10.0);       
        break;
    case 2:
        pointsAll = qRound((pointst1*2 + t21()*5 +t22() +t23(mueergpr) +t24())/10.0);       
        break;
    case 3:
        pointsAll = qRound((pointst1*2 + t21()*5 +t22() +t23() +t24(mueergpr))/10.0);       
        break;
    default:
        //error
        ;
    }
    return pointsAll;
}

void MainWindow::writeResults(){
    qint32 nr=0;
    qint32 points=0;
    qint32 status=0;
    
    // Relevante Werte einlesen
    qint32 ga1 = static_cast<qint32>(ui->spinboxGa1->value());
    qint32 ga2  = static_cast<qint32>(ui->spinboxGa2->value());
    qint32 wiso  = static_cast<qint32>(ui->spinboxWiso->value());

    qint32 pointsT21 = t21();
    // Werte in die GUI schreiben...
    ui->labelPointsT21->setText(QString::number(pointsT21));
    colorLabel(ui->labelPointsT21,pointsT21);
    ui->labelGradeT21->setText(getGrade(pointsT21));
    colorLabel(ui->labelGradeT21,pointsT21);

    if(checkMAllowed()){    // oral is possible
        if(couldPass(1) && ga1<50){ // oral in ga1)   // UND Gesamtprüfung muss bestanden werden können
            ui->radioButton1->setEnabled(true);
            if(ui->radioButton1->isChecked()){ // points for oral in ga1 exists
                ui->spinboxGa1E->setEnabled(true);
                ui->spinboxGa1E->setFocus();
                ui->spinboxGa1E->show();
                nr=1;
                points=static_cast<qint32>(qRound(ui->spinboxGa1E->value()));
            }
            else{ // mit 0 initialisieren und verstecken
                ui->spinboxGa1E->setValue(0);
                ui->spinboxGa1E->setEnabled(false);
                ui->spinboxGa1E->hide();
            }
        }
        else{ // oral in ga1 not possible
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
        if(couldPass(2) && ga2<50){ // oral in ga2   // UND Gesamtprüfung muss bestanden werden
            ui->radioButton2->setEnabled(true);
            if(ui->radioButton2->isChecked()){ // points for oral in ga2 exists
                 ui->spinboxGa2E->setEnabled(true);
                 ui->spinboxGa2E->setFocus();
                 ui->spinboxGa2E->show();
                 nr=2;
                 points=static_cast<qint32>(qRound(ui->spinboxGa2E->value()));
                 qDebug()<<"Nachher"<<points;
             }
             else{ // mit 0 initialisieren und verstecken
                 ui->spinboxGa2E->setValue(0);
                 ui->spinboxGa2E->setEnabled(false);
                 ui->spinboxGa2E->hide();
             }
        }
        else{   // oral in ga2 not possible
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
        if(couldPass(3) && wiso<50){ // oral in wiso
            ui->radioButton3->setEnabled(true);
            if(ui->radioButton3->isChecked()){ // points for oral in wiso exists
                ui->spinboxWisoE->setEnabled(true);
                ui->spinboxWisoE->setFocus();
                ui->spinboxWisoE->show();
                nr=3;
                points=static_cast<qint32>(qRound(ui->spinboxWisoE->value()));
            }
            else{   // mit 0 initialisieren und verstecken
                ui->spinboxWisoE->setValue(0);
                ui->spinboxWisoE->setEnabled(false);
                ui->spinboxWisoE->hide();
            }

        }
        else{   // oral in wiso not possible
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
    else{   // oral not possible. hide and reset radio buttons and spinboxees
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

    qint32 pointsT2 = t2(nr,points);
    colorLabel(ui->labelGradeT2,pointsT2);
    ui->labelPointsB->setText(QString::number(pointsT2).rightJustified(3,' '));
    ui->labelGradeT2->setText(getGrade(pointsT2).rightJustified(12,' '));


    // Results

    // Part T2
    ui->labelResultT2->setText(QString::number(pointsT2).rightJustified(3,' '));
    ui->labelGradeResultT2->setText(getGrade(pointsT2).rightJustified(12,' '));
    colorLabel(ui->labelResultT2,pointsT2);
    colorLabel(ui->labelGradeResultT2,pointsT2);

    // All
    qint32 pointsAll = calcAll(nr,points);
    ui->labelResultAll->setText(QString::number(pointsAll).rightJustified(3,' '));
    ui->labelGradeResult->setText(getGrade(pointsAll).rightJustified(12,' '));

    fillPRFG();
    fillMEPR();
    
    // Prüfbereiche aus T2 füllen
    ui->label_t21->setText(QString::number(t21()));
    colorLabel(ui->label_t21,t21());
    ui->label_t22->setText((nr==1)?QString::number(t22(points)):QString::number(t22()));
    colorLabel(ui->label_t22,((nr==1)?t22(points):t22()));
    ui->label_t23->setText((nr==2)?QString::number(t23(points)):QString::number(t23()));
    colorLabel(ui->label_t23,((nr==2)?t23(points):t23()));
    ui->label_t24->setText((nr==3)?QString::number(t24(points)):QString::number(t24()));
    colorLabel(ui->label_t24,((nr==3)?t24(points):t24()));

    // Aktueller Stand der Berechnungen  (sperrt/färbt nichts)
    status = hasPassedExamination();
    if(status & 2){
        ui->label_info1->setText("Weniger als 3 PB in T2 mit ausreichend!!");
    }else{
        ui->label_info1->setText("");
    }

    // Aktueller Stand der Berechnungen  (sperrt/färbt nichts)    
    if(status & 1){
        ui->label_info2->setText("Mindestens ein PB in T2 mit ungenügend!!");
    }else{
        ui->label_info2->setText("");
    }
    
    // Bestimmen, ob die Gesamtprüfung bestanden ist
    if(status == 0){
        ui->labelGradeResult->setStyleSheet("QLabel {color:green;}");
        ui->labelResultAll->setStyleSheet("QLabel {color:green;}");
    }else{
        ui->labelGradeResult->setStyleSheet("QLabel {color:red;}");
        ui->labelResultAll->setStyleSheet("QLabel {color:red;}");
    }
    
    ui->saveFile->setEnabled(true);
}


// prüft, ob Gesamtprüfung bestanden
// 0 bestanden
// 1 mindestens eine 6
// 2 mehr als 2 5en
// 4 Teil 2 schlechter als 4
// 8 Gesamtprüfung schlechter als 4
qint32 MainWindow::hasPassedExamination(){
    qint32 retVal = 0;
    qint32  cnt=0;
    qint32 pb11 = ui->spinboxGa0->text().toInt();
    qint32 pb21 = ui->label_t21->text().toInt();
    qint32 pb22 = ui->label_t22->text().toInt();
    qint32 pb23 = ui->label_t23->text().toInt();
    qint32 pb24 = ui->label_t24->text().toInt();

    // keine ungenügend
    if(pb21<30 || pb22<30 || pb23<30 || pb24<30){  // mindestens ein ungenügend
        retVal += 1;
    }
    
    // nur ein mangelhaft erlaubt
    if(pb21<50)
        cnt++;
    if(pb22<50)
        cnt++;
    if(pb23<50)
        cnt++;
    if(pb24<50)
        cnt++;
    if(cnt>1){
        retVal += 2;
    }

    // Teil 2 muss mindestens ausreichend sein
    if(qRound((pb21+pb22+pb23+pb24)/4.0)<50){
        retVal += 4;
    }
    
    // Gesamtprüfung muss mindestens ausreichend sein
    if(qRound((pb11*2.0+pb21*5.0+pb22+pb23+pb24)/10.0)<50){
        retVal += 8;
    }
    
    return retVal;
}

// prüft, ob Gesamtprüfung alle Bedingungen mit mündlicher Ergänzungsprüfung erfüllen KÖNNTE
bool MainWindow::couldPass(qint32 nr){
    qint32  cnt=0;
    qint32 pb21 = ui->labelPointsT21->text().toInt();
    qint32 pb22 = ui->spinboxGa1->text().toInt();
    qint32 pb23 = ui->spinboxGa2->text().toInt();
    qint32 pb24 = ui->spinboxWiso->text().toInt();

    
    // optimale Ergebnisse in der muendlichen Prüfung
    switch(nr){
    case 0: // nothing
        break;
    case 1: pb22 = qRound((pb22*2.0+100.0)/3.0);
        break;
    case 2: pb23 = qRound((pb23*2.0+100.0)/3.0);
        break;
    case 3: pb24 = qRound((pb24*2.0+100.0)/3.0);
        break;
    default:    //nothing
        break;
    }

    // keine ungenügend
    if(pb21<30 || pb22<30 || pb23<30 || pb24<30){
        return false;
    }
    
    // nur ein mangelhaft erlaubt
    if(pb21<50)
        cnt++;
    if(pb22<50)
        cnt++;
    if(pb23<50)
        cnt++;
    if(pb24<50)
        cnt++;
    if(cnt>1){
        return false;
    }

    // Teil 2 muss mindestens ausreichend sein
    if(qRound((pb21+pb22+pb23+pb24)/4.0)<50){
        return false;
    }
    
    // Gesamtprüfung muss mindestens ausreichend sein
    if(qRound((t11()*2.0+pb21*5.0+pb22+pb23+pb24)/10.0)<50){
        return false;
    }
    
    return true;
}


bool MainWindow::checkMAllowed(){
    qint32 ga1 = static_cast<qint32>(qRound(ui->spinboxGa1->value()));
    qint32 ga2 = static_cast<qint32>(qRound(ui->spinboxGa2->value()));
    qint32 wiso = static_cast<qint32>(qRound(ui->spinboxWiso->value()));
    
    if(couldPass()){     // zu gut
        return false;   // keine weiteren Prüfungen notwendig
    }

    // Es folgen die einzigen 3 Möglichkeiten
    // in Ga1
    if(couldPass(1) &&  ga1<50 ){ // darf mit mündlicher Prüfung nicht durchfallen
        return true;
    }
    // in Ga2
    if(couldPass(2) &&  ga2<50 ){ // darf mit mündlicher Prüfung nicht durchfallen
        return true;
    }
    // in Wiso
    if(couldPass(3) && wiso<50    ){ // darf mit mündlicher Prüfung nicht durchfallen
        return true;
    }
    return false;
}

qint32 MainWindow::getOralNr(){
    if(ui->radioButton1->isChecked())
            return 1;
    if(ui->radioButton2->isChecked())
            return 2;
    if(ui->radioButton3->isChecked())
            return 3;
    return 0;
}

qint32 MainWindow::getOralPoints(){
    if(ui->radioButton1->isChecked())
            return ui->spinboxGa1E->text().toInt();
    if(ui->radioButton2->isChecked())
            return ui->spinboxGa2E->text().toInt();
    if(ui->radioButton3->isChecked())
            return ui->spinboxWisoE->text().toInt();
    return 0;
}

// prüft, ob Gesamtprüfung bestanden aber mit simulierten Werten
// 0 bestanden
// 1 mindestens eine 6
// 2 mehr als 2 5en
// 4 Teil 2 schlechter als 4
// 8 Gesamtprüfung schlechter als 4
bool MainWindow::passedSimExamination(qint32 t11,qint32 t21,qint32 t22,qint32 t23,qint32 t24){
    qint32  cnt=0;
    qint32 pb11 = t11;
    qint32 pb21 = t21;
    qint32 pb22 = t22;
    qint32 pb23 = t23;
    qint32 pb24 = t24;

    // keine ungenügend
    if(pb21<30 || pb22<30 || pb23<30 || pb24<30){  // mindestens ein ungenügend
        return false;
    }
    
    // nur ein mangelhaft erlaubt
    if(pb21<50)
        cnt++;
    if(pb22<50)
        cnt++;
    if(pb23<50)
        cnt++;
    if(pb24<50)
        cnt++;
    if(cnt>1){
        return false;
    }

    // Teil 2 muss mindestens ausreichend sein
    if(qRound((pb21+pb22+pb23+pb24)/4.0)<50){
        return false;
    }
    
    // Gesamtprüfung muss mindestens ausreichend sein
    if(qRound((pb11*2.0+pb21*5.0+pb22+pb23+pb24)/10.0)<50){
        return false;
    }
    return true;
}

void MainWindow::fillPRFG(){
    // Create model
    model = new QStringListModel(this);

    // Make data
    QStringList list;
    qint32 nr = getOralNr(); // nummer der muendl. Prüfung
    qint32 points = getOralPoints(); // Punkte der muendlichen Prüfung
    qint32 a; // simulated T21
    qint32 b; // simulated T2
    qint32 g; // simulated Result    
    QString aGrade=getGrade(0,QUALITY::LONG);
    QString bGrade=getGrade(0,QUALITY::LONG);
    QString gGrade=getGrade(0,QUALITY::LONG);
    bool passedOld=false;
    bool passed=false;

    for(int i=0;i<=100;i++){
        a = t21(i);
        switch(nr){
        case 0:
            b = qRound((a+t22()+t23()+t24())/4.0);
            g = qRound((t11()*2.0+a*5.0+t22()+t23()+t24())/10.0);
            passed = passedSimExamination(t11(),a,t22(),t23(),t24());
            break;
        case 1:
            b = qRound((a+t22(points)+t23()+t24())/4.0);
            g = qRound((t11()*2.0+a*5.0+t22(points)+t23()+t24())/10.0);
            passed = passedSimExamination(t11(),a,t22(points),t23(),t24());
            break;
        case 2:
            b = qRound((a+t22()+t23(points)+t24())/4.0);
            g = qRound((t11()*2.0+a*5.0+t22()+t23(points)+t24())/10.0);
            passed = passedSimExamination(t11(),a,t22(),t23(points),t24());
            break;
        case 3:
            b = qRound((a+t22()+t23()+t24(points))/4.0);
            g = qRound((t11()*2.0+a*5.0+t22()+t23()+t24(points))/10.0);
            passed = passedSimExamination(t11(),a,t22(),t23(),t24(points));
            break;
        default:
            b= qRound((a+t22()+t23()+t24())/4.0);
            g = qRound((t11()*2.0+a*5.0+t22()+t23()+t24())/10.0);
            passed = passedSimExamination(t11(),a,t22(),t23(),t24());
            break;
        }
        
        // Bei Veränderung...
        if(     aGrade.compare(getGrade(a,QUALITY::LONG))!=0 ||
                bGrade.compare(getGrade(b,QUALITY::LONG))!=0 ||
                gGrade.compare(getGrade(g,QUALITY::LONG))!=0 ||  passed!=passedOld){
            QString item;
            if(passed == true){
                item = QString("%1  T21=%2 T2=%3 +G=%4").arg(i,3)
                    .arg(getGrade(a,QUALITY::LONG),-12).arg(getGrade(b,QUALITY::LONG),-12).arg(getGrade(g,QUALITY::LONG),-12);

                passedOld = passed;
            }else{
                item = QString("%1  T21=%2 T2=%3 -G=%6").arg(i,3)
                                    .arg(getGrade(a,QUALITY::LONG),-12).arg(getGrade(b,QUALITY::LONG),-12).arg(getGrade(g,QUALITY::LONG),-12);
            }
            list << item;
            aGrade=getGrade(a,QUALITY::LONG);
            bGrade=getGrade(b,QUALITY::LONG);
            gGrade=getGrade(g,QUALITY::LONG);
        }
    }


// Populate our model
    model->setStringList(list);

        // Glue model and view together
#ifdef Q_OS_OSX
        QFont newFont("Courier", 12, QFont::Normal, true);
#else
        QFont newFont("Courier", 10, QFont::Normal, true);
#endif
    ui->listViewPRFG->setFont(newFont);
    ui->listViewPRFG->setModel(model);
}

void MainWindow::fillMEPR(){
    // Create model
    model = new QStringListModel(this);

    // Make data
    QStringList list;
    qint32 nr=getOralNr();
    qint32 a;  // simulierte Ergebnisse in der Teilprüfung T2x
    qint32 b; //Teil2 simuliert
    qint32 g; // Resultat simuliert
    bool passed = false;
    bool passedOld=false;
    QString aGrade = getGrade(0,QUALITY::LONG);
    QString bGrade = getGrade(0,QUALITY::LONG);
    QString gGrade = getGrade(0,QUALITY::LONG);

    if(checkMAllowed() && nr>0){
        for(int i=0;i<=100;i++){
            switch(nr){
            case 0: // sollte niemals auftreten
                a=0;
                b = qRound((t21()+t22()+t23()+t24())/4.0);
                g = qRound((t11()*2.0+t21()*5.0+t22()+t23()+t24())/10.0);
                passed = passedSimExamination(t11(),t21(),t22(),t23(),t24());
                break;
            case 1:
                a=t22(i);
                b = qRound((t21()+t22(i)+t23()+t24())/4.0);
                g = qRound((t11()*2.0+t21()*5.0+t22(i)+t23()+t24())/10.0);
                passed = passedSimExamination(t11(),t21(),t22(i),t23(),t24());
                break;
            case 2:
                a=t23(i);
                b = qRound((t21()+t22()+t23(i)+t24())/4.0);
                g = qRound((t11()*2.0+t21()*5.0+t22()+t23(i)+t24())/10.0);
                passed = passedSimExamination(t11(),t21(),t22(),t23(i),t24());
                break;
            case 3:
                a=t24(i);
                b = qRound((t21()+t22()+t23()+t24(i))/4.0);
                g = qRound((t11()*2.0+t21()*5.0+t22()+t23()+t24(i))/10.0);
                passed = passedSimExamination(t11(),t21(),t22(),t23(),t24(i));
                break;
            default: // sollte niemals auftreten
                a=0;
                b= qRound((t21()+t22()+t23()+t24())/4.0);
                g = qRound((t11()*2.0+t21()*5.0+t22()+t23()+t24())/10.0);
                passed = passedSimExamination(t11(),t21(),t22(),t23(),t24());
                break;
            }
            
            // es hat sich was verändert...
            if(     aGrade.compare(getGrade(a,QUALITY::LONG))!=0 ||
                    bGrade.compare(getGrade(b,QUALITY::LONG))!=0 ||
                    gGrade.compare(getGrade(g,QUALITY::LONG))!=0 || (passed!=passedOld)){
                QString item;
                if(passed==true){
                    item = QString("%1  T2%2=%3 T2=%4 +G=%5").arg(i,3).arg((nr+1),1)
                            .arg(getGrade(a,QUALITY::LONG),-12).arg(getGrade(b,QUALITY::LONG),-12).arg(getGrade(g,QUALITY::LONG),-12);
                    passedOld = passed;
                }
                else{
                    item = QString("%1  T2%2=%3 T2=%4 -G=%5").arg(i,3).arg((nr+1),1)
                            .arg(getGrade(a,QUALITY::LONG),-12).arg(getGrade(b,QUALITY::LONG),-12).arg(getGrade(g,QUALITY::LONG),-12);
                }
                list << item;
                aGrade = getGrade(a,QUALITY::LONG);
                bGrade = getGrade(b,QUALITY::LONG);
                gGrade = getGrade(g,QUALITY::LONG);
            }
        } // for...
    }else{
        list.clear();
    }

        // Populate our model
    model->setStringList(list);

// Glue model and view together
#ifdef Q_OS_OSX
        QFont newFont("Courier", 12, QFont::Normal, true);
#else
        QFont newFont("Courier", 10, QFont::Normal, true);
#endif
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
QString MainWindow::getGrade(qint32 points, QUALITY q){

    QString grade = (q==QUALITY::LONG)?"ungenügend":"6";
    if(points>=30){
        grade = (q==QUALITY::LONG)?"mangelhaft":"5";
    }
    if(points>=50){
        grade = (q==QUALITY::LONG)?"ausreichend":"4";
    }
    if(points>=67){
        grade = (q==QUALITY::LONG)?"befriedigend":"3";
    }
    if(points>=81){
        grade = (q==QUALITY::LONG)?"gut":"2";
    }
    if(points>=92){
        grade = (q==QUALITY::LONG)?"sehr gut":"1";
    }
    return grade;
}

QJsonObject MainWindow::packQJD(){
    QJsonObject json;
    QJsonArray anwesend;
    QJsonArray korr1;
    QJsonArray korr2;
    
    json["PIHKVersion"] = app.version;
    json["Fachrichtung"] = ui->comboBoxExam->currentText().trimmed();
    json["Ausschuss"] = ui->comboBoxExam_2->currentText().trimmed();
    json["Datum"] = ui->pDate->text();
    json["Name"] = ui->pname->text().trimmed();
    json["Id-Nummer"] = ui->pnummer->text().trimmed();
    json["Doku"] = ui->spinboxDocumentation->text();
    json["PRFG"] = ui->spinboxExamination->text();
    json["GA0"] = ui->spinboxGa0->text();
    json["GA1"] = ui->spinboxGa1->text();
    json["GA2"] = ui->spinboxGa2->text();
    json["Wiso"] = ui->spinboxWiso->text();
    json["MEP-GA1"] = ui->spinboxGa1E->text();
    json["MEP-GA2"] = ui->spinboxGa2E->text();
    json["MEP-WISO"] = ui->spinboxWisoE->text();
    json["Prüfungszeit"] = ui->lcdNumber->value();
    //json["Ergebnis A"] = ui->labelResultA->text()+" ("+ui->labelGradeResultA->text().trimmed()+")"; // Wird nicht wieder eingelesen!
    json["Ergebnis B"] = ui->labelResultT2->text()+" ("+ui->labelGradeResultT2->text().trimmed()+")";   // Wird nicht wieder eingelesen!
    json["Ergebnis"] = ui->labelResultAll->text()+" ("+ui->labelGradeResult->text().trimmed()+")";    // Wird nicht wieder eingelesen!
    json["Prüfungsergebnis"] = (hasPassed)?"BESTANDEN":"NICHT bestanden";                   // Wird nicht wieder eingelesen!

    // Auslesen der Prüfer aus dem Model
    QModelIndex parent = ui->comboBoxExam_2->rootModelIndex();
    qint32 i = ui->comboBoxExam_2->currentIndex();
    QModelIndex start = ui->tableView->model()->index(i,0,parent);
    for( int row = 0; row < ui->tableView->model()->rowCount(start); ++row ) {
        QString name = QVariant(ui->tableView->model()->index(row,0,start).data(Qt::ItemIsEditable)).toString();
        for ( int col = 1; col < ui->tableView->model()->columnCount(start); ++col ) {
            if(ui->tableView->model()->index(row,col,start).data(Qt::CheckStateRole).toUInt()>0){
                switch(col){
                case 1: // 1.Korr
                    korr1.append(name);
                    break;
                case 2: // 2.Korr
                    korr2.append(name);
                    break;
                case 3: // Anwesend
                    anwesend.append(name);
                    break;
                default:
                    break;
                }
            }
        }
    }
    json.insert("Korr1",korr1);
    json.insert("Korr2",korr2);
    json.insert("Anwesend",anwesend);
    
    return json;
}

void MainWindow::unpackQJO(QJsonObject json ){
    QList<QVariantList> prueferListe;
    
    app.version =  json["PIHKVersion"].toString();
    ui->comboBoxExam->setCurrentText(json["Fachrichtung"].toString().trimmed());
    ui->comboBoxExam_2->setCurrentText(json["Ausschuss"].toString().trimmed());
    ui->pDate->setDate(QDate::fromString(json.value("Datum").toString(),"dd.MM.yyyy"));
    ui->pname->setText(json.value("Name").toString().trimmed());
    ui->pnummer->setText(json.value("Id-Nummer").toString().trimmed());
    ui->spinboxDocumentation->setValue(json.value("Doku").toString().toInt());
    ui->spinboxExamination->setValue(json.value("PRFG").toString().toInt());
    ui->spinboxGa0->setValue(json.value("GA0").toString().toInt()); 
    ui->spinboxGa1->setValue(json.value("GA1").toString().toInt());
    ui->spinboxGa2->setValue(json.value("GA2").toString().toInt());
    ui->spinboxWiso->setValue(json.value("Wiso").toString().toInt());
    if(json.value("MEP-GA1").toString().toInt() != 0){
        ui->radioButton1->setEnabled(true);
        ui->radioButton1->setChecked(true);
        ui->spinboxGa1E->setValue(json.value("MEP-GA1").toString().toInt());
    }
    if(json.value("MEP-GA2").toString().toInt() != 0){
        ui->radioButton2->setEnabled(true);
        ui->radioButton2->setChecked(true);
        ui->spinboxGa2E->setValue(json.value("MEP-GA2").toString().toInt());
    }
    if(json.value("MEP-WISO").toString().toInt() != 0){
        ui->radioButton3->setEnabled(true);
        ui->radioButton3->setChecked(true);
        ui->spinboxWisoE->setValue(json.value("MEP-WISO").toString().toInt());
    }
    ui->lcdNumber->display(static_cast<qint32>(json["Prüfungszeit"].toInteger()));

    // Wird automatisch ermittelt:
    //   * json["Ergebnis A"] = ui->labelResultA->text()+" ("+ui->labelGradeResultA->text()+")";
    //   * json["Ergebnis B"] = ui->labelResultB->text()+" ("+ui->labelGradeResultB->text()+")";
    //   * json["Ergebnis"]   = ui->labelResultAll->text()+" ("+ui->labelGradeResult->text()+")";
    //   * Prüfungsergebnis
    
    // Einlesen der Prüfer
    clearModelCheckboxes(ui->checkBoxAll->isChecked());
    prueferListe = readPruefer(json);
    for(int pv=0; pv< prueferListe.count();pv++){   // Über alle gefundenen Prüfer gehen
        QVariantList v = prueferListe[pv];
        insertPrueferIntoModel(v);  // ein Prüfer als QVariantList 
    }
}

QList<QVariantList> MainWindow::readPruefer(QJsonObject json){
    QList<QVariantList> listAll;
    QVariant qv0,qv1,qv2,qv3;
    QJsonArray anwesend;
    QJsonArray korr1;
    QJsonArray korr2;
    QSet<QString> namen;
    QString name;
    anwesend = json.value("Anwesend").toArray();
    korr1 = json.value("Korr1").toArray();
    korr2 = json.value("Korr2").toArray();
    for(int i=0;i<anwesend.count();i++){
        namen.insert(anwesend.at(i).toString().trimmed());
    }
    for(int i=0;i<korr1.count();i++){
        namen.insert(korr1.at(i).toString().trimmed());
    }
    for(int i=0;i<korr2.count();i++){
        namen.insert(korr2.at(i).toString().trimmed());
    }
    QSet<QString>::iterator si;
    for (si = namen.begin(); si != namen.end(); ++si){
        QVariantList list;
        name = *si;
        qv0 = QVariant(name);
        qv1 = (korr1.contains(name))?QVariant(Qt::Checked):QVariant(Qt::Unchecked);
        qv2 = (korr2.contains(name))?QVariant(Qt::Checked):QVariant(Qt::Unchecked);
        qv3 = (anwesend.contains(name))?QVariant(Qt::Checked):QVariant(Qt::Unchecked);
        list.append(qv0);
        list.append(qv1);
        list.append(qv2);
        list.append(qv3);
        listAll.append(list);
    }
    return listAll;
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
    QString f = QFileDialog::getOpenFileName(this,tr("Öffnen"),ui->folder->text(),tr("JSON (*.json)"));
    if(f.length() != 0){
        QJsonObject json = loadJson(f);
        unpackQJO(json);
    }
}


void MainWindow::on_actionSichernAls_triggered()
{
    if(checkModel()){
        QString f = QFileDialog::getSaveFileName(this,tr("Sichern"),ui->folder->text(),tr("JSON (*.json)"));
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
    }
    timerReset();
    clearModelCheckboxes(ui->checkBoxAll->isChecked());
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
    ui->comboBoxExam_2->setModel(treeModel);
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
    case static_cast<qint32>(FILEPARTS::DATUM): retVal = ui->pDate->date().toString("yyyyMMdd");break;
    case static_cast<qint32>(FILEPARTS::NAME): retVal = ui->pname->text().replace(" ",filler);break;
    case static_cast<qint32>(FILEPARTS::NUMMER): retVal = ui->pnummer->text();break;
    case static_cast<qint32>(FILEPARTS::FACHRICHTUNG): retVal = ui->comboBoxExam->currentText().trimmed().replace(" ",filler);break;
    case static_cast<qint32>(FILEPARTS::AUSSCHUSS): retVal = ui->comboBoxExam_2->currentText().trimmed().replace(" ",filler);break;
    case static_cast<qint32>(FILEPARTS::LEER): retVal = "" ;break;
    }
    return retVal;
}

QString MainWindow::makeFileDelim(qint32 index){
    QString retVal = "";
    switch(index){
    case static_cast<qint32>(FILEPARTSDELIM::MINUS): retVal = "-";break;
    case static_cast<qint32>(FILEPARTSDELIM::PLUS): retVal = "+" ;break;
    case static_cast<qint32>(FILEPARTSDELIM::PUNKT): retVal = "." ;break;
    case static_cast<qint32>(FILEPARTSDELIM::UNTERSTRICH): retVal = "_" ;break;
    case static_cast<qint32>(FILEPARTSDELIM::EMPTY): retVal = "";break;
    }
    return retVal;
}

// make file name from selected categories
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
    case static_cast<qint32>(FILESPACECHAR::UNDERSCORE): filler = "_"; break;
    case static_cast<qint32>(FILESPACECHAR::ADD): filler = "+"; break;
    case static_cast<qint32>(FILESPACECHAR::SUB): filler = "-"; break;
    case static_cast<qint32>(FILESPACECHAR::ORIGINAL): filler = " "; break;
    case static_cast<qint32>(FILESPACECHAR::DELETE): filler = ""; break;
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

    // check filename for a valid filename for this platform
    if( !isValidFilename(fn) ){
#ifdef Q_OS_OSX        
        QMessageBox::warning(this,"Achtung","Der gewählte Filename kann zu Problemen auf anderen Plattform führen!", QMessageBox::Ok);
#else
        QMessageBox::warning(this,"Achtung","Der gewählte Filename wird zu Problemen auf dieser Plattform führen!", QMessageBox::Ok);
#endif
    }
    
    ui->path->setText(fn);
    ui->saveFile->setEnabled(true);
    this->fileName=fn;
    return fn;
}

bool MainWindow::isValidFilename(QString fn){
    bool isLegal=true;
    QString illegal="<>:\"|?*";
    // Windows filenames are not case sensitive.
    fn = fn.toUpper();
    foreach (const QChar& c, fn)
    {
        // Check for control characters
        if (c.toLatin1() > 0 && c.toLatin1() < 32)
            isLegal = false;
    
        // Check for illegal characters
        if (illegal.contains(c))
            isLegal = false;
    }
    // Check for device names in filenames
    QStringList devices;
    devices << "CON" << "PRN" << "AUX" << "NUL" << "COM0" << "COM1" << "COM2"
            << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "LPT0"
            << "LPT1" << "LPT2" << "LPT3" << "LPT4" << "LPT5" << "LPT6" << "LPT7" << "LPT8"
            << "LPT9";
    QFileInfo fi(fn);
    QString basename = fi.baseName();
    foreach (const QString& s, devices){
        if (basename == s){
            isLegal = false;
        }
    }
    // Check for trailing periods or spaces
    if (fn.right(1)=="." || fn.right(1)==" "){
        isLegal = false;
    }
    // Check for pathnames that are too long (disregarding raw pathnames)
    if (fn.length()>260){
        isLegal = false;
    }
    // Exclude raw device names
    if (fn.left(4)=="\\\\.\\"){
        isLegal = false;
    }
    // Since we are checking for a filename, it mustn't be a directory
    if (fn.right(1)=="\\"){
        isLegal = false;
    }
    return isLegal;
}

void MainWindow::on_saveFile_clicked()
{
    if(checkModel()){
        QString filepath="";
        filepath = ui->folder->text() + QDir::separator() + fileName;
        QJsonObject json = packQJD();
        saveJson(json,filepath);
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
        //QString name = QVariant(ui->tableView->model()->index(row,0,start).data(Qt::ItemIsEditable)).toString();
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
                    break;
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
                    break;
                default:
                    break;
                }
            }
        }
    }
    ui->saveFile->setEnabled(true);
}

void MainWindow::insertPrueferIntoModel(QVariantList qvl){
   // suchen nach Namen im entsprechenden Zweig
    QModelIndex child;
    bool found = false;
    QModelIndex parent = ui->comboBoxExam_2->rootModelIndex();
    qint32 iAusschuss = ui->comboBoxExam_2->currentIndex();
    QModelIndex start = ui->tableView->model()->index(iAusschuss,0,parent);
    for( int row = 0; row < ui->tableView->model()->rowCount(start); ++row ) {
        QString name = QVariant(ui->tableView->model()->index(row,0,start).data(Qt::ItemIsEditable)).toString();
        if(name.compare(qvl.at(0).toString())==0){
            found = true;
            // Name vorhanden in allen 3 Spalten Checkstats setzen
            for ( int col = 1; col < ui->tableView->model()->columnCount(start); ++col ) {
                treeModel->setData(ui->tableView->model()->index(row,col,start),qvl.at(col),Qt::CheckStateRole);
            }
            break; // Name gefunden. Schleife kann verlassen werden
        }
    }
    if(found==false){
        QString msg = "Der Prüfer "+qvl.at(0).toString()+" aus der Datei ist noch nicht vorhanden\nSoll der neue Prüfer zugefügt werden?";
        QMessageBox::StandardButton reply = QMessageBox::warning(this,"Prüfer nicht vorhande",msg,
                             QMessageBox::Yes|QMessageBox::No,QMessageBox::No); 
        if(reply == QMessageBox::Yes){
            // Einfügen eines neuen Users in das Model
            const QModelIndex index = start;

            if (treeModel->columnCount(index) == 0) {
                if (!treeModel->insertColumn(0, index))
                    return;
            }
            if (!treeModel->insertRow(0, index))
                return;
            child = treeModel->index(0, 0, index);
            treeModel->setData(child, qvl.at(0), Qt::EditRole);
            child = treeModel->index(0, 1, index);
            treeModel->setData(child, qvl.at(1), Qt::CheckStateRole);
            child = treeModel->index(0, 2, index);
            treeModel->setData(child, qvl.at(2), Qt::CheckStateRole);
            child = treeModel->index(0, 3, index);
            treeModel->setData(child, qvl.at(3), Qt::CheckStateRole);
        }
    }
    ui->saveFile->setEnabled(true);
}

void MainWindow::on_actionQuit_2_triggered()
{
    // Falls noch aufzuräumen ist....wäre das dann hier.
    QCoreApplication::quit();
}

void MainWindow::saveSettings(bool withModel){
    settings.clear();
    if(withModel){
        // save the Model
        settings.beginGroup("/Model");
            saveTreeQsettings(QModelIndex(),treeModel);
        settings.endGroup();
        settings.beginGroup("/ModelDefaultSelection");
            settings.setValue("Fachrichtung",QString::number(ui->comboBoxExam->currentIndex()) );
            settings.setValue("Pruefungsausschuss",QString::number(ui->comboBoxExam_2->currentIndex()));
        settings.endGroup();        
    }
    
    // Save other settings
    settings.setValue("folderPath",ui->folder->text());
    settings.beginGroup("/Examination");
        settings.setValue("maxMinutes",QString::number(this->maxMinutes));
    settings.endGroup();
    settings.beginGroup("/FilenamePattern");
        settings.setValue("d1",QString::number(mypref->d1()));
        settings.setValue("d2",QString::number(mypref->d2()));
        settings.setValue("d3",QString::number(mypref->d3()));
        settings.setValue("t1",QString::number(mypref->t1()));
        settings.setValue("t2",QString::number(mypref->t2()));        
        settings.setValue("space",QString::number(mypref->space()));
    settings.endGroup();
    settings.sync();
}

void MainWindow::recurseGroups(QString group,QString tab, QString &lines){
    QStringList g1;
    QString str="";
    QString aLine="";
    tab += "    ";
    settings.beginGroup(group);
        g1 = settings.childGroups();
        // erstmalrekursiv in die einzelnen Gruppen...
        if(g1.length() > 0){
            for(int i=0; i<g1.length();i++){
                str = (tab+ g1[i].trimmed() + "\n");
                lines.append(str);
                recurseGroups(g1[i],tab,lines);
            }
        }
        g1.clear();
        // ...dann alle keys einsammeln
        g1 = settings.childKeys();
        if(g1.length()> 0){
            for(int i=0; i<g1.length();i++){
                aLine = tab + g1[i].trimmed() + "\n";
                lines.append(aLine);
            }
        }
    settings.endGroup();
}

// Liest Konfigurationsdaten aus den Registry/plist Daten
void MainWindow::loadSettings(bool withModel,QStringList headers){
    QByteArray larr;
    QString lines="";
    QStringList linesList;
    // Zunächst nur die Einstellungsparameter lesen...
    ui->folder->setText(settings.value("folderPath",QDir::homePath()).toString());
    settings.beginGroup("/Examination"); 
        maxMinutes = settings.value("maxMinutes","15").toInt();
        mypref->setMinutes(maxMinutes);
        ui->label_minutes->setText(QString::number(mypref->minutes())+ "min");
    settings.endGroup();
    settings.beginGroup("/FilenamePattern");
        mypref->setD1(settings.value("d1","0").toInt());
        mypref->setD2(settings.value("d2","1").toInt());
        mypref->setD3(settings.value("d3","2").toInt());
        mypref->setT1(settings.value("t1","0").toInt());
        mypref->setT2(settings.value("t2","0").toInt());
        mypref->setSpace(QVariant(settings.value("space","0")).toInt());
    settings.endGroup();
    // jetzt versuchen, die Prüfer einzulesen... 
    if(withModel){
        recurseGroups("/Model","", lines);
        if(lines.length()>1){
            // im 3. Level die 0en für Spalten hinzufügen
            linesList.append(lines.split('\n'));
            for(int i=0;i<linesList.length();i++){
                // die ersten 4 spaces jeder Zeile löschen
                if(linesList[i].startsWith("    ")){
                    linesList[i] = linesList[i].remove(0,4);
                }
                else{
                    linesList[i] = linesList[i];
                }
                // Einrückungen für Prüfer erkannt, Scheckstastes intialisieren
                if(linesList[i].startsWith("        ") && !linesList[i].startsWith("            ")){
                    linesList[i].append("\t0\t0\t0");                
                }
                larr.append((linesList[i]+"\n").toUtf8());
            }
            treeModel = new TreeModel(headers,larr);
            ui->tableView->setModel(treeModel);
            ui->comboBoxExam->setModel(treeModel);
            ui->comboBoxExam_2->setModel(treeModel);
        }
        else{ //Nothing found in QSettings...trying to read in basic Tester model for treeModel
            QFile file(":/tree.txt");
            file.open(QIODevice::ReadOnly);
            treeModel = new TreeModel(headers, file.readAll());
            file.close();
            ui->tableView->setModel(treeModel);
            ui->comboBoxExam->setModel(treeModel);
            ui->comboBoxExam_2->setModel(treeModel);   
        }     
        // Default indizes wählen für Comboboxen.... 
        settings.beginGroup("/ModelDefaultSelection");
            ui->comboBoxExam->setCurrentIndex(settings.value("Fachrichtung","0").toInt());
            ui->comboBoxExam_2->setCurrentIndex(settings.value("Pruefungsausschuss","0").toInt());
        settings.endGroup();
    }
    ui->progressBar->setMaximum(maxMinutes);
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, APP_NAME,
                                                                tr("Wollen Sie vorher die Einstellungen sichern?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        saveSettings(true);
        event->accept();
    } else if (resBtn == QMessageBox::No)  {
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::on_actionAusgabeblatt_triggered()
{
    Ihk *ihk = new Ihk(this);
    ihk->exec(); 
}

qint32 MainWindow::t11(){
    return (ui->spinboxGa0->text().toInt());
}

qint32 MainWindow::t21(qint32 exam){    // wenn ohne exam aufgerufen: Punkte aus GUI-Werten
    if(exam==-1){
        return (qRound((ui->spinboxDocumentation->text().toDouble() + ui->spinboxExamination->text().toDouble())/2.0));
    }else{
        return (qRound((ui->spinboxDocumentation->text().toDouble() + exam)/2.0));
    }
}

qint32 MainWindow::t22(qint32 mueergpr){ // ohne argumente keine Muendliche Pruefung
    qint32 retVal=0;
    if(mueergpr !=  -1){
        retVal = qRound((ui->spinboxGa1->text().toDouble()*2.0 + mueergpr)/3.0);
    }
    else{
        retVal = ui->spinboxGa1->text().toInt();
    }
    return retVal;
}

qint32 MainWindow::t23(qint32 mueergpr){// ohne argumente keine Muendliche Pruefung
    qint32 retVal=0;
    if(mueergpr !=  -1){
        retVal = qRound((ui->spinboxGa2->text().toDouble()*2.0 + mueergpr)/3.0);
    }
    else{
        retVal = qRound(ui->spinboxGa2->text().toDouble());
    }
    return retVal;
}

qint32 MainWindow::t24(qint32 mueergpr){
    qint32 retVal=0;
    if(mueergpr !=  -1){
        retVal = qRound((ui->spinboxWiso->text().toDouble()*2.0 + mueergpr)/3.0);
    }
    else{
        retVal = ui->spinboxWiso->text().toInt();
    }
    return retVal;
}

qint32 MainWindow::t2(qint32 nr, qint32 mueergpr){    // mit mündliche: nr=1,2,3   ohne: nr=0
    qint32 result=0;
    switch(nr){
    case 0: result = qRound((t21()+t22()+t23()+t24())/4.0);
        break;
    case 1: result = qRound((t21()+t22(mueergpr)+t23()+t24())/4.0);
        break;
    case 2: result = qRound((t21()+t22()+t23(mueergpr)+t24())/4.0);
        break;
    case 3: result = qRound((t21()+t22()+t23()+t24(mueergpr))/4.0);
        break;
    default:result = qRound((t21()+t22()+t23()+t24())/4.0);
        break;
    }
    return result;
}

void MainWindow::saveTreeQsettings(const QModelIndex & index, const QAbstractItemModel * model,QString str)
{
     if (index.isValid()){
        str+=(model->data(index).toString()).trimmed();
        str+="/";      
     }
     if (!model->hasChildren(index) || (index.flags() & Qt::ItemNeverHasChildren)){
          settings.setValue(str,"x");
          return;
     }
     auto rows = model->rowCount(index);
     for (int i = 0; i < rows; ++i){
         saveTreeQsettings(model->index(i, 0, index), model,str);
     }
}

void MainWindow::colorLabel(QLabel *label, qint32 points){
    label->setStyleSheet("QLabel { color : red; }");
    if(points>=30){
        label->setStyleSheet("QLabel { color : orange; }");
    }
    if(points>=50){
        label->setStyleSheet("QLabel { color : green; }");
    }
    if(points>=67){
        label->setStyleSheet("QLabel { color : green; }");
    }
    if(points>=81){
        label->setStyleSheet("QLabel { color : green; }");
    }
    if(points>=92){
        label->setStyleSheet("QLabel { color : green; }");
    }
}

void MainWindow::on_buttonSimPRFG_clicked()
{
    fillPRFG();
}


void MainWindow::on_buttonSimMEPR_clicked()
{
    fillMEPR();
}

QString MainWindow::getBuildDate(){
    QString macroDate = __DATE__;
    QString buildDate;
    QStringList months = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    QString day = macroDate.mid(4,2);
    QString month="00";
    QString year = macroDate.mid(7,4);

    for (int i = 0; i < 12; i++){
        if(months[i].compare(macroDate.mid(0,3)) == 0){
            month = QString("%1").arg(i+1,2,10,QLatin1Char('0'));
        }
    }
    buildDate = day+"."+month+"."+year;
    return buildDate;
}


void MainWindow::on_actionBericht_triggered()
{
    QString title = "IHK-Prüfungen";
    QString dirPath=ui->folder->text();
    QString filePath="";
    QStringList nameFilter("*.json");
    QDir directory(dirPath);
    QStringList jsonFilesAndDirectories = directory.entryList(nameFilter);
    
    
    QString f = QFileDialog::getSaveFileName(this,tr("Sichern"),dirPath + "/Report.pdf",tr("PDF (*.pdf)"));
    if(f.length() == 0){
        return;
    }

    QPdfWriter pdfwriter(f);
    pdfwriter.setPageSize(QPageSize(QPageSize::A4));
    pdfwriter.setTitle(title);
    pdfwriter.setCreator("zenmeister");
    pdfwriter.setResolution(300);

    
    QPainter painter(&pdfwriter);
    reportHeadFoot(painter, title); 
    qint32 skip = 8;
    
    for(int i=0, k=0; i<jsonFilesAndDirectories.length();i++,k++){
        filePath = jsonFilesAndDirectories[i];
        if(QFileInfo(filePath).isDir()){
            continue;
        }
        QString absFilePath= dirPath + QDir::separator() + filePath;
        
        QString fileContent;
        QFile file;
        file.setFileName(absFilePath);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        fileContent = file.readAll();
        file.close();
        
        QJsonDocument d = QJsonDocument::fromJson(fileContent.toUtf8());
        QJsonObject jo = d.object();
        QString name = jo["Name"].toString();
        QString pe = jo["Prüfungsergebnis"].toString();      
        QString datum = jo["Datum"].toString();      
        QString ergebnis = jo["Ergebnis"].toString();      
        QString ergebnisb = jo["Ergebnis B"].toString();      
        QString idnr = jo["Id-Nummer"].toString();
        QString gb = QString::number(qRound((jo["Doku"].toDouble()+jo["PRFG"].toDouble())/2)); 
        QString ga0 = jo["GA0"].toString();
        QString ga1 = jo["GA1"].toString();
        QString ga2 = jo["GA2"].toString();
        QString mepga1 = jo["MEP-GA1"].toString();
        QString mepga2 = jo["MEP-GA2"].toString();
        QString mepwiso = jo["MEP-WISO"].toString();
        QString wiso = jo["Wiso"].toString();
        QString line1 = QString("%1: %2").arg(i+1,3).arg((name.trimmed()+"/"+idnr.trimmed()),-35);
        QString line2 = QString("%1").arg(pe,-16);
        QString line3 = QString("%1").arg(datum,-10);
        QString line4 = QString("T2=%1").arg(ergebnisb,-16);
        QString line5 = QString("Gesamt=%1").arg(ergebnis,-16);
        QString mep = "";
        if(mepga1.compare("0")!=0)
            mep = QString("MEP-GA1=%1").arg(mepga1);
        if(mepga2.compare("0")!=0)
            mep = QString("MEP-GA2=%1").arg(mepga2);
        if(mepwiso.compare("0")!=0)
            mep = QString("MEP-WISO=%1").arg(mepwiso);
        QString line6 = QString("T1=%1  T21=%2  T22=%3  T23=%4  T24=%5  %6")
                .arg(ga0,-2).arg(gb,-2).arg(ga1,-2).arg(ga2,-2).arg(wiso,-2).arg(mep,-12);
        painter.drawText(pos(0,(k+1)*skip),line1); // Name Nummer
        painter.drawText(pos(65,(k+1)*skip),line2); // Bestanden
        painter.drawText(pos(95,(k+1)*skip),line3); // Datum
        painter.drawText(pos(115,(k+1)*skip),line4); // 1
        painter.drawText(pos(151,(k+1)*skip),line5); // 2
        painter.setFont(QFont("times",9));
        painter.drawText(pos(115,(k+1+0.3)*skip),line6);
        painter.setFont(QFont("times",11));
        
        if((k+1)*skip > 170){
            pdfwriter.newPage();
            reportHeadFoot(painter,title);
            k=-1;
        }
//        QJsonValue value = sett2.value(QString("appName"));
//        qWarning() << value;
//        QJsonObject item = value.toObject();
//        qWarning() << tr("QJsonObject of description: ") << item;
  
//        /* in case of string value get value and convert into string*/
//        qWarning() << tr("QJsonObject[appName] of description: ") << item["description"];
//        QJsonValue subobj = item["description"];
//        qWarning() << subobj.toString();
  
//        /* in case of array get array and convert into string*/
//        qWarning() << tr("QJsonObject[appName] of value: ") << item["imp"];
//        QJsonArray test = item["imp"].toArray();
//        qWarning() << test[1].toString();
    }
    painter.end();
    
    QMessageBox::information(this,"Information",QString("%1 erstellt").arg(f));
}

QPoint MainWindow::pos(double x, double y){
    qint32 pRight = 2478; // 300*8.26
    qint32 pBottom = 3507; // 300*11.69
    double lMargin = 5; // mm
    double tMargin = 10; // mm
    double myX = 210.0; // mm
    
    QPoint pos = QPoint(static_cast<qint32>( pRight * ((x+lMargin)/myX)),
                        static_cast<qint32>(pBottom * ((y+tMargin)/myX)));
    return pos;
}

void MainWindow::reportHeadFoot(QPainter &p, QString title){
    p.setFont(QFont("times",20));
    p.drawText(pos(75,0),title);
    p.setFont(QFont("times",11));
    p.drawLine(pos(0,0.1),pos(1000,0));
    p.drawLine(pos(0,190),pos(1000,190));

    p.setFont(QFont("times",8));
    p.drawText(pos(0,192),app.versionLong);
    p.setFont(QFont("times",11));
    
}

