#include "NTUToolBrowser.h"
#include "projectManager/NTUProjectManager.h"

NTUToolBrowser::NTUToolBrowser(QWidget* parent) : QTreeView(parent)
{
  this->setItemDelegate(new BackgroundDelegate(this));
  this->setSortingEnabled(true);
  // this->sortByColumn(0, Qt::AscendingOrder);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &NTUToolBrowser::customContextMenuRequested, this, &NTUToolBrowser::contextualMenuTreeView);
}

void NTUToolBrowser::removeModel()
{
  QTreeView::setModel(nullptr);

  mCurrentProjectManager = nullptr;
  // delete  mCurrentProjectManager;
}

void NTUToolBrowser::setModel(QAbstractItemModel* model)
{
  QTreeView::setModel(model);
  mCurrentProjectManager = dynamic_cast<NTUProjectManager*>(model);
  this->setSelectionModel(mCurrentProjectManager->getItemSelectionModel());
  for (int i = 0; i < this->header()->count(); ++i)
    this->header()->resizeSection(i, this->header()->sectionSizeHint(i));

  connect(this, &NTUToolBrowser::newShowItemFromIndex, dynamic_cast<NTUProjectManager*>(model),
          &NTUProjectManager::onReceiverItemFromClick);
  connect(this, &NTUToolBrowser::newDeleteItem, dynamic_cast<NTUProjectManager*>(model),
          &NTUProjectManager::onReceiverDeleteFromSelectedIndex);
}

void NTUToolBrowser::mousePressEvent(QMouseEvent* event)
{
  QModelIndex index = this->currentIndex();
  if (index.isValid())
  {
    Q_EMIT newClearVBoxLayout();
    Q_EMIT newShowItemFromIndex(index);
  }

  QTreeView::mousePressEvent(event);
}

void NTUToolBrowser::contextualMenuTreeView(const QPoint& point)
{
  // Get the mouse position in the scene
  QPoint globalPos = mapToGlobal(point);
  // Create the menu and add action
  QMenu* contextMenu = new QMenu(this);
  QModelIndex index = this->currentIndex();

  if (index.isValid()/* && index.parent().isValid()*/)
  {
      contextMenu->addAction(QIcon(":/Icons/pqQuit22.png"), "Delete", this, SLOT(onReceiverDeleteFromIndex()));

    // Display the menu
    contextMenu->exec(globalPos);
  }
}

void NTUToolBrowser::onReceiverDeleteFromIndex() {
    QModelIndex index = this->currentIndex();
    Q_EMIT newDeleteItem(index); }

void BackgroundDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  // Fill the background before calling the base class paint
  // otherwise selected cells would have a white background
  // QVariant background = index.data (Qt::BackgroundRole);
  // if (background.canConvert<QBrush> ())
  //   painter->fillRect (option.rect, background.value<QBrush> ());

  QVariant text_color_variant = index.data(Qt::TextColorRole);
  if (text_color_variant.canConvert<QColor>())
  {
    QColor text_color = text_color_variant.value<QColor>();
    QPalette palette = option.palette;
    QStyleOptionViewItem option_copy = option;
    option_copy.palette.setColor(QPalette::HighlightedText, text_color);
    QStyledItemDelegate::paint(painter, option_copy, index);
  }
  else
    QStyledItemDelegate::paint(painter, option, index);
}
