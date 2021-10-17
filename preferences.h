#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QFile>
#include <QMenu>
#include "mainwindow.h"
//#include <QStandardItemModel>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();

private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_actionDelete_triggered();
    void on_actionInsertRow_triggered();
    void on_actionInsertChild_triggered();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    
private:
    Ui::Preferences *ui;
    MainWindow *w;
    TreeModel *oldModel;
//    void copy(QStandardItemModel*,QStandardItemModel*);
    void removeRow();
    void insertRow();
    void insertChild();
    void updateActions();
};

#endif // PREFERENCES_H
