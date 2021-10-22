#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QColor>
#include <QPalette>
#include <QDir>
#include <math.h>
#include <QLabel>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringListModel>
#include <QFont>
#include <QWindow>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QFileDialog>
#include <QDate>
#include <QTextBrowser>
#include <QSettings>
#include <treemodel.h>
#include <QCheckBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <prefs.h>

namespace Ui {
class MainWindow;
}

struct appInfo{
    QString version;
    QString versionLong;
    QString date;
    QString name;
    QString author;
    QString email;
};
typedef appInfo APPINFO;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    APPINFO app;
    TreeModel *treeModel;
    Prefs *mypref;

private:
    Ui::MainWindow *ui;

    QString fileName;
    bool isTimerStarted;    // state of timer
    int timerValue;         // timer value in minutes
    int offset;             // 15, after that time the excess is counted
    QLabel *statusLabel;    // line in statusbar
    bool hasPassed;         // Bestanden=true, Nicht bestanden=false
    QStringListModel *model;
    qint32 maxMinutes;      // maximale Vortragszeit

    quint32 calcA(qint32 docu, qint32 exam);
    quint32 calcB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 epnr=0, quint32 mueergpr=0);
    quint32 calcAll(quint32 pointsA, quint32 pointsB);
    QString getGrade(qint32 points);
    bool checkModel();
    QTimer *timer;

    bool checkPassedA(quint32 docu, quint32 exam);     // checks if passed
    bool checkPassedB(quint32 ga1, quint32 ga2, quint32 wiso,quint32 nr=0,quint32 points=0); // checks if passed
    bool checkMAllowed(quint32 ga1, quint32 ga2, quint32 wiso); // checks if oral is possible

    void unpackQJO(QJsonObject json);
    QJsonObject packQJD();
    QJsonObject loadJson(QString fileName);
    void saveJson(QJsonObject json, QString fileName);
    void clearModelCheckboxes(bool all);
    QList<QVariantList> readPruefer(QJsonObject json);
    void insertPrueferIntoModel(QVariantList qvl);
    
private slots:
    
    // timer
    void toggleStartStop();     // for timer control
    void updateProgressBar();   // progress bar
    void timerReset();
    // GUI
    QString makeFilename();
    void makeNewTimer();
    QString makeFilePart(qint32 index, QString filler);
    QString makeFileDelim(qint32 index);
    void writeResults();
    void fillPRFG();
    void fillMEPR();
    void setPointsPRFG(const QModelIndex &index);
    void setPointsMEPR(const QModelIndex &index);
    void about();
    void on_actionQuit_triggered();
    void on_actionOeffnen_triggered();
    void on_actionSichernAls_triggered();
    void on_pushButton_DeleteAll_clicked();
    void on_actionPreferences_triggered();
    void on_pushButton_SelectDir_clicked();
    void on_actionLizenz_triggered();
    void on_actionRegularien_triggered();
    void on_comboBoxExam_currentIndexChanged(int index);
    void on_comboBoxExam_2_currentIndexChanged(int index);
    void on_tableView_clicked(const QModelIndex &index);
    void on_saveFile_clicked();
    void on_actionSichern_triggered();
};

#endif // MAINWINDOW_H
