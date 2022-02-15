#ifndef NTUPROJECTMANAGER_H
#define NTUPROJECTMANAGER_H

#include <QItemSelectionModel>
#include <QObject>
#include <QStandardItemModel>
#include <QThread>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

#include "ItemTool/NTUItemTool.h"
#include "ItemTool/NTUToolWidget.h"

class SMVideoZoomWidget;
class NTUProjectManager : public QStandardItemModel
{
  Q_OBJECT
public:
  NTUProjectManager(QObject* parent = nullptr, QWidget* areaWidget = nullptr, QWidget* videoWidget = nullptr, QGridLayout* layout = nullptr,
                    QString projectName = "Untitled");

  inline void setProjectName(QString projectName);

  QItemSelectionModel* getItemSelectionModel(){return  mSelectionModel;}

  virtual ~NTUProjectManager();

  Q_SIGNALS:
      void newClearVBox();
      void newDrawResult();

public Q_SLOTS:

  void onOpenProjectFromFileName(QString filename);

  void insertNewTool(NTUTOOLS tool);
  void itemSelectionChanged(const QItemSelection&, const QItemSelection&);
  void clearWidgetInVBox();
  void onReceiverItemFromClick(QModelIndex index);
  void onRunProject();
  void onReceiverFixtureTools(NTUTools* tool, QString fixtureName);
  void onReceiverSaveProject(QString fileName);
  void onReceiverDeleteFromSelectedIndex(QModelIndex index);

private:
    ///
    /// \brief inserNewItem
    /// \param newTool
    /// \param parentItem
    ///
  void inserNewItem(NTUItemTool* newTool, QStandardItem* parentItem);

  ///
  /// \brief findnameDuplicate
  /// \param name
  /// \return
///
  QString findnameDuplicate(QString name);

  ///
  /// \brief getFixtureTools
  /// \return
///
  QVector<QString> getFixtureToolsName();

  SMVideoZoomWidget* mVideoWidget;
  QWidget* mParameterAreaWidget;
  QGridLayout* mLayout;
  QItemSelectionModel* mSelectionModel;
  QModelIndexList mSelectedIndexes;
  QString mProjectName;
  QVector<NTUToolWidget*> mToolManager;
};

#endif // NTUPROJECTMANAGER_H
