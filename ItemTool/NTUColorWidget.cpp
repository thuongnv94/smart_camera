#include "NTUColorWidget.h"
#include "SMVideoZoomWidget.h"
#include "cvtools.h"
#include "ui_NTUColorWidget.h"
#include <QColorDialog>

NTUColorWidget::NTUColorWidget(QWidget* parent, QWidget* video, QString toolName) : NTUToolWidget(parent, video), ui(new Ui::NTUColorWidget)
{
  ui->setupUi(this);

  this->setupCombobox(ui->comboBox_toolEnable);
  this->setupCombobox(ui->comboBox_toolFixture);
  //! [1]
  this->mName = toolName;
  this->mToolType = COLOR;
  ui->lineEdit_name->setText(mName);
  //! [1]

  //! [2]
  this->mRectangleSearch = new NTURectangle("Search_Color");
  this->mToolAlgorithm = new NTUColor(mName);

  //! [2]

  //! [3]
  this->mVideoWidget->addNewNTUTool(mName, mToolAlgorithm);
  this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  //! [3]

  //! [4]
  this->setTextFromRectToLineEdit(mRectangleSearch->getRegtangle(), ui->lineEdit_search);

  //! [5]
  connect(this, &NTUToolWidget::newJSonSetting, mToolAlgorithm, &NTUTools::onReadSettingFromJSon);

  // connect(ui->lineEdit_minBGR, &QLineEdit::textChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  // connect(ui->lineEdit_maxBGR, &QLineEdit::textChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_hMin, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_Smin, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_Vmin, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_Hmax, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_Smax, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->horizontalSlider_Vmax, &QSlider::valueChanged, this, &NTUColorWidget::onReceiverSettingChanged);

  connect(ui->lineEdit_search, &QLineEdit::textChanged, this, &NTUColorWidget::onReceiverSettingChanged);

  connect(ui->spinBox_count, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUColorWidget::onReceiverSettingChanged);
  connect(ui->spinBox_timeOut, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUColorWidget::onReceiverSettingChanged);

  connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &NTUColorWidget::onUpdateName);
  connect(mRectangleSearch, &NTURectangle::newUpdateRectangle, this, &NTUColorWidget::onUpdateRectangleSearch);
  // connect(ui->comboBox_toolFixture, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  // &NTUPatternWidget::onReceiverSettingChanged);

  connect(mToolAlgorithm, &NTUColor::newSendImageResult, ui->imageShow, &NTUVideoWidget::showImageCV);
  connect(mToolAlgorithm, &NTUColor::newExecutionTime, ui->lineEdit_executionTime, &QLineEdit::setText);
  //! [5]

  //! [6]
  this->onReceiverSettingChanged();
  this->mVideoWidget->drawImage();
  //! [6]
}

NTUColorWidget::~NTUColorWidget()
{
  delete ui;
  if (mRectangleSearch != nullptr)
  {
    delete mRectangleSearch;
    mRectangleSearch = nullptr;
  }
}

void NTUColorWidget::updateSettingFromJsonObject(QJsonObject jsonObject)
{
  this->blockSignals(true);
  ui->lineEdit_minBGR->setText(jsonObject.value(ColorSettings[NTU_COLOR_HSV_MIN]).toString());
  ui->lineEdit_maxBGR->setText(jsonObject.value(ColorSettings[NTU_COLOR_HSV_MAX]).toString());

  int r, g, b;
  if (cvtools::getBGRFromQString(jsonObject.value(ColorSettings[NTU_COLOR_HSV_MIN]).toString(), b, g, r))
  {
    ui->horizontalSlider_hMin->blockSignals(true);
    ui->horizontalSlider_hMin->setValue(b);
    ui->horizontalSlider_hMin->blockSignals(false);

    ui->horizontalSlider_Smin->blockSignals(true);
    ui->horizontalSlider_Smin->setValue(g);
    ui->horizontalSlider_Smin->blockSignals(false);

    ui->horizontalSlider_Vmin->blockSignals(true);
    ui->horizontalSlider_Vmin->setValue(r);
    ui->horizontalSlider_Vmin->blockSignals(false);
  }

  if (cvtools::getBGRFromQString(jsonObject.value(ColorSettings[NTU_COLOR_HSV_MAX]).toString(), b, g, r))
  {
    ui->horizontalSlider_Hmax->blockSignals(true);
    ui->horizontalSlider_Hmax->setValue(b);
    ui->horizontalSlider_Hmax->blockSignals(false);

    ui->horizontalSlider_Smax->blockSignals(true);
    ui->horizontalSlider_Smax->setValue(g);
    ui->horizontalSlider_Smax->blockSignals(false);

    ui->horizontalSlider_Vmax->blockSignals(true);
    ui->horizontalSlider_Vmax->setValue(r);
    ui->horizontalSlider_Vmax->blockSignals(false);
  }

  QString textSearch = jsonObject.value(ColorSettings[NTU_COLOR_SEARCH]).toString();
  ui->spinBox_count->setValue(jsonObject.value(ColorSettings[NTU_COLOR_COUNT]).toInt());
  ui->lineEdit_search->setText(textSearch);
  this->mRectangleSearch->setDimension(textSearch);

  this->blockSignals(false);

  this->onReceiverSettingChanged();
  ui->comboBox_toolFixture->setCurrentText(jsonObject.value("fixture").toString());

  Q_EMIT newFixtureTool(this->mToolAlgorithm, jsonObject.value("fixture").toString());

  Mat templateMat = NTUCVTools::jsonToMat(jsonObject["image"].toObject());
  if (!templateMat.empty())
  {
    mToolAlgorithm->setImageTemplate(templateMat);
  }
  this->onReceiverSettingChanged();
}

QJsonObject* NTUColorWidget::getJsonObject()
{
  this->mJsonObject->insert("image", NTUCVTools::matToJson(mToolAlgorithm->mImageTemplate));
  this->mJsonObject->insert("fixture", ui->comboBox_toolFixture->currentText());
  return this->mJsonObject;
}

void NTUColorWidget::onReceiverSettingChanged()
{
  int timeOut = ui->spinBox_timeOut->value();
  QString textSearch = ui->lineEdit_search->text();

  QString brgMin = tr("%1 %2 %3")
                       .arg(ui->horizontalSlider_hMin->value())
                       .arg(ui->horizontalSlider_Smin->value())
                       .arg(ui->horizontalSlider_Vmin->value());
  ui->lineEdit_minBGR->blockSignals(true);
  ui->lineEdit_minBGR->setText(brgMin);
  ui->lineEdit_minBGR->blockSignals(false);

  QString brgMax = tr("%1 %2 %3")
                       .arg(ui->horizontalSlider_Hmax->value())
                       .arg(ui->horizontalSlider_Smax->value())
                       .arg(ui->horizontalSlider_Vmax->value());

  ui->lineEdit_maxBGR->blockSignals(true);
  ui->lineEdit_maxBGR->setText(brgMax);
  ui->lineEdit_maxBGR->blockSignals(false);

  //  int b, g, r;
  //  if (cvtools::getBGRFromQString(brgMin, b, g, r))
  //  {
  //    QPalette palette;
  //    palette.setColor(QPalette::Base, QColor(r, g, b));
  //    ui->lineEdit_minBGR->setPalette(palette);
  //  }

  //  if (cvtools::getBGRFromQString(brgMax, b, g, r))
  //  {
  //    QPalette palette;
  //    palette.setColor(QPalette::Base, QColor(r, g, b));
  //    ui->lineEdit_maxBGR->setPalette(palette);
  //  }

  int countPixel = ui->spinBox_count->value();

  this->mJsonObject = new QJsonObject();
  this->mJsonObject->insert("name", this->mName);
  this->mJsonObject->insert("type", this->mToolType);

  this->mJsonObject->insert(ColorSettings[NTU_COLOR_HSV_MIN], QJsonValue::fromVariant(brgMin));
  this->mJsonObject->insert(ColorSettings[NTU_COLOR_HSV_MAX], QJsonValue::fromVariant(brgMax));
  this->mJsonObject->insert(ColorSettings[NTU_COLOR_COUNT], QJsonValue::fromVariant(countPixel));
  this->mJsonObject->insert(ColorSettings[NTU_COLOR_SEARCH], QJsonValue::fromVariant(textSearch));
  this->mJsonObject->insert(ColorSettings[NTU_COLOR_TIME_OUT], QJsonValue::fromVariant(timeOut));

  Q_EMIT newJSonSetting(mJsonObject);
  Q_EMIT newExureToolAlgorithm();
}

void NTUColorWidget::onReceiverVisibilityShape(bool value)
{
  if (value)
  {
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleSearch));
    this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  }
  else
  {
    this->mVideoWidget->removeShape(mName, mRectangleSearch);
    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);
  }
}

void NTUColorWidget::insertToolFixture(QString toolName)
{
  ui->comboBox_toolFixture->addItem(toolName);
  ui->comboBox_toolFixture->setItemData(ui->comboBox_toolFixture->count() - 1, Qt::AlignRight, Qt::TextAlignmentRole);
}

void NTUColorWidget::onReceiverResult(QString result)
{
  ui->lineEdit_result->setText(result);
  NTUToolWidget::onReceiverResult(result);
}

void NTUColorWidget::onUpdateRectangleSearch(QRect rect) { this->setTextFromRectToLineEdit(rect, ui->lineEdit_search); }

void NTUColorWidget::on_comboBox_toolFixture_currentIndexChanged(int index)
{
  Q_EMIT newFixtureTool(this->mToolAlgorithm, ui->comboBox_toolFixture->currentText());
  Q_EMIT newExureToolAlgorithm();
}

void NTUColorWidget::setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit)
{
  QString textRect = tr("%1 %2 %3 %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  lineEdit->setText(textRect);
}
