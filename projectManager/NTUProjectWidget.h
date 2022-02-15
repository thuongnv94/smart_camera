#ifndef NTUPROJECTWIDGET_H
#define NTUPROJECTWIDGET_H

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QSettings>
#include <QString>
#include <QWidget>

namespace Ui
{
class NTUProjectWidget;
}

class NTUProjectWidget : public QGroupBox
{
  Q_OBJECT

public:
  explicit NTUProjectWidget(QWidget* parent = nullptr);
  ~NTUProjectWidget();

Q_SIGNALS:
  void newCreateProjectName(QString name);
  void newOpenProjectName(QString fileName);
private Q_SLOTS:
  void on_pushButton_Create_clicked();

  void on_pushButton_openProject_clicked();

private:
  Ui::NTUProjectWidget* ui;
};

#endif // NTUPROJECTWIDGET_H
