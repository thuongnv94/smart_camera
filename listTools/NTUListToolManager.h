#ifndef NTULISTTOOLMANAGER_H
#define NTULISTTOOLMANAGER_H
#include <QDebug>
#include <QObject>

#include "ItemTool/NTUItemTool.h"
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QStandardItemModel>

class NTUListToolItem;

class NTUListToolManager : public QStandardItemModel
{
  Q_OBJECT
public:
  NTUListToolManager(QObject* parent = nullptr);

  void addNewTools(QStandardItem* item);
  QItemSelectionModel* getItemSelectionModel() { return mSelectionModel; }

Q_SIGNALS:
  void newSelectionIndexAvailable(bool value);
  void newCreateToolFromIndexSelected(NTUTOOLS type);
public Q_SLOTS:
  void itemSelectionChanged(const QItemSelection&, const QItemSelection&);
  void onReceiverCreaeToolFromIndexSelected();

private:
  void inserNewItem(NTUListToolItem* newTool, QStandardItem* parentItem);

  ///
  /// \brief onlyNTUListItemSelected
  /// \return
  ///
  bool onlyNTUListItemSelected();

  QItemSelectionModel* mSelectionModel;
  QModelIndexList mSelectionIndexes;
};

#endif // NTULISTTOOLMANAGER_H
