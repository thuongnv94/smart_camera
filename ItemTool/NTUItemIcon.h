#ifndef NTUITEMICON_H
#define NTUITEMICON_H
#include <QStandardItem>
#include "NTUItemTool.h"

class NTUItemIcon :  public QStandardItem
{
public:
    NTUItemIcon(QString name = nullptr, NTUTOOLS type = PATTERN);

};

#endif // NTUITEMICON_H
