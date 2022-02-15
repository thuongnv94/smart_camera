#include "NTUListToolBrowser.h"
#include "NTUListToolManager.h"

NTUListToolBrowser::NTUListToolBrowser(QWidget *parent):QTreeView(parent)
{
    this->setSortingEnabled(true);
    //this->sortByColumn(0, Qt::AscendingOrder);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void NTUListToolBrowser::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    this->mCurrentListToolManager = dynamic_cast<NTUListToolManager*>(model);
    this->setSelectionModel(mCurrentListToolManager->getItemSelectionModel());
}

void NTUListToolBrowser::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());

    if (item.isValid())
    {
        QTreeView::mousePressEvent(event);
    }
    else
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }
    QTreeView::mousePressEvent(event);
}

//void NTUListToolBrowser::mouseDoubleClickEvent(QMouseEvent *event)
//{

//}

