#include "NTUToolWidget.h"
#include "SMVideoZoomWidget.h"

NTUToolWidget::NTUToolWidget(QWidget* parent, QWidget* video) : QGroupBox(parent)
{
  mVideoWidget = dynamic_cast<SMVideoZoomWidget*>(video);
  connect(this, &NTUToolWidget::newWidgetVisibilityChanged, this, &NTUToolWidget::onReceiverVisibilityShape);
  this->mJsonObject = new QJsonObject();
}

NTUToolWidget::~NTUToolWidget()
{
  if (mNTUItemMyselft != nullptr)
  {
    delete mNTUItemMyselft;
    mNTUItemMyselft = nullptr;
  }
  if (mIndicatorItem != nullptr)
  {
    delete mIndicatorItem;
    mIndicatorItem = nullptr;
  }
  if (mIconItem != nullptr)
  {
    delete mIconItem;
    mIconItem = nullptr;
  }
  if (mResultItem != nullptr)
  {
    delete mResultItem;
    mResultItem = nullptr;
  }
  if (mToolNameItem != nullptr)
  {
    delete mToolNameItem;
    mToolNameItem = nullptr;
  }
  if (mJsonObject != nullptr)
  {
    delete mJsonObject;
    mJsonObject = nullptr;
  }
  if (mToolAlgorithm != nullptr)
  {
    delete mToolAlgorithm;
    mToolAlgorithm = nullptr;
  }

}

void NTUToolWidget::setNTUItemTool(NTUItemTool* itemTool) { mNTUItemMyselft = itemTool; }

QList<QStandardItem*> NTUToolWidget::getListStandarItems(NTUTOOLS type)
{
  QList<QStandardItem*> listItems;
  this->mToolType = type;

  mIndicatorItem = new NTUIndicator("", NONE);
  listItems << dynamic_cast<QStandardItem*>(mIndicatorItem);

  mIconItem = new NTUItemIcon("", type);
  listItems << dynamic_cast<QStandardItem*>(mIconItem);

  mNTUItemMyselft = new NTUItemTool(mName, this);
  listItems << dynamic_cast<QStandardItem*>(mNTUItemMyselft);

  mResultItem = new NTUItemResult("");
  listItems << dynamic_cast<QStandardItem*>(mResultItem);

  mToolNameItem = new NTUItemToolName(mName, type);
  listItems << dynamic_cast<QStandardItem*>(mToolNameItem);

  connect(mToolAlgorithm, &NTUTools::newResult, this, &NTUToolWidget::onReceiverResult);
  // connect(this, &NTUToolWidget::newExureToolAlgorithm, mToolAlgorithm, &NTUTools::computer);

  return listItems;
}

NTUItemTool* NTUToolWidget::getItemTool() { return mNTUItemMyselft; }

void NTUToolWidget::excureToolAlgorithm()
{
  if (mToolAlgorithm != nullptr)
    mToolAlgorithm->computer();
}

void NTUToolWidget::showEvent(QShowEvent* event)
{
  Q_EMIT newWidgetVisibilityChanged(true);
  QWidget::showEvent(event);
}

void NTUToolWidget::hideEvent(QHideEvent* event)
{
  Q_EMIT newWidgetVisibilityChanged(false);
  QWidget::hideEvent(event);
}

void NTUToolWidget::setupCombobox(QComboBox* ccbox)
{
  ccbox->setEditable(true);
  ccbox->lineEdit()->setReadOnly(true);
  ccbox->lineEdit()->setAlignment(Qt::AlignRight);
  for (int i = 0; i < ccbox->count(); i++)
  {
    ccbox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
  }
}

void NTUToolWidget::onUpdateName(QString name)
{
  this->mName = name;
  mNTUItemMyselft->setText(name);
  this->mToolAlgorithm->setToolName(name);
}

void NTUToolWidget::onReceiverResult(QString result)
{
  if (mResultItem != nullptr)
    mResultItem->setResult(result);
  if (mIndicatorItem != nullptr)
  {
    if (result == "")
      mIndicatorItem->setStatus(NG);
    else
      mIndicatorItem->setStatus(OK);
  }
}
