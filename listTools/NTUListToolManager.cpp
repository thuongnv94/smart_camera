#include "NTUListToolManager.h"
#include "NTUListToolItem.h"
NTUListToolManager::NTUListToolManager(QObject* parent) : QStandardItemModel(parent)
{
  //! [2] set header name
  // If it hasn't been set yet
  if (!horizontalHeaderItem(0))
    setHorizontalHeaderItem(0, new QStandardItem("Tools"));
  else
  {
    QStandardItem* header = horizontalHeaderItem(0);
    header->setText("Tools");
  }
  //!
  mSelectionModel = new QItemSelectionModel(this);
  connect(mSelectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this,
          SLOT(itemSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

void NTUListToolManager::addNewTools(QStandardItem* item)
{
  this->inserNewItem(dynamic_cast<NTUListToolItem*>(item), this->invisibleRootItem());
}

void NTUListToolManager::itemSelectionChanged(const QItemSelection&, const QItemSelection&)
{
  if (onlyNTUListItemSelected())
    {
    Q_EMIT newSelectionIndexAvailable(mSelectionModel->hasSelection());
    QModelIndexList selected_indexes = mSelectionModel->selectedIndexes();
  }
  else
    Q_EMIT newSelectionIndexAvailable(false);
}

void NTUListToolManager::onReceiverCreaeToolFromIndexSelected()
{
  if (onlyNTUListItemSelected())
  {
    QModelIndexList selected_indexes = mSelectionModel->selectedIndexes();
    NTUTOOLS type = dynamic_cast<NTUListToolItem*>(this->item(selected_indexes[0].row(), 0))->mType;
    Q_EMIT newCreateToolFromIndexSelected(type);
  }


}

void NTUListToolManager::inserNewItem(NTUListToolItem* newTool, QStandardItem* parentItem) { parentItem->appendRow(newTool); }

bool NTUListToolManager::onlyNTUListItemSelected()
{

  QModelIndexList selected_indexes = mSelectionModel->selectedIndexes();

  if (selected_indexes.size() <= 0 || selected_indexes.size() > 1)
    return false;

  return true;
}
