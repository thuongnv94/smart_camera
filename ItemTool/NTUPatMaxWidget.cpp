#include "NTUPatMaxWidget.h"
#include "AlgorithmTools/NTUPatMax.h"
#include "SMVideoZoomWidget.h"
#include "cvtools.h"
#include "ui_NTUPatMaxWidget.h"

NTUPatMaxWidget::NTUPatMaxWidget(QWidget* parent, QWidget* video, QString toolName)
    : NTUToolWidget(parent, video), ui(new Ui::NTUPatMaxWidget), mSelectMode(0)
{
  ui->setupUi(this);
  this->setupCombobox(ui->comboBox_toolEnable);
  this->setupCombobox(ui->comboBox_toolFixture);
  //! [1]
  this->mName = toolName;
  this->mToolType = PATMAX;
  ui->lineEdit_name->setText(mName);
  //! [1]

  //! [2]
  this->mRectangleSearch = new NTURectangle("Search");
  this->mRectangleObject = new NTURectangle("Model", 60, 60, 100, 100);
  this->mToolAlgorithm = new NTUPatMax(mName);

  //! [2]

  //! [3]
  this->mVideoWidget->addNewNTUTool(mName, mToolAlgorithm);
  this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  //! [3]

  //! [4]
  this->setTextFromRectToLineEdit(mRectangleSearch->getRegtangle(), ui->lineEdit_search);
  this->setTextFromRectToLineEdit(mRectangleObject->getRegtangle(), ui->lineEdit_object);
  //! [4]

  //! [5]
  connect(this, &NTUToolWidget::newJSonSetting, mToolAlgorithm, &NTUTools::onReadSettingFromJSon);

  connect(ui->doubleSpinBox_aceptThreshold, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &NTUPatMaxWidget::onReceiverSettingChanged);
  connect(ui->doubleSpinBox_verticalOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &NTUPatMaxWidget::onReceiverSettingChanged);
  connect(ui->doubleSpinBox_horizontalOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &NTUPatMaxWidget::onReceiverSettingChanged);
  connect(ui->spinBox_AngleTolerance, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUPatMaxWidget::onReceiverSettingChanged);
  connect(ui->spinBox_timeOut, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUPatMaxWidget::onReceiverSettingChanged);

  connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &NTUPatMaxWidget::onUpdateName);
  connect(mRectangleSearch, &NTURectangle::newUpdateRectangle, this, &NTUPatMaxWidget::onUpdateRectangleSearch);
  connect(mRectangleObject, &NTURectangle::newUpdateRectangle, this, &NTUPatMaxWidget::onUpdateRectangleObject);
  // connect(ui->comboBox_toolFixture, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  // &NTUPatternWidget::onReceiverSettingChanged);

  connect(mToolAlgorithm, &NTUPatMax::newImageTemplate, ui->imageShow, &NTUVideoWidget::showImageCV);
  connect(mToolAlgorithm, &NTUPatMax::newExecutionTime, ui->lineEdit_executionTime, &QLineEdit::setText);
  //! [5]

  //! [6]
  this->onReceiverSettingChanged();
  this->mVideoWidget->drawImage();
  //! [6]
  //!
}

NTUPatMaxWidget::~NTUPatMaxWidget()
{
  delete ui;
  if (mRectangleSearch != nullptr)
  {
    mRectangleSearch = nullptr;
    delete mRectangleSearch;
  }
  if (mRectangleObject != nullptr)
  {
    mRectangleObject = nullptr;
    delete mRectangleObject;
  }
  NTUToolWidget::~NTUToolWidget();
}

void NTUPatMaxWidget::onReceiverVisibilityShape(bool value)
{
  if (value)
  {
    switch (mSelectMode)
    {
    case 0:
      break;
    case 1:
      this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleSearch));
      this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleObject));
      break;
    case 2:
      this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleObject));
      break;
    default:
      this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
      break;
    }
  }
  else
  {
    this->mVideoWidget->removeShape(mName, mRectangleObject);
    this->mVideoWidget->removeShape(mName, mRectangleSearch);

    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);
  }
}

void NTUPatMaxWidget::onReceiverSettingChanged()
{
  int contrastThreshold = ui->spinBox_contrastThreshold->value();
  int angleTolerance = ui->spinBox_AngleTolerance->value();
  double aceptThreshold = ui->doubleSpinBox_aceptThreshold->value();
  double horizontalOffset = ui->doubleSpinBox_horizontalOffset->value();
  double verticalOffset = ui->doubleSpinBox_verticalOffset->value();
  int timeOut = ui->spinBox_timeOut->value();
  QString textSearch = ui->lineEdit_search->text();
  QString textObject = ui->lineEdit_object->text();

  this->mJsonObject = new QJsonObject();
  this->mJsonObject->insert("name", this->mName);
  this->mJsonObject->insert("type", this->mToolType);
  this->mJsonObject->insert(PatMaxSettings[PATMAX_CONTRAST_THRESHOLD], QJsonValue::fromVariant(contrastThreshold));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_ANGLE_TOLERANCE], QJsonValue::fromVariant(angleTolerance));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_ACCEPT_THRESHOLD], QJsonValue::fromVariant(aceptThreshold));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_HORIZONTAL_OFFSET], QJsonValue::fromVariant(horizontalOffset));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_VERTICAL_OFFSET], QJsonValue::fromVariant(verticalOffset));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_SEARCH], QJsonValue::fromVariant(textSearch));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_OBJECT], QJsonValue::fromVariant(textObject));
  this->mJsonObject->insert(PatMaxSettings[PATMAX_TIME_OUT], QJsonValue::fromVariant(timeOut));

  Q_EMIT newJSonSetting(mJsonObject);
  Q_EMIT newExureToolAlgorithm();
}

void NTUPatMaxWidget::onReceiverResult(QString result)
{
  ui->lineEdit_result->setText(result);
  NTUToolWidget::onReceiverResult(result);
}

void NTUPatMaxWidget::insertToolFixture(QString toolName)
{
  ui->comboBox_toolFixture->addItem(toolName);
  ui->comboBox_toolFixture->setItemData(ui->comboBox_toolFixture->count() - 1, Qt::AlignRight, Qt::TextAlignmentRole);
}

QJsonObject* NTUPatMaxWidget::getJsonObject()
{
  this->mJsonObject->insert("image", NTUCVTools::matToJson(mToolAlgorithm->mImageTemplate));
  this->mJsonObject->insert("fixture", ui->comboBox_toolFixture->currentText());
  return this->mJsonObject;
}

void NTUPatMaxWidget::updateSettingFromJsonObject(QJsonObject jsonObject)
{
  this->blockSignals(true);
  ui->spinBox_contrastThreshold->setValue(jsonObject.value(PatMaxSettings[PATMAX_CONTRAST_THRESHOLD]).toInt());
  ui->spinBox_AngleTolerance->setValue(jsonObject.value(PatMaxSettings[PATMAX_ANGLE_TOLERANCE]).toInt());
  ui->doubleSpinBox_aceptThreshold->setValue(jsonObject.value(PatMaxSettings[PATMAX_ACCEPT_THRESHOLD]).toDouble());
  ui->doubleSpinBox_horizontalOffset->setValue(jsonObject.value(PatMaxSettings[PATMAX_HORIZONTAL_OFFSET]).toDouble());
  ui->doubleSpinBox_verticalOffset->setValue(jsonObject.value(PatMaxSettings[PATMAX_VERTICAL_OFFSET]).toDouble());
  QString rectSearch = jsonObject.value(PatMaxSettings[PATMAX_SEARCH]).toString();
  QString rectcObj = jsonObject.value(PatMaxSettings[PATMAX_OBJECT]).toString();

  this->mRectangleSearch->setDimension(rectSearch);
  this->mRectangleObject->setDimension(rectcObj);
  ui->lineEdit_search->setText(rectSearch);
  ui->lineEdit_object->setText(rectcObj);
  this->blockSignals(false);

  ui->comboBox_toolFixture->setCurrentText(jsonObject.value("fixture").toString());
  this->onReceiverSettingChanged();

  Q_EMIT newFixtureTool(this->mToolAlgorithm, jsonObject.value("fixture").toString());

  Mat templateMat = NTUCVTools::jsonToMat(jsonObject["image"].toObject());
  if (!templateMat.empty())
  {

    mToolAlgorithm->setImageTemplate(templateMat);
  }

  this->onReceiverSettingChanged();
}

void NTUPatMaxWidget::onUpdateRectangleSearch(QRect rect) { this->setTextFromRectToLineEdit(rect, ui->lineEdit_search); }

void NTUPatMaxWidget::onUpdateRectangleObject(QRect rect) { this->setTextFromRectToLineEdit(rect, ui->lineEdit_object); }

void NTUPatMaxWidget::setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit)
{
  QString textRect = tr("%1 %2 %3 %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  lineEdit->setText(textRect);
}

void NTUPatMaxWidget::on_comboBox_toolFixture_currentIndexChanged(const QString& arg1)
{
  Q_EMIT newFixtureTool(this->mToolAlgorithm, ui->comboBox_toolFixture->currentText());
  Q_EMIT newExureToolAlgorithm();
}

void NTUPatMaxWidget::on_pushButton_setObject_clicked()
{
  switch (mSelectMode)
  {
  case 0:
    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleSearch));
    this->onReceiverSettingChanged();
    mSelectMode++;
    break;
  case 1:
  {
    this->mVideoWidget->removeShape(mName, mRectangleSearch);
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleObject));
    this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
    ui->pushButton_setObject->setText("Set Object");
    mSelectMode++;
    this->onReceiverSettingChanged();
    break;
  }
  case 2:
  {
    ui->pushButton_setObject->setText("Train");
    this->onReceiverSettingChanged();
    mSelectMode++;
    break;
  }
  case 3:
  {
    this->mVideoWidget->removeShape(mName, mRectangleObject);
    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);

    cv::Mat mat_image = dynamic_cast<SMVideoZoomWidget*>(this->mVideoWidget)->getImage();
    mToolAlgorithm->setImageTemplate(mat_image);
    ui->pushButton_setObject->setText("Set Region");
    this->onReceiverSettingChanged();
    mSelectMode = 0;
    break;
  }
  }
  QApplication::processEvents();
}
