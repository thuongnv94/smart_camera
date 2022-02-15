#ifndef NTUISPECTIONTOOLS_H
#define NTUISPECTIONTOOLS_H

#include <QGroupBox>

#include "listTools/NTUListToolItem.h"
#include "listTools/NTUListToolManager.h"
#include "listTools/NTUSignalMultiplexer.h"

namespace Ui
{
class NTUIspectionTools;
}

class NTUIspectionTools : public QGroupBox
{
  Q_OBJECT

public:
  explicit NTUIspectionTools(QWidget* parent = nullptr);
  ~NTUIspectionTools();
public Q_SLOTS:
  void onReceiverCreateNewToolFromSelected(NTUTOOLS type);
Q_SIGNALS:
  void newClearWidgetInVBox();
  void newCreateNewToolFromSelected(NTUTOOLS type);
private Q_SLOTS:

private:
  Ui::NTUIspectionTools* ui;
  NTUListToolManager* mListToolManager;
  NTUSignalMultiplexer* mMultiplexer;
};

#endif // NTUISPECTIONTOOLS_H
