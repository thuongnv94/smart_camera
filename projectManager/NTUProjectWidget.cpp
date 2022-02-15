#include "NTUProjectWidget.h"
#include "ui_NTUProjectWidget.h"

NTUProjectWidget::NTUProjectWidget(QWidget* parent) : QGroupBox(parent), ui(new Ui::NTUProjectWidget) { ui->setupUi(this); }

NTUProjectWidget::~NTUProjectWidget() { delete ui; }

void NTUProjectWidget::on_pushButton_Create_clicked()
{
  QString projectName = ui->lineEdit_projectName->text();
  //  QString command = tr("mkdir ") + projectName;
  //  system(command.toLatin1());
  Q_EMIT newCreateProjectName(projectName);
  this->close();
}

void NTUProjectWidget::on_pushButton_openProject_clicked()
{
  QSettings settings("Setting.ini", QSettings::IniFormat);

  const QString DEFAULT_DIR_KEY("default_dir");

  QString filename =
      QFileDialog::getOpenFileName(this, tr("Open Job"), settings.value(DEFAULT_DIR_KEY).toString(), tr("Json(*.json *.JSON)"));
  if (filename.isNull())
  {
    return;
  }
  else
  {
    QFileInfo file_info(filename);
    settings.setValue(DEFAULT_DIR_KEY, file_info.absolutePath());
    Q_EMIT newOpenProjectName(filename);
  }
}
