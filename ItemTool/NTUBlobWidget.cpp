#include "NTUBlobWidget.h"
#include "AlgorithmTools/NTUCVTools.h"
#include "SMVideoZoomWidget.h"
#include "ui_NTUBlobWidget.h"

NTUBlobWidget::NTUBlobWidget(QWidget* parent, QWidget* video, QString toolName) : NTUToolWidget(parent, video), ui(new Ui::NTUBlobWidget)
{
  ui->setupUi(this);

  this->setupCombobox(ui->comboBox_toolFixture);
  this->setupCombobox(ui->comboBox_blobColor);
  this->setupCombobox(ui->comboBox_toolEnable);
  this->setupCombobox(ui->comboBox_thresholdMode);

  //! [1]
  this->mName = toolName;
  this->mToolType = BLOB;
  ui->lineEdit_name->setText(mName);
  //! [1]

  //! [2]
  this->mRectangleSearch = new NTURectangle("Search");
  this->mToolAlgorithm = new NTUBlob(mName);
  //! [2]
  //! [3]
  this->mVideoWidget->addNewNTUTool(mName, mToolAlgorithm);
  this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
  //! [3]
  //! [4]
  this->setTextFromRectToLineEdit(mRectangleSearch->getRegtangle(), ui->lineEdit_search);
  //! [4]
  //! [5]
  connect(this, &NTUToolWidget::newJSonSetting, mToolAlgorithm, &NTUTools::onReadSettingFromJSon);
  connect(ui->comboBox_thresholdMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NTUBlobWidget::onReceiverSettingChanged);
  connect(ui->comboBox_blobColor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NTUBlobWidget::onReceiverSettingChanged);
  connect(ui->spinBox_blobThreshold, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUBlobWidget::onReceiverSettingChanged);
  connect(ui->spinBox_minimumArea, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUBlobWidget::onReceiverSettingChanged);
  connect(ui->spinBox_maximumArea, QOverload<int>::of(&QSpinBox::valueChanged), this, &NTUBlobWidget::onReceiverSettingChanged);
  connect(mRectangleSearch, &NTURectangle::newUpdateRectangle, this, &NTUBlobWidget::onUpdateRectangleSearch);
  connect(ui->lineEdit_name, &QLineEdit::textChanged, this, &NTUBlobWidget::onUpdateName);
  connect(mToolAlgorithm, &NTUBlob::newExecutionTime, ui->lineEdit_executionTime, &QLineEdit::setText);
  connect(mVideoWidget, &SMVideoZoomWidget::newImageInput, this, &NTUBlobWidget::onReceiverMat);

  //! [5]

  this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleSearch));

  ui->widget_histogram->addGraph();

  //! [6]
  this->onReceiverSettingChanged();
  this->mVideoWidget->drawImage();
  //! [6]
}

NTUBlobWidget::~NTUBlobWidget() { delete ui; }

void NTUBlobWidget::onReceiverVisibilityShape(bool value)
{
  if (value)
  {
    this->mVideoWidget->addNewShape(mName, dynamic_cast<NTUShape*>(mRectangleSearch));
    this->mVideoWidget->addNewNTUToolDrawRegion(mName, mToolAlgorithm);
    connect(mVideoWidget, &SMVideoZoomWidget::newImageInput, this, &NTUBlobWidget::onReceiverMat);
  }
  else
  {
    this->mVideoWidget->removeShape(mName, mRectangleSearch);
    this->mVideoWidget->removeNTUToolDrawRegion(mName, mToolAlgorithm);
    disconnect(mVideoWidget, &SMVideoZoomWidget::newImageInput, this, &NTUBlobWidget::onReceiverMat);
  }
}

void NTUBlobWidget::onReceiverSettingChanged()
{
  int thresholdMode = ui->comboBox_thresholdMode->currentIndex();
  int Blobthreshold = ui->spinBox_blobThreshold->value();
  int blobColor = ui->comboBox_blobColor->currentIndex();
  int minimumArea = ui->spinBox_minimumArea->value();
  int maximumArea = ui->spinBox_maximumArea->value();
  QString textSearch = ui->lineEdit_search->text();

  this->mJsonObject = new QJsonObject();
  this->mJsonObject->insert("name", this->mName);
  this->mJsonObject->insert("type", this->mToolType);
  mJsonObject->insert(BlobSettings[BLOB_THRESHOLD_MODE], QJsonValue::fromVariant(thresholdMode));
  mJsonObject->insert(BlobSettings[BLOB_COLOR], QJsonValue::fromVariant(blobColor));
  mJsonObject->insert(BlobSettings[BLOB_THRESHOLD], QJsonValue::fromVariant(Blobthreshold));
  mJsonObject->insert(BlobSettings[BLOB_MINIMUM_AREA], QJsonValue::fromVariant(minimumArea));
  mJsonObject->insert(BlobSettings[BLOB_MAXIMUM_AREA], QJsonValue::fromVariant(maximumArea));
  mJsonObject->insert(BlobSettings[BLOB_SEARCH], QJsonValue::fromVariant(textSearch));

  //! [6]
  Mat image = mVideoWidget->getImage();
  if (!image.empty())
    this->onReceiverMat(image);
  //! [6]

  Q_EMIT newJSonSetting(mJsonObject);
  Q_EMIT newExureToolAlgorithm();
}

void NTUBlobWidget::onReceiverResult(QString result)
{
  ui->lineEdit_result->setText(result);
  NTUToolWidget::onReceiverResult(result);
}

void NTUBlobWidget::insertToolFixture(QString toolName)
{
  ui->comboBox_toolFixture->addItem(toolName);
  ui->comboBox_toolFixture->setItemData(ui->comboBox_toolFixture->count() - 1, Qt::AlignRight, Qt::TextAlignmentRole);
}

QJsonObject *NTUBlobWidget::getJsonObject()
{
    this->mJsonObject->insert("fixture", ui->comboBox_toolFixture->currentText());
    return mJsonObject;
}

void NTUBlobWidget::updateSettingFromJsonObject(QJsonObject jsonObject)
{
    this->blockSignals(true);
    ui->comboBox_thresholdMode->setCurrentIndex(jsonObject.value(BlobSettings[BLOB_THRESHOLD_MODE]).toInt());
    ui->comboBox_blobColor->setCurrentIndex(jsonObject.value(BlobSettings[BLOB_COLOR]).toInt());
    ui->spinBox_blobThreshold->setValue(jsonObject.value(BlobSettings[BLOB_THRESHOLD]).toInt());
    ui->spinBox_minimumArea->setValue(jsonObject.value(BlobSettings[BLOB_MINIMUM_AREA]).toInt());
    ui->spinBox_maximumArea->setValue(jsonObject.value(BlobSettings[BLOB_MAXIMUM_AREA]).toInt());
    QString textSearch = jsonObject.value(BlobSettings[BLOB_SEARCH]).toString();
    ui->lineEdit_search->setText(textSearch);
    this->mRectangleSearch->setDimension(textSearch);
    this->blockSignals(false);

    ui->comboBox_toolFixture->setCurrentText(jsonObject.value("fixture").toString());
    Q_EMIT newFixtureTool(this->mToolAlgorithm, jsonObject.value("fixture").toString());

    this->onReceiverSettingChanged();
}

void NTUBlobWidget::onUpdateRectangleSearch(QRect rect)
{
  this->setTextFromRectToLineEdit(rect, ui->lineEdit_search);
  this->onReceiverSettingChanged();
}

void NTUBlobWidget::setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit)
{
  QString textRect = tr("%1 %2 %3 %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
  lineEdit->setText(textRect);
}

void NTUBlobWidget::on_comboBox_toolFixture_currentTextChanged(const QString& arg1)
{
  Q_EMIT newFixtureTool(this->mToolAlgorithm, arg1);
  Q_EMIT newExureToolAlgorithm();
}

void NTUBlobWidget::onReceiverMat(Mat image)
{
  Rect2f rectROI;
  if (!cvtools::getRectFromQString(ui->lineEdit_search->text(), rectROI)) // not corect
    return;
  Mat image_ROI;
  image(rectROI).copyTo(image_ROI);
  vector<float> data = NTUCVTools::calculatorHistogram(image_ROI);

  ui->widget_histogram->graph(0)->setPen(QPen(Qt::green)); // line color red for second graph
  ui->widget_histogram->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
  ui->widget_histogram->xAxis->setLabel(tr("Intensity"));
  ui->widget_histogram->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
  ui->widget_histogram->yAxis->setLabel(tr("Pixels"));

  // generate some points of data (y0 for first, y1 for second graph):
  QVector<double> x(data.size()), yr(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    x[i] = i;
    yr[i] = data[i]; // exponentially decaying cosine
  }
  // configure right and top axis to show ticks but no labels:
  ui->widget_histogram->xAxis->setVisible(true);
  ui->widget_histogram->xAxis->setTickLabels(true);
  ui->widget_histogram->yAxis->setVisible(true);
  ui->widget_histogram->yAxis->setTickLabels(true);
  // make left and bottom axes always transfer their ranges to right and top
  // axes:

  // pass data points to graphs:
  ui->widget_histogram->graph(0)->setData(x, yr);

  // let the ranges scale themselves so graph 0 fits perfectly in the visible
  // area:
  ui->widget_histogram->graph(0)->rescaleAxes();
  // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller
  // than graph 0):

  // zoom out a bit:
  ui->widget_histogram->yAxis->scaleRange(1.1, ui->widget_histogram->yAxis->range().center());
  ui->widget_histogram->xAxis->scaleRange(1.1, ui->widget_histogram->xAxis->range().center());

  // Note: we could have also just called customPlot->rescaleAxes(); instead
  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select
  // graphs by clicking:
  // ui->widget_histogram->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  ui->widget_histogram->axisRect()->setupFullAxesBox();
  ui->widget_histogram->replot();
}
