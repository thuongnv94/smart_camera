#include "NTUIndicator.h"

NTUIndicator::NTUIndicator(QString name, ItemIndocator indicator) : QStandardItem(name) { this->setIcon(QIcon(":/Icons/gray.svg")); }

void NTUIndicator::setStatus(ItemIndocator idicator)
{
  switch (idicator)
  {
  case NONE:
    this->setIcon(QIcon(":/Icons/gray.svg"));
    break;
  case OK:
    this->setIcon(QIcon(":/Icons/green.svg"));
    break;
  case NG:
    this->setIcon(QIcon(":/Icons/red.svg"));
    break;
  }
}
