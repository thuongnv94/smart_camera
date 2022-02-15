#ifndef NTUITEMTOOLNAME_H
#define NTUITEMTOOLNAME_H

#include <QStandardItem>
#include "NTUItemTool.h"

class NTUItemToolName: public QStandardItem
{
public:
    NTUItemToolName(QString name = "", NTUTOOLS type = PATTERN);
};

#endif // NTUITEMTOOLNAME_H
