#include "NTUProjectManager.h"
#include "ItemTool/NTUBlobWidget.h"
#include "ItemTool/NTUCircleDetectWidget.h"
#include "ItemTool/NTUPatMaxWidget.h"
#include "ItemTool/NTUPatternWidget.h"
#include "ItemTool/NTUColorWidget.h"
#include "SMVideoZoomWidget.h"

NTUProjectManager::NTUProjectManager(QObject* parent, QWidget* areaWidget, QWidget* videoWidget, QGridLayout* layout, QString projectName)
    : QStandardItemModel(parent), mProjectName(projectName)
{
  this->mParameterAreaWidget = dynamic_cast<QWidget*>(areaWidget);
  this->mVideoWidget = dynamic_cast<SMVideoZoomWidget*>(videoWidget);
  this->mLayout = dynamic_cast<QGridLayout*>(layout);

  mSelectionModel = new QItemSelectionModel(this);

  //! [2] set header name
  if (!horizontalHeaderItem(0))
  {
    QStandardItem* header = new QStandardItem();
    header->setIcon(QIcon(":/Icons/arrows.svg"));
    setHorizontalHeaderItem(0, header);
  }
  else
  {
    QStandardItem* header = horizontalHeaderItem(0);
    header->setIcon(QIcon(":/Icons/arrows.svg"));
  }

  if (!horizontalHeaderItem(1))
  {
    QStandardItem* header = new QStandardItem();
    header->setIcon(QIcon(":/Icons/tools-and-utensils.svg"));
    setHorizontalHeaderItem(1, header);
  }
  else
  {
    QStandardItem* header = horizontalHeaderItem(0);
    header->setIcon(QIcon(":/Icons/arrows.svg"));
  }

  // If it hasn't been set yet
  if (!horizontalHeaderItem(2))
    setHorizontalHeaderItem(2, new QStandardItem("Name"));
  else
  {
    QStandardItem* header = horizontalHeaderItem(2);
    header->setText("Name");
  }

  if (!horizontalHeaderItem(3))
    setHorizontalHeaderItem(3, new QStandardItem("          Result          "));
  else
  {
    QStandardItem* header = horizontalHeaderItem(3);
    header->setText("          Result          ");
  }

  if (!horizontalHeaderItem(4))
    setHorizontalHeaderItem(4, new QStandardItem("Type"));
  else
  {
    QStandardItem* header = horizontalHeaderItem(4);
    header->setText("Type");
  }

  connect(mSelectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
          SLOT(itemSelectionChanged(QItemSelection, QItemSelection)));
  connect(mVideoWidget, &SMVideoZoomWidget::newExcureProject, this, &NTUProjectManager::onRunProject);
  connect(this, &NTUProjectManager::newDrawResult, mVideoWidget, &SMVideoZoomWidget::drawImage);
  //!
}

void NTUProjectManager::setProjectName(QString projectName) { this->mProjectName = projectName; }

void NTUProjectManager::onOpenProjectFromFileName(QString filename)
{
  QFile jsonFile(filename);

  if (!jsonFile.open(QIODevice::ReadOnly))
  {
    return;
  }

  // Read the entire file
  QByteArray readData = jsonFile.readAll();
  // Create QJsonDocument
  QJsonDocument jsonDocument(QJsonDocument::fromJson(readData));
  QJsonObject objectManager = jsonDocument.object();

  QJsonArray listTools = objectManager["listTools"].toArray();
  for (int i = 0; i < listTools.size(); i++)
  {
    QString toolName = listTools[i].toString();

    QJsonObject toolObject = objectManager[toolName].toObject();
    int type = toolObject["type"].toInt();
    this->insertNewTool((NTUTOOLS)type);


    for (int it = 0; it < mToolManager.size(); it++)
    {
      if (QString::compare(mToolManager[it]->getName(), toolName, Qt::CaseInsensitive) == 0)
      {
        mToolManager[it]->updateSettingFromJsonObject(toolObject);
      }
    }
  }
}

void NTUProjectManager::insertNewTool(NTUTOOLS tool)
{

  //! [1] clear selected
  mSelectionModel->clearSelection();
  //! [1]

  //! [2]
  QVector<QString> listFixtureTools = this->getFixtureToolsName();
  //! [2]

  NTUToolWidget* itemToolWidget;
  QList<QStandardItem*> itemList;
  QString nameID;
  this->clearWidgetInVBox();

  switch (tool)
  {
  case NTUTOOLS::PATTERN:
  {
    nameID = findnameDuplicate("Pattern");

    itemToolWidget = new NTUPatternWidget(mParameterAreaWidget, this->mVideoWidget, nameID);
    this->mLayout->addWidget(itemToolWidget, 0, 1, 1, 1);

    this->appendRow(itemToolWidget->getListStandarItems(PATTERN));
    break;
  }
  case NTUTOOLS::BLOB:
  {

    nameID = findnameDuplicate("Blob");

    itemToolWidget = new NTUBlobWidget(mParameterAreaWidget, this->mVideoWidget, nameID);
    this->mLayout->addWidget(itemToolWidget, 0, 1, 1, 1);

    this->appendRow(itemToolWidget->getListStandarItems(BLOB));
    break;
  }
  case NTUTOOLS::CIRCLEDETECT:
  {

    nameID = findnameDuplicate("Circle");

    itemToolWidget = new NTUCircleDetectWidget(mParameterAreaWidget, this->mVideoWidget, nameID);
    this->mLayout->addWidget(itemToolWidget, 0, 1, 1, 1);

    this->appendRow(itemToolWidget->getListStandarItems(CIRCLEDETECT));
    break;
  }
  case NTUTOOLS::PATMAX:
  {
    nameID = findnameDuplicate("PatMax");

    itemToolWidget = new NTUPatMaxWidget(mParameterAreaWidget, this->mVideoWidget, nameID);
    this->mLayout->addWidget(itemToolWidget, 0, 1, 1, 1);

    this->appendRow(itemToolWidget->getListStandarItems(PATMAX));
    break;
  }
  case NTUTOOLS::COLOR:
  {
      nameID = findnameDuplicate("Color");

      itemToolWidget = new NTUColorWidget(mParameterAreaWidget, this->mVideoWidget, nameID);
      this->mLayout->addWidget(itemToolWidget, 0, 1, 1, 1);

      this->appendRow(itemToolWidget->getListStandarItems(COLOR));
      break;
  }
  }

  if (itemToolWidget != nullptr)
  {
    mToolManager.push_back(itemToolWidget);
  }

  for (int k = 0; k < this->rowCount(); ++k)
  {
    auto currentIndex = this->index(k, 2);
    if (this->itemFromIndex(currentIndex)->data(Qt::DisplayRole) == nameID)
    {
      for (int i = 0; i < this->columnCount(); ++i)
      {
        QModelIndex index = this->index(currentIndex.row(), i);
        mSelectionModel->setCurrentIndex(index, QItemSelectionModel::Select);
      }
    }
  }
  connect(itemToolWidget, &NTUToolWidget::newExureToolAlgorithm, this, &NTUProjectManager::onRunProject);
  connect(itemToolWidget, &NTUToolWidget::newFixtureTool, this, &NTUProjectManager::onReceiverFixtureTools);

  for (size_t i = 0; i < listFixtureTools.size(); i++)
  {
    itemToolWidget->insertToolFixture(listFixtureTools[i]);
  }
}

void NTUProjectManager::itemSelectionChanged(const QItemSelection&, const QItemSelection&)
{
  // Q_EMIT newClearVBox();

  QModelIndexList listItemSelected = mSelectionModel->selectedRows();
  if (listItemSelected.empty())
    return;
  QStandardItem* item = this->item(listItemSelected.at(0).row(), 2);

  if (!item)
    return;

  QString text = item->text();

  for (int it = 0; it < mToolManager.size(); it++)
  {
    if (QString::compare(mToolManager[it]->getName(), text, Qt::CaseInsensitive) == 0)
    {
      mToolManager[it]->show();
      mLayout->addWidget(mToolManager[it], 0, 1, 1, 1);
    }
    else
    {
      mToolManager[it]->hide();
      mLayout->removeWidget(mToolManager[it]);
    }
  }
}

NTUProjectManager::~NTUProjectManager()
{
  // qDeleteAll(mVideoWidget);

  for (int it = 0; it < mToolManager.size(); it++)
  {
    mVideoWidget->removeToolWithName(mToolManager[it]->getName());
  }

  qDeleteAll(mToolManager); //  deletes all the values stored in "map"
}
void NTUProjectManager::inserNewItem(NTUItemTool* newTool, QStandardItem* parentItem) { parentItem->appendRow(newTool); }

void NTUProjectManager::clearWidgetInVBox()
{

  for (int it = 0; it < mToolManager.size(); it++)
  {
    QWidget* widget = dynamic_cast<QWidget*>(mToolManager[it]);
    if (widget != nullptr)
    {
      mLayout->removeWidget(widget);
      widget->hide();
    }
  }
  /*
QLayoutItem* child;
while ((child = vlayout->takeAt(0)) != 0)
{
  vlayout->removeItem(child);
}
*/
}

void NTUProjectManager::onReceiverItemFromClick(QModelIndex index)
{

  QStandardItem* item = this->item(index.row(), 2);

  if (!item)
    return;

  QString text = item->text();

  for (int it = 0; it < mToolManager.size(); it++)
  {
    if (QString::compare(mToolManager[it]->getName(), text, Qt::CaseInsensitive) == 0)
    {
      mToolManager[it]->show();
      mLayout->addWidget(mToolManager[it], 0, 1, 1, 1);
    }
    else
    {
      mToolManager[it]->hide();
      mLayout->removeWidget(mToolManager[it]);
    }
  }
}

void NTUProjectManager::onRunProject()
{
  std::cerr << "Run project\n";
  for (int it = 0; it < mToolManager.size(); it++)
  {
    std::cerr << "Name: " << mToolManager[it]->getName().toStdString() << "\n";
    mToolManager[it]->excureToolAlgorithm();
  }

  Q_EMIT newDrawResult();
}

void NTUProjectManager::onReceiverFixtureTools(NTUTools* tool, QString fixtureName)
{

  for (int it = 0; it < mToolManager.size(); it++)
  {
    if (mToolManager[it]->getName() == fixtureName)
    {
      tool->setToolFixture(dynamic_cast<NTUTools*>(mToolManager[it]->mToolAlgorithm));
    }
  }
}

void NTUProjectManager::onReceiverSaveProject(QString fileName)
{
  QJsonObject toolName;
  //! [1] get all name
  QJsonArray toolNameArray;
  for (int it = 0; it < mToolManager.size(); it++)
  {
    toolNameArray << QJsonValue::fromVariant(mToolManager[it]->getName());
    toolName[mToolManager[it]->getName()] = *(mToolManager[it]->getJsonObject());
  }
  toolName["listTools"] = toolNameArray;
  //! [1]

  QFile jsonFile(fileName);
  if (!jsonFile.open(QIODevice::WriteOnly))
  {
    return;
  }
  QJsonDocument document(toolName);

  jsonFile.write(document.toJson());
  jsonFile.close();
}

void NTUProjectManager::onReceiverDeleteFromSelectedIndex(QModelIndex index)
{
  QStandardItem* item = this->item(index.row(), 2);

  for (int it = 0; it < mToolManager.size(); it++)
  {
    if (mToolManager[it]->getName() == item->text())
    {
      mToolManager[it]->hide();
      mLayout->removeWidget(mToolManager[it]);
      this->mVideoWidget->removeToolWithName(item->text());

      mToolManager.erase(mToolManager.begin() + it);
    }
  }
  this->removeRow(index.row());
  this->mVideoWidget->drawImage();
}

QString NTUProjectManager::findnameDuplicate(QString name)
{
  QString createName = name + "-%1";
  QString name_ = name; // createName.arg(0, 5, 10, QChar('0'));
  int k = 1;
  while (true)
  {

    int number = 0;
    int total = 0;

    for (int it = 0; it < mToolManager.size(); it++)
    {
      total++;
      if (mToolManager[it]->getName() == name_)
      {
        name_ = createName.arg(k, 1, 10, QChar('0'));
        k++;
        break;
      }
      else
      {
        number++;
      }
    }
    if (total == number)
      break;
  }

  return name_;
}

QVector<QString> NTUProjectManager::getFixtureToolsName()
{
  QVector<QString> listTools;
  for (int it =0 ; it < mToolManager.size(); it++)
  {
    switch (mToolManager[it]->getToolType())
    {
    case PATTERN:
    {
      listTools.push_back(mToolManager[it]->getName());
      break;
    }
    case BLOB:
    {
      listTools.push_back(mToolManager[it]->getName());
      break;
    }
    case PATMAX:
    {
      listTools.push_back(mToolManager[it]->getName());
      break;
    }
    default:
      break;
    }
  }
  return listTools;
}
