#include "NTUCircleDetectWidget.h"
#include "AlgorithmTools/NTUCVTools.h"
#include "AlgorithmTools/NTUCircleDetect.h"
#include "SMVideoZoomWidget.h"
#include "ui_NTUCircleDetectWidget.h"

NTUCircleDetectWidget::NTUCircleDetectWidget(QWidget* parent, QWidget* video, QString toolName)
    : NTUToolWidget(parent, video), ui(new Ui::NTUCircleDetectWidget)
{
  ui->setupUi(this);
  this->setupCombobox(ui->comboBox_toolEnable);
  this->setupCombobox(ui->comboBox_toolFixture);
  this->setupCombobox(ui->comboBox_edgeTransition);
  this->setupCombobox(ui->comboBox_thresholdMethod);
  //! [1]
  this->mName = toolName;
  this->mToolType = CIRCLEDETECT;
  ui->lineEdit_name->setText(mName);
  //! [1]
  //! [2]
  this->mCircleOut = new NTUCircle("Out");
  this->mCircleIn = new NTUCircle("In", 100, 100, 50);
  this->mToolAlgorithm = new NTUCircleDetect(mName);
  //! [2]
  //! [3]
  this->mVideoWidget->addNewNTUTool(mName, mToolAlgorithm);
  this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  //! [3]

  this->setTextFromRectToLineEdit(mCircleIn->getRegtangle(), ui->lineEdit_in);
  this->setTextFromRectToLineEdit(mCircleOut->getRegtangle(), ui->lineEdit_out);

  this->onReceiverSettingChanged();


  //! [4]
  connect(ui->comboBox_thresholdMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &NTUCircleDetectWidget::onReceiverSettingChanged);
  connect(ui->spinBox_edgeContrast, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUCircleDetectWidget::onReceiverSettingChanged);
  connect(ui->spinBox_edgeWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUCircleDetectWidget::onReceiverSettingChanged);
  connect(ui->comboBox_edgeTransition, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &NTUCircleDetectWidget::onReceiverSettingChanged);
  connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &NTUCircleDetectWidget::onUpdateName);
  connect(mToolAlgorithm, &NTUCircleDetect::newExecutionTime, ui->lineEdit_executionTime, &QLineEdit::setText);

  connect(mCircleIn, &NTUCircle::newCenter, mCircleOut, &NTUCircle::onReceiverCenter);
  connect(mCircleOut, &NTUCircle::newCenter, mCircleIn, &NTUCircle::onReceiverCenter);
  connect(mCircleIn, &NTUCircle::newUpdateRectangle, this, &NTUCircleDetectWidget::onUpdateCircleIn);
  connect(mCircleOut, &NTUCircle::newUpdateRectangle, this, &NTUCircleDetectWidget::onUpdateCircleOut);

  connect(this, &NTUToolWidget::newJSonSetting, mToolAlgorithm, &NTUTools::onReadSettingFromJSon);
  //! [4]
  //! [5]
  this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mCircleOut));
  this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mCircleIn));
  //! [5]

  //! [7]
  this->mVideoWidget->drawImage();
  //! [7]
}

NTUCircleDetectWidget::~NTUCircleDetectWidget() { delete ui; }

void NTUCircleDetectWidget::onReceiverVisibilityShape(bool value)
{
  if (value)
  {
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mCircleOut));
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mCircleIn));
    this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  }
  else
  {
    this->mVideoWidget->removeShape(mName, mCircleIn);
    this->mVideoWidget->removeShape(mName, mCircleOut);
    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);
  }
}

void NTUCircleDetectWidget::onReceiverSettingChanged()
{
  int thresholdMethod = ui->comboBox_thresholdMethod->currentIndex();
  int edgeContrast = ui->spinBox_edgeContrast->value();
  int edgeTransition = ui->comboBox_edgeTransition->currentIndex();
  int edgeWidth = ui->spinBox_edgeWidth->value();
  QString circleIn = ui->lineEdit_in->text();
  QString circleOut = ui->lineEdit_out->text();

  this->mJsonObject = new QJsonObject();
  this->mJsonObject->insert("name", this->mName);
  this->mJsonObject->insert("type", this->mToolType);
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_THRESHOLD_MEDTHOD], QJsonValue::fromVariant(thresholdMethod));
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_EDGE_CONTRAST], QJsonValue::fromVariant(edgeContrast));
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_EDGETRANSITION], QJsonValue::fromVariant(edgeTransition));
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_EDGE_WIDTH], QJsonValue::fromVariant(edgeWidth));
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_CERCLE_IN], QJsonValue::fromVariant(circleIn));
  mJsonObject->insert(CircleDetectSettings[CIRCLE_DETECT_CERCLE_OUT], QJsonValue::fromVariant(circleOut));

  //! [3]
  Q_EMIT newJSonSetting(mJsonObject);
  Q_EMIT newExureToolAlgorithm();
  //! [3]
}

void NTUCircleDetectWidget::onReceiverResult(QString result)
{
  ui->lineEdit_result->setText(result);
  NTUToolWidget::onReceiverResult(result);
}

void NTUCircleDetectWidget::insertToolFixture(QString toolName) { ui->comboBox_toolFixture->addItem(toolName);
                                                                  ui->comboBox_toolFixture->setItemData(ui->comboBox_toolFixture->count() - 1, Qt::AlignRight, Qt::TextAlignmentRole);
                                                                }

void NTUCircleDetectWidget::updateSettingFromJsonObject(QJsonObject jsonObject)
{
    this->blockSignals(true);
    ui->comboBox_thresholdMethod->setCurrentIndex(jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_THRESHOLD_MEDTHOD]).toInt());
    ui->spinBox_edgeContrast->setValue(jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_EDGE_CONTRAST]).toInt());
    ui->comboBox_edgeTransition->setCurrentIndex(jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_EDGETRANSITION]).toInt());
    ui->spinBox_edgeWidth->setValue(jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_EDGE_WIDTH]).toInt());
    QString circleIn = jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_CERCLE_IN]).toString();
    QString circleOut = jsonObject.value(CircleDetectSettings[CIRCLE_DETECT_CERCLE_OUT]).toString();
    ui->lineEdit_in->setText(circleIn);
    ui->lineEdit_out->setText(circleOut);
    this->mCircleIn->setDimension(circleIn);
    this->mCircleOut->setDimension(circleOut);
    this->blockSignals(false);

    ui->comboBox_toolFixture->setCurrentText(jsonObject["fixture"].toString());
    Q_EMIT newFixtureTool(this->mToolAlgorithm, jsonObject.value("fixture").toString());

    this->onReceiverSettingChanged();
}

QJsonObject *NTUCircleDetectWidget::getJsonObject()
{
    this->mJsonObject->insert("fixture", ui->comboBox_toolFixture->currentText());
    return mJsonObject;
}

void NTUCircleDetectWidget::onUpdateCircleOut(QRect rect)
{
  this->setTextFromRectToLineEdit(rect, ui->lineEdit_out);
  this->setTextFromRectToLineEdit(mCircleIn->getRegtangle(), ui->lineEdit_in);

  this->onReceiverSettingChanged();
}

void NTUCircleDetectWidget::onUpdateCircleIn(QRect rect)
{
  this->setTextFromRectToLineEdit(rect, ui->lineEdit_in);
  this->setTextFromRectToLineEdit(mCircleOut->getRegtangle(), ui->lineEdit_out);

  this->onReceiverSettingChanged();
}

void NTUCircleDetectWidget::setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit)
{
  QString text = QString("%1 %2 %3").arg(rect.x() + rect.width() / 2.f).arg(rect.y() + rect.height() / 2.f).arg(rect.width() / 2.f);
  lineEdit->setText(text);
}

void NTUCircleDetectWidget::on_comboBox_toolFixture_currentIndexChanged(const QString &arg1)
{
    Q_EMIT newFixtureTool(this->mToolAlgorithm, arg1);
    Q_EMIT newExureToolAlgorithm();
}
