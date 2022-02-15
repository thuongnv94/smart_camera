#include "NTUListToolItem.h"
#include <QDebug>
NTUListToolItem::NTUListToolItem(QString name, NTUTOOLS tool, QIcon icon):QStandardItem(name)
{
    this->setData(QVariant::fromValue(name), NAME);
    //this->setData(QVariant::fromValue(tool), TYPE);
    mType = tool;
    this->setIcon(icon);
    this->setText(name);

}
