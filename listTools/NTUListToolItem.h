#ifndef NTULISTTOOLITEM_H
#define NTULISTTOOLITEM_H

#include <QObject>
#include <QStandardItem>
#include "ItemTool/NTUItemTool.h"



class NTUListToolItem : public QStandardItem
{


public:
    NTUListToolItem(QString name = "defaultItem", NTUTOOLS tool = PATTERN, QIcon icon = QIcon());

    NTUTOOLS mType;
};

Q_DECLARE_METATYPE(NTUListToolItem);

#endif // NTULISTTOOLITEM_H
