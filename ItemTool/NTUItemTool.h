#ifndef NTUITEMTOOL_H
#define NTUITEMTOOL_H

#include <QObject>
#include <QStandardItem>

enum NTUTOOLS
{
  PATTERN,
  BLOB,
  CIRCLEDETECT,
  PATMAX,
  COLOR
};

enum ItemType
{
  TYPE,
  NAME
};

class SMVideoZoomWidget;
class NTUToolWidget;

class NTUItemTool : public QStandardItem
{

public:
  NTUItemTool(QString name = "default_item", NTUToolWidget* widget = nullptr);
  virtual ~NTUItemTool();

protected:
  NTUToolWidget* mItemToolWidget;
};

Q_DECLARE_METATYPE(NTUItemTool);
Q_DECLARE_METATYPE(NTUTOOLS);

#endif // NTUITEMTOOL_H
