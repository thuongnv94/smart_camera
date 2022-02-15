#include "NTULoadImagePC.h"
#include "ui_NTULoadImagePC.h"

NTULoadImagePC::NTULoadImagePC(QWidget* parent) : QGroupBox(parent), ui(new Ui::NTULoadImagePC) { ui->setupUi(this);


}

NTULoadImagePC::~NTULoadImagePC() { delete ui; }

void NTULoadImagePC::loadDefaultImageFromPath()
{
    QSettings settings("Setting.ini", QSettings::IniFormat);
    QString imageLocalpath = settings.value("ImageDir", "").toString();
    if(!imageLocalpath.isEmpty())
    {
        ui->lineEdit_imagePathPC->setText(imageLocalpath);
        QDir dir(imageLocalpath);
        Q_EMIT newDirectory(dir);

    }
}

void NTULoadImagePC::on_pushButton_choseDirectory_clicked()
{
  QSettings settings("Setting.ini", QSettings::IniFormat);

  const QString DEFAULT_DIR_KEY("default_dir");

  QDir dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), settings.value(DEFAULT_DIR_KEY).toString(),
                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  QDir currentdir;
  settings.setValue(DEFAULT_DIR_KEY, dir.absolutePath());

//  QStringList images = QDir(dir).entryList(QStringList() << "*.bmp"
//                                                         << "*.BMP",
//                                           QDir::Files);

  ui->lineEdit_imagePathPC->setText(dir.absolutePath());
  settings.setValue("ImageDir", dir.absolutePath());

  Q_EMIT newDirectory(dir);
}
