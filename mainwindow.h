#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QDebug>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QAbstractItemView>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "projectManager/NTUProjectManager.h"
#include "projectManager/NTUProjectWidget.h"




#include "Interface/NTULoadImagePC.h"
#include "Interface/NTUIspectionTools.h"
#include "Interface/NTUSaveJob.h"

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  ///
  /// \brief initializerConnect
  ///
  void initializerConnect();

  ///
  /// \brief initializerListTools
  ///
  void initializerListTools();

Q_SIGNALS:
  void newInsertItem(NTUTOOLS tool);
  void newOpenProjectName(QString fileName);
  void newClearLayout();
public Q_SLOTS:
  void onReceiverCreateNewProject();
  void onReceiverCreateNewToolFromSelected(NTUTOOLS type);
  void onCreateNewProject(QString projectName);
  void onReceiverDirectoryImagesFromPC(QDir directory);
  void listWidgetImageselectionChanged();
  void onReceiverOpenProject(QString fileName);

private Q_SLOTS:
    void clearWidgetInVBox();

    void on_toolButton_listWidgetBegin_clicked();

    void on_toolButton_listWidgetEnd_clicked();

    void on_toolButton_listWidgetPrevious_clicked();

    void on_toolButton_listWidgetNext_clicked();

    void on_toolButton_imageSetup_clicked();

    void on_toolButton_locatePart_clicked();

    void on_toolButton_setupCamera_clicked();

    void on_toolButton_saveJob_clicked();

private:
  Ui::MainWindow* ui;

  NTUProjectManager* mCurrentProject;
  NTUProjectWidget* mProjectConfig;
  NTULoadImagePC *mLoadImagePC;
  NTUIspectionTools *mInspectionTool;
  NTUSaveJob *mSaveJob;
  QString mDirectoryImagesPC;


};
#endif // MAINWINDOW_H
