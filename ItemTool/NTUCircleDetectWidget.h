#ifndef NTUCIRCLEDETECTWIDGET_H
#define NTUCIRCLEDETECTWIDGET_H

#include "AlgorithmTools/NTUCircleDetect.h"
#include "NTUToolWidget.h"
#include "shapeTools/NTUCircle.h"
#include <QWidget>

namespace Ui
{
class NTUCircleDetectWidget;
}

class NTUCircleDetectWidget : public NTUToolWidget
{
  Q_OBJECT

public:
  explicit NTUCircleDetectWidget(QWidget* parent = nullptr, QWidget* video = nullptr,
                                 QString toolName = "Circle");
  ~NTUCircleDetectWidget();

public Q_SLOTS:

  ///
  /// \brief onReceiverVisibilityShape
  /// \param value
  ///
  virtual void onReceiverVisibilityShape(bool value) override;

  ///
  /// \brief onReceiverSettingChanged
  ///
  virtual void onReceiverSettingChanged() override;

  ///
  /// \brief onReceiverResult
  /// \param result
  ///
  virtual void onReceiverResult(QString result) override;

  ///
  /// \brief insertToolFixture
  /// \param toolName
  ///
  virtual void insertToolFixture(QString toolName) override;

  ///
  /// \brief updateSettingFromJsonObject
  /// \param jsonObject
  ///
  virtual void updateSettingFromJsonObject(QJsonObject jsonObject) override;

  ///
  /// \brief getJsonObject
  /// \return
///
  virtual QJsonObject * getJsonObject() override;

private Q_SLOTS:
  ///
  /// \brief onUpdateCircleOut
  /// \param rect
  ///
  void onUpdateCircleOut(QRect rect);

  ///
  /// \brief onUpdateCircleIn
  /// \param rect
  ///
  void onUpdateCircleIn(QRect rect);

  void on_comboBox_toolFixture_currentIndexChanged(const QString &arg1);

  private:
  void setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit);

  Ui::NTUCircleDetectWidget* ui;
  NTUCircle* mCircleIn;
  NTUCircle* mCircleOut;
};

#endif // NTUCIRCLEDETECTWIDGET_H
