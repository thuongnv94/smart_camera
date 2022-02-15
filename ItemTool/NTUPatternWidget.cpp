#include "NTUPatternWidget.h"
#include "SMVideoZoomWidget.h"
#include "cvtools.h"
#include "ui_NTUPatternWidget.h"

NTUPatternWidget::NTUPatternWidget(QWidget* parent, QWidget* video, QString toolName)
    : NTUToolWidget(parent, video), ui(new Ui::NTUPatternWidget), mSelectMode(0)
{
  ui->setupUi(this);
  this->setupCombobox(ui->comboBox_toolEnable);
  this->setupCombobox(ui->comboBox_toolFixture);
  //! [1]
  this->mName = toolName;
  this->mToolType = PATTERN;
  ui->lineEdit_name->setText(mName);
  //! [1]

  //! [2]
  this->mRectangleSearch = new NTURectangle("Search");
  this->mRectangleObject = new NTURectangle("Model", 60, 60, 100, 100);
  this->mToolAlgorithm = new NTUPattern(mName);

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
          &NTUPatternWidget::onReceiverSettingChanged);
  connect(ui->doubleSpinBox_verticalOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &NTUPatternWidget::onReceiverSettingChanged);
  connect(ui->doubleSpinBox_horizontalOffset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &NTUPatternWidget::onReceiverSettingChanged);
  connect(ui->spinBox_AngleTolerance, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUPatternWidget::onReceiverSettingChanged);
  connect(ui->spinBox_timeOut, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUPatternWidget::onReceiverSettingChanged);

  connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &NTUPatternWidget::onUpdateName);
  connect(mRectangleSearch, &NTURectangle::newUpdateRectangle, this, &NTUPatternWidget::onUpdateRectangleSearch);
  connect(mRectangleObject, &NTURectangle::newUpdateRectangle, this, &NTUPatternWidget::onUpdateRectangleObject);
  // connect(ui->comboBox_toolFixture, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
  // &NTUPatternWidget::onReceiverSettingChanged);

  connect(mToolAlgorithm, &NTUPattern::newImageTemplate, ui->imageShow, &NTUVideoWidget::showImageCV);
  connect(mToolAlgorithm, &NTUPattern::newExecutionTime, ui->lineEdit_executionTime, &QLineEdit::setText);
  //! [5]

  //! [6]
  this->onReceiverSettingChanged();
  this->mVideoWidget->drawImage();
  //! [6]
}

NTUPatternWidget::~NTUPatternWidget()
{
  delete ui;
  if (mRectangleSearch != nullptr)
  {
    delete mRectangleSearch;
    mRectangleSearch = nullptr;
  }
  if (mRectangleObject != nullptr)
  {
    delete mRectangleObject;
    mRectangleObject = nullptr;
  }
}

void NTUPatternWidget::onUpdateRectangleSearch(QRect rect) { this->setTextFromRectToLineEdit(rect, ui->lineEdit_search); }

void NTUPatternWidget::onUpdateRectangleObject(QRect rect) { this->setTextFromRectToLineEdit(rect, ui->lineEdit_object); }

void NTUPatternWidget::onReceiverVisibilityShape(bool value)
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

void NTUPatternWidget::onReceiverSettingChanged()
{
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
  this->mJsonObject->insert(PatternSettings[PATTERN_ANGLE_TOLERANCE], QJsonValue::fromVariant(angleTolerance));
  this->mJsonObject->insert(PatternSettings[PATTERN_ACCEPT_THRESHOLD], QJsonValue::fromVariant(aceptThreshold));
  this->mJsonObject->insert(PatternSettings[PATTERN_HORIZONTAL_OFFSET], QJsonValue::fromVariant(horizontalOffset));
  this->mJsonObject->insert(PatternSettings[PATTERN_VERTICAL_OFFSET], QJsonValue::fromVariant(verticalOffset));
  this->mJsonObject->insert(PatternSettings[PATTERN_SEARCH], QJsonValue::fromVariant(textSearch));
  this->mJsonObject->insert(PatternSettings[PATTERN_OBJECT], QJsonValue::fromVariant(textObject));
  this->mJsonObject->insert(PatternSettings[PATTERN_TIME_OUT], QJsonValue::fromVariant(timeOut));

  Q_EMIT newJSonSetting(mJsonObject);
  Q_EMIT newExureToolAlgorithm();
}

void NTUPatternWidget::onReceiverResult(QString result)
{
  ui->lineEdit_result->setText(result);
  NTUToolWidget::onReceiverResult(result);
}

void NTUPatternWidget::insertToolFixture(QString toolName)
{
  ui->comboBox_toolFixture->addItem(toolName);
  ui->comboBox_toolFixture->setItemData(ui->comboBox_toolFixture->count() - 1, Qt::AlignRight, Qt::TextAlignmentRole);
}

QJsonObject* NTUPatternWidget::getJsonObject()
{
  this->mJsonObject->insert("image", NTUCVTools::matToJson(mToolAlgorithm->mImageTemplate));
  this->mJsonObject->insert("fixture", ui->comboBox_toolFixture->currentText());
  return this->mJsonObject;
}

void NTUPatternWidget::updateSettingFromJsonObject(QJsonObject jsonObject)
{
  this->blockSignals(true);
  ui->spinBox_AngleTolerance->setValue(jsonObject.value(PatternSettings[PATTERN_ANGLE_TOLERANCE]).toInt());
  ui->doubleSpinBox_aceptThreshold->setValue(jsonObject.value(PatternSettings[PATTERN_ACCEPT_THRESHOLD]).toDouble());
  ui->doubleSpinBox_horizontalOffset->setValue(jsonObject.value(PatternSettings[PATTERN_HORIZONTAL_OFFSET]).toDouble());
  ui->doubleSpinBox_verticalOffset->setValue(jsonObject.value(PatternSettings[PATTERN_VERTICAL_OFFSET]).toDouble());
  QString textSearch = jsonObject.value(PatternSettings[PATTERN_SEARCH]).toString();
  QString textObject = jsonObject.value(PatternSettings[PATTERN_OBJECT]).toString();
  ui->lineEdit_search->setText(textSearch);
  ui->lineEdit_object->setText(textObject);
  this->mRectangleSearch->setDimension(textSearch);
  this->mRectangleObject->setDimension(textObject);
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

void NTUPatternWidget::setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit)
{
  QString textRect = tr("%1 %2 %3 %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  lineEdit->setText(textRect);
}

void NTUPatternWidget::on_pushButton_setObject_clicked()
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
}

void NTUPatternWidget::on_comboBox_toolFixture_currentIndexChanged(int index)
{
  Q_EMIT newFixtureTool(this->mToolAlgorithm, ui->comboBox_toolFixture->currentText());
  Q_EMIT newExureToolAlgorithm();
}
