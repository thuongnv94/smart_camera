#ifndef NTUCOLORWIDGET_H
#define NTUCOLORWIDGET_H

#include "AlgorithmTools/NTUColor.h"
#include "NTUToolWidget.h"
#include "shapeTools/NTURectangle.h"
#include <QLineEdit>
#include <QWidget>
namespace Ui
{
class NTUColorWidget;
}

class NTUColorWidget : public NTUToolWidget
{
  Q_OBJECT

public:
  explicit NTUColorWidget(QWidget* parent = nullptr, QWidget* video = nullptr, QString toolName = "Color");
  ~NTUColorWidget();

public Q_SLOTS:
  virtual void updateSettingFromJsonObject(QJsonObject jsonObject) override;
  virtual QJsonObject* getJsonObject() override;
  virtual void onReceiverSettingChanged() override;
  virtual void onReceiverVisibilityShape(bool value) override;
  virtual void insertToolFixture(QString toolName) override;
  virtual void onReceiverResult(QString result) override;
  void onUpdateRectangleSearch(QRect rect);

private Q_SLOTS:
  void on_comboBox_toolFixture_currentIndexChanged(int index);

private:
  void setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit);

private:
  Ui::NTUColorWidget* ui;
  NTURectangle* mRectangleSearch;
};

#endif // NTUCOLORWIDGET_H
