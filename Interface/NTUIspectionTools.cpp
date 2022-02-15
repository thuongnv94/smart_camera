#include "NTUIspectionTools.h"
#include "ui_NTUIspectionTools.h"

NTUIspectionTools::NTUIspectionTools(QWidget* parent) : QGroupBox(parent), ui(new Ui::NTUIspectionTools)
{
  ui->setupUi(this);

  mListToolManager = new NTUListToolManager(this);

  NTUListToolItem* pattern = new NTUListToolItem("Pattern", PATTERN, QIcon(":/Icons/inspection.svg"));
  mListToolManager->addNewTools(pattern);

  NTUListToolItem* blob = new NTUListToolItem("Blob", BLOB, QIcon(":/Icons/search.svg"));
  mListToolManager->addNewTools(blob);

  NTUListToolItem* circle = new NTUListToolItem("Circle", CIRCLEDETECT, QIcon(":/Icons/rec.svg"));
  mListToolManager->addNewTools(circle);

  NTUListToolItem* partMax = new NTUListToolItem("PatMax", PATMAX, QIcon(":/Icons/searching.svg"));
  mListToolManager->addNewTools(partMax);

  NTUListToolItem* color = new NTUListToolItem("Color", COLOR, QIcon(":/Icons/rgb.svg"));
  mListToolManager->addNewTools(color);


  ui->treeViewListTools->setModel(mListToolManager);
  mMultiplexer = new NTUSignalMultiplexer(this);
  mMultiplexer->setCurrentObject(mListToolManager);

  mMultiplexer->connect(SIGNAL(newSelectionIndexAvailable(bool)), ui->pushButton_addToolFromTreeView, SLOT(setEnabled(bool)));
  mMultiplexer->connect(ui->pushButton_addToolFromTreeView, SIGNAL(clicked()), SLOT(onReceiverCreaeToolFromIndexSelected()));
  mMultiplexer->connect(SIGNAL(newCreateToolFromIndexSelected(NTUTOOLS)), this, SLOT(onReceiverCreateNewToolFromSelected(NTUTOOLS)));
  // connect(ui->pushButton_addToolFromTreeView, &QPushButton::clicked, this, &MainWindow::clearWidgetInVBox);
}

NTUIspectionTools::~NTUIspectionTools() { delete ui; }

void NTUIspectionTools::onReceiverCreateNewToolFromSelected(NTUTOOLS type)
{
    Q_EMIT newClearWidgetInVBox();
    Q_EMIT newCreateNewToolFromSelected(type);
}

