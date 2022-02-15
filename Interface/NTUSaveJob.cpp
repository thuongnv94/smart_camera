#include "NTUSaveJob.h"
#include "ui_NTUSaveJob.h"

NTUSaveJob::NTUSaveJob(QWidget* parent) : QGroupBox(parent), ui(new Ui::NTUSaveJob) { ui->setupUi(this); }

NTUSaveJob::~NTUSaveJob() { delete ui; }

void NTUSaveJob::on_pushButton_save_clicked()
{
  QSettings settings("Setting.ini", QSettings::IniFormat);

  const QString DEFAULT_DIR_KEY("default_dir");

  QString filename =
      QFileDialog::getSaveFileName(this, tr("Save Job"), settings.value(DEFAULT_DIR_KEY).toString(), tr("Json(*.json *.JSON)"));
  if (filename.isNull())
  {
    return;
  }
  else
  {
    QFileInfo file_info(filename);
    settings.setValue(DEFAULT_DIR_KEY, file_info.absolutePath());
    Q_EMIT newSaveJobFile(filename);
  }
}
