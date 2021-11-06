#include "preferences.h"
#include "ui_preferences.h"
#include "treeitem.h"
#include "treemodel.h"
#include "prefs.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    w = qobject_cast<MainWindow*>(parent);

    ui->setupUi(this);
    
    //TreeModel* oldModel;
    //oldModel = w->treeModel;    // noch nicht ausprogrammiert TODO
    //copy(oldModel,w->treeModel);    // leider kein deepcopy: TODO
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setModel(w->treeModel);
//    for (int column = 0; column < (w->treeModel)->columnCount(); ++column){
//        ui->treeView->resizeColumnToContents(column);
//    }
    //ui->treeView->setColumnWidth(0,351);  // alles dahinter wollen wir nicht sehen
    ui->treeView->setColumnHidden(1,true);
    ui->treeView->setColumnHidden(2,true);
    ui->treeView->setColumnHidden(3,true);
    ui->treeView->horizontalScrollBar()->setDisabled(true);  // und der user soll sich das nicht zurechtschieben können

    ui->comboBoxD1->setCurrentIndex(w->mypref->d1());
    ui->comboBoxD2->setCurrentIndex(w->mypref->d2());
    ui->comboBoxD3->setCurrentIndex(w->mypref->d3());
    ui->comboBoxT1->setCurrentIndex(w->mypref->t1());
    ui->comboBoxT2->setCurrentIndex(w->mypref->t2());
    ui->comboBoxSpacher->setCurrentIndex(w->mypref->space());
    ui->spinBoxMinutes->setValue(w->mypref->minutes());
}

Preferences::~Preferences()
{
    delete ui;
}

//void Preferences::copy(QStandardItemModel* from, QStandardItemModel* to)
//{
//   to->clear();
//   for (int i = 0 ; i < from->rowCount() ; i++)
//   {
//      to->appendRow(from->item(i)->clone());
//   }
//}

void Preferences::on_treeView_customContextMenuRequested(const QPoint &pos)
{   // the context menu
    QMenu menu(this); // add menu items
    menu.addAction(ui->actionDelete);
    menu.addAction(ui->actionInsertRow);
    menu.addAction(ui->actionInsertChild);

    //ui->actionDelete->setData(QVariant(pos)); // if you will need the position data save it to the action
    menu.exec( ui->treeView->mapToGlobal(pos) );
}


void Preferences::on_actionDelete_triggered()
{
    removeRow();
}

void Preferences::on_actionInsertRow_triggered()
{
    insertRow();
}

void Preferences::on_actionInsertChild_triggered()
{
    insertChild();
}

// we dont use
void Preferences::removeRow()
{
    const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void Preferences::insertRow()
{
    const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        const QModelIndex child = model->index(index.row() + 1, column, index.parent());
        if(column==0){
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
        }
        else{
            model->setData(child, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

void Preferences::insertChild()
{
    const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        const QModelIndex child = model->index(0, column, index);
        if(column==0){
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);

        }
        else{
            model->setData(child, Qt::Unchecked, Qt::CheckStateRole);
        }
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
    }

    ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

void Preferences::updateActions()
{
//    const bool hasSelection = !ui->treeView->selectionModel()->selection().isEmpty();
//    removeRowAction->setEnabled(hasSelection);
//    removeColumnAction->setEnabled(hasSelection);

    const bool hasCurrent = ui->treeView->selectionModel()->currentIndex().isValid();
//    insertRowAction->setEnabled(hasCurrent);
//    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        ui->treeView->closePersistentEditor(ui->treeView->selectionModel()->currentIndex());
    }
}

void Preferences::on_buttonBox_accepted()
{
    //w->treeModel = oldModel;
    // treeModel sichern
    // Dateimuster sichern
    // position sichern
    // Minuten sichern
}


void Preferences::on_comboBoxD1_currentIndexChanged(int index)
{
    w->mypref->setD1(index);
}


void Preferences::on_comboBoxD2_currentIndexChanged(int index)
{
    w->mypref->setD2(index);
}


void Preferences::on_comboBoxD3_currentIndexChanged(int index)
{
    w->mypref->setD3(index);
}


void Preferences::on_comboBoxT1_currentIndexChanged(int index)
{
    w->mypref->setT1(index);
}


void Preferences::on_comboBoxT2_currentIndexChanged(int index)
{
    w->mypref->setT2(index);
}


void Preferences::on_comboBoxSpacher_currentIndexChanged(int index)
{
    w->mypref->setSpace(index);
}


void Preferences::on_spinBoxMinutes_valueChanged(int arg1)
{
    w->mypref->setMinutes(arg1);
}

