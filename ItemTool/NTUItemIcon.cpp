#include "NTUItemIcon.h"

NTUItemIcon::NTUItemIcon(QString name, NTUTOOLS type) : QStandardItem(name)
{
  switch (type)
  {
  case PATTERN:
    this->setIcon(QIcon(":/Icons/inspection.svg"));
    break;
  case BLOB:
    this->setIcon(QIcon(":/Icons/search.svg"));
    break;
  case CIRCLEDETECT:
    this->setIcon(QIcon(":/Icons/rec.svg"));
    break;
  case PATMAX:
    this->setIcon(QIcon(":/Icons/searching.svg"));
    break;
  case COLOR:
    this->setIcon(QIcon(":/Icons/rgb.svg"));
    break;
  }
}
