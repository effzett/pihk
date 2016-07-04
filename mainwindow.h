#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QColor>
#include <QPalette>
#include <QDir>
#include <math.h>
#include <Qlabel.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringListModel>
#include <QFont>
#include <QWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

bool isTimerStarted;    // state of timer
int timerValue;         // timer value in minutes
int offset;             // 15, after that time the excess is counted
QLabel *statusLabel;    // line in statusbar
bool hasPassed;         // Bestanden=true, Nicht bestanden=false
QStringListModel *model;

quint32 calcA(qint32 docu, qint32 exam);
quint32 calcB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 epnr=0, quint32 mueergpr=0);
quint32 calcAll(quint32 pointsA, quint32 pointsB);
QString getGrade(qint32 points);

QTimer *timer;

bool checkPassedA(quint32 docu, quint32 exam);     // checks if passed
bool checkPassedB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 nr=0,quint32 points=0); // checks if passed
bool checkMAllowed(quint32 ga1, quint32 ga2, quint32 wiso); // checks if oral is possible

private slots:
    // timer
    void toggleStartStop();     // for timer control
    void updateProgressBar();   // progress bar
    void timerReset();
    // GUI
    void makeFilename();
    void writeResults();
    void saveData();
    void fillPRFG();
    void fillMEPR();
    void setPointsPRFG(const QModelIndex &index);
    void setPointsMEPR(const QModelIndex &index);
};

#endif // MAINWINDOW_H
