#include "mainwindow.h"
#include "AlgorithmTools/NTUCVTools.h"
#include "cvtools.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), mCurrentProject(nullptr)
{
  ui->setupUi(this);

  qRegisterMetaType<NTUTOOLS>("NTUTOOLS");

  ui->dockWidget->setTitleBarWidget(new QWidget());
  ui->dockWidget_parameters->setTitleBarWidget(new QWidget());
  ui->dockWidget_4->setTitleBarWidget(new QWidget());

  ui->listWidget_imagesItem->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->listWidget_imagesItem->setViewMode(QListWidget::IconMode);
  ui->listWidget_imagesItem->setIconSize(QSize(80, 60));

  this->initializerListTools();

  this->initializerConnect();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initializerConnect()
{
  connect(ui->actionNew_Project, &QAction::triggered, this, &MainWindow::onReceiverCreateNewProject);
  connect(ui->actionFitScreen, &QAction::triggered, ui->graphicsView, &SMVideoZoomWidget::fitImage);
  connect(ui->actionZoom_In, &QAction::triggered, ui->graphicsView, &SMVideoZoomWidget::zoomIn);
  connect(ui->actionZoom_Out, &QAction::triggered, ui->graphicsView, &SMVideoZoomWidget::zoomOut);

  connect(ui->listWidget_imagesItem, &QListWidget::itemSelectionChanged, this, &MainWindow::listWidgetImageselectionChanged);
  connect(ui->treeViewTools, &NTUToolBrowser::newClearVBoxLayout, this, &MainWindow::clearWidgetInVBox);

  //! [2]
  // connect(mListToolManager, &NTUListToolManager::newSelectionIndexAvailable, ui->pushButton_addToolFromTreeView,
  // &QPushButton::setEnabled);
  //! [2]
}

void MainWindow::initializerListTools()
{
  //! [1]
  mInspectionTool = new NTUIspectionTools(ui->dockWidgetContents_parameters);
  // connect(mInspectionTool, &NTUIspectionTools::newClearWidgetInVBox, this, &MainWindow::clearWidgetInVBox);
  connect(mInspectionTool, &NTUIspectionTools::newCreateNewToolFromSelected, this, &MainWindow::onReceiverCreateNewToolFromSelected);
  //! [1]

  //! [2]
  mProjectConfig = new NTUProjectWidget(ui->dockWidgetContents_parameters);
  connect(mProjectConfig, &NTUProjectWidget::newCreateProjectName, this, &MainWindow::onCreateNewProject);
  connect(mProjectConfig, &NTUProjectWidget::newOpenProjectName, this, &MainWindow::onReceiverOpenProject);
  //! [2]
  //! [3]
  mSaveJob = new NTUSaveJob(ui->dockWidgetContents_parameters);
  //! [3]
  //! [4]
  mLoadImagePC = new NTULoadImagePC(ui->dockWidgetContents_parameters);
  connect(mLoadImagePC, &NTULoadImagePC::newDirectory, this, &MainWindow::onReceiverDirectoryImagesFromPC);
  mLoadImagePC->loadDefaultImageFromPath();
  //! [4]

  //! [7] default
  this->clearWidgetInVBox();
  mProjectConfig->show();
  //! 7]
}

void MainWindow::onReceiverCreateNewProject()
{
  //! [1] clear all child widget
  this->clearWidgetInVBox();
  //! [1]
  Q_EMIT newClearLayout();

  //! [2] create dialog
  ui->gridLayout_Task->addWidget(mProjectConfig, 0, 1, 1, 1);

  //! [2]
}

void MainWindow::onReceiverCreateNewToolFromSelected(NTUTOOLS type) { Q_EMIT newInsertItem(type); }

void MainWindow::onCreateNewProject(QString projectName)
{
  //! [1] clear all child widget
  this->clearWidgetInVBox();
  //! [1]

  ui->treeViewTools->removeModel();

  if (mCurrentProject != nullptr)
  {
    delete mCurrentProject;
    mCurrentProject = nullptr;
  }

  Q_EMIT newClearLayout();

  NTUProjectManager* project =
      new NTUProjectManager(this, ui->dockWidgetContents_parameters, ui->graphicsView, ui->gridLayout_Task, projectName);
  mCurrentProject = project;

  ui->treeViewTools->setModel(mCurrentProject);
  connect(this, &MainWindow::newInsertItem, mCurrentProject, &NTUProjectManager::insertNewTool);
  connect(this, &MainWindow::newClearLayout, mCurrentProject, &NTUProjectManager::clearWidgetInVBox);
  connect(mCurrentProject, &NTUProjectManager::newClearVBox, this, &MainWindow::clearWidgetInVBox);
  connect(mSaveJob, &NTUSaveJob::newSaveJobFile, mCurrentProject, &NTUProjectManager::onReceiverSaveProject);
}

void MainWindow::onReceiverDirectoryImagesFromPC(QDir directory)
{

  directory.setNameFilters({"*.png", "*.jpg", "*.bmp", "*.BMP"});
  ui->listWidget_imagesItem->clear();
  for (const QFileInfo& finfo : directory.entryInfoList())
  {
    QListWidgetItem* item = new QListWidgetItem(QIcon(finfo.absoluteFilePath()), finfo.fileName());
    ui->listWidget_imagesItem->addItem(item);
  }

  mDirectoryImagesPC = directory.absolutePath();

  if (ui->listWidget_imagesItem->count() > 0)
  {
    ui->listWidget_imagesItem->setCurrentRow(0);

    QString filename = mDirectoryImagesPC + "/" + ui->listWidget_imagesItem->item(0)->text();
    cv::Mat imageLoad = imread(filename.toStdString());
    if (imageLoad.empty())
      return;
    ui->graphicsView->showImageCV(imageLoad);
    ui->graphicsView->fitImage();
    // listWidgetImageselectionChanged();
  }
}

void MainWindow::listWidgetImageselectionChanged()
{
  QString filename = mDirectoryImagesPC + "/" + ui->listWidget_imagesItem->currentItem()->text();
  cv::Mat imageLoad = imread(filename.toStdString());
  if (imageLoad.empty())
    return;
  ui->graphicsView->showImageCV(imageLoad);
}

void MainWindow::onReceiverOpenProject(QString fileName)
{
  //! [1] clear all child widget
  this->clearWidgetInVBox();
  //! [1]
  ui->treeViewTools->removeModel();

  if (mCurrentProject != nullptr)
  {
    delete mCurrentProject;
    mCurrentProject = nullptr;

  }

  Q_EMIT newClearLayout();
  QFileInfo fileInfor(fileName);
  QString croped_fileName = fileInfor.fileName().section(".", 0, 0);
  NTUProjectManager* project =
      new NTUProjectManager(this, ui->dockWidgetContents_parameters, ui->graphicsView, ui->gridLayout_Task, croped_fileName);
  mCurrentProject = project;
  ui->treeViewTools->setModel(mCurrentProject);
  connect(this, &MainWindow::newInsertItem, mCurrentProject, &NTUProjectManager::insertNewTool);
  connect(this, &MainWindow::newClearLayout, mCurrentProject, &NTUProjectManager::clearWidgetInVBox);
  connect(this, &MainWindow::newOpenProjectName, mCurrentProject, &NTUProjectManager::onOpenProjectFromFileName);
  connect(mCurrentProject, &NTUProjectManager::newClearVBox, this, &MainWindow::clearWidgetInVBox);
  connect(mSaveJob, &NTUSaveJob::newSaveJobFile, mCurrentProject, &NTUProjectManager::onReceiverSaveProject);
  Q_EMIT newOpenProjectName(fileName);
}

void MainWindow::clearWidgetInVBox()
{
  if (mProjectConfig != nullptr)
    mProjectConfig->hide();

  if (mLoadImagePC != nullptr)
    mLoadImagePC->hide();

  if (mInspectionTool != nullptr)
    mInspectionTool->hide();

  if (mSaveJob != nullptr)
    mSaveJob->hide();
}

void MainWindow::on_toolButton_listWidgetBegin_clicked()
{
  if (ui->listWidget_imagesItem->count() != 0)
  {
    ui->listWidget_imagesItem->setCurrentItem(ui->listWidget_imagesItem->item(0));
  }
}

void MainWindow::on_toolButton_listWidgetEnd_clicked()
{
  if (ui->listWidget_imagesItem->count() != 0)
  {
    ui->listWidget_imagesItem->setCurrentItem(ui->listWidget_imagesItem->item(ui->listWidget_imagesItem->count() - 1));
  }
}

void MainWindow::on_toolButton_listWidgetPrevious_clicked()
{
  int cout = ui->listWidget_imagesItem->count();
  if (cout != 0)
  {
    int currentRow = ui->listWidget_imagesItem->currentRow();
    if (currentRow == 0)
      return;
    ui->listWidget_imagesItem->setCurrentItem(ui->listWidget_imagesItem->item(currentRow - 1));
  }
}

void MainWindow::on_toolButton_listWidgetNext_clicked()
{
  int cout = ui->listWidget_imagesItem->count();
  if (cout != 0)
  {
    int currentRow = ui->listWidget_imagesItem->currentRow();
    if (currentRow >= ui->listWidget_imagesItem->count() - 1)
      return;
    ui->listWidget_imagesItem->setCurrentItem(ui->listWidget_imagesItem->item(currentRow + 1));
  }
}

void MainWindow::on_toolButton_imageSetup_clicked()
{
  //! [1] clear all child widget
  this->clearWidgetInVBox();
  //! [1]
  Q_EMIT newClearLayout();

  ui->gridLayout_Task->addWidget(mLoadImagePC, 0, 1, 1, 1);
  mLoadImagePC->show();
}

void MainWindow::on_toolButton_locatePart_clicked()
{
  this->clearWidgetInVBox();
  ui->gridLayout_Task->addWidget(mInspectionTool, 0, 0, 1, 1, Qt::AlignLeft);
  mInspectionTool->show();
}

void MainWindow::on_toolButton_setupCamera_clicked()
{
  this->clearWidgetInVBox();
  ui->gridLayout_Task->addWidget(mProjectConfig, 0, 0, 1, 1, Qt::AlignLeft);
  mProjectConfig->show();
}

void MainWindow::on_toolButton_saveJob_clicked()
{
  this->clearWidgetInVBox();
  ui->gridLayout_Task->addWidget(mSaveJob, 0, 0, 1, 1, Qt::AlignLeft);
  mSaveJob->show();
}
