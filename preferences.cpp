#include "preferences.h"
#include "ui_preferences.h"
#include "treeitem.h"
#include "treemodel.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    w = qobject_cast<MainWindow*>(parent);

    ui->setupUi(this);
    
    TreeModel* oldModel;
    oldModel = w->treeModel;
    //copy(oldModel,w->treeModel);    // leider kein deepcopy: TODO
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setModel(oldModel);
//    for (int column = 0; column < (w->treeModel)->columnCount(); ++column){
//        ui->treeView->resizeColumnToContents(column);
//    }
    //ui->treeView->setColumnWidth(0,351);  // alles dahinter wollen wir nicht sehen
    ui->treeView->setColumnHidden(1,true);
    ui->treeView->setColumnHidden(2,true);
    ui->treeView->setColumnHidden(3,true);
    ui->treeView->horizontalScrollBar()->setDisabled(true);  // und der user soll sich das nicht zurechtschieben können
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
            qDebug()<<tr("[No data]")<<"insert Row-Preferences";
        }
        else{
            model->setData(child, Qt::Checked, Qt::CheckStateRole);
            qDebug()<<"Qt::Checked"<<"insert Row-Preferences";
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
            qDebug()<<tr("[No data]")<<"insert Child-Preferences";

        }
        else{
            model->setData(child, Qt::Unchecked, Qt::CheckStateRole);
            qDebug()<<"Qt::Checked"<<"insert Child-Preferences";
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

//        const int row = ui->treeView->selectionModel()->currentIndex().row();
//        const int column = ui->treeView->selectionModel()->currentIndex().column();
//        if (ui->treeView->selectionModel()->currentIndex().parent().isValid())
//            qDebug()<<(tr("Position: (%1,%2)").arg(row).arg(column));
//        else
//            qDebug()<<(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void Preferences::on_buttonBox_accepted()
{
    w->treeModel = oldModel;
}


void Preferences::on_buttonBox_rejected()
{
    
}

