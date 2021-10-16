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

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setModel(w->treeModel);
//    for (int column = 0; column < (w->treeModel)->columnCount(); ++column){
//        ui->treeView->resizeColumnToContents(column);
//    }
    ui->treeView->setColumnWidth(0,351);
    ui->treeView->horizontalScrollBar()->setDisabled(true);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    qDebug()<< pos.x() << pos.y();
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
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
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
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
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






