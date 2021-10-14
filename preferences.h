#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QFile>
#include <QMenu>
#include "mainwindow.h"

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

private:
    Ui::Preferences *ui;
    MainWindow *w;
    void removeRow();
    void insertRow();
    void insertChild();
    void updateActions();
};

#endif // PREFERENCES_H
