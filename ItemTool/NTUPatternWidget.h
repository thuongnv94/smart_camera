#ifndef NTUPATTERNWIDGET_H
#define NTUPATTERNWIDGET_H
#include "AlgorithmTools/NTUPattern.h"
#include "NTUToolWidget.h"
#include "shapeTools/NTURectangle.h"
#include <QLineEdit>
#include <QWidget>

namespace Ui
{
class NTUPatternWidget;
}

class NTUPatternWidget : public NTUToolWidget
{
  Q_OBJECT

public:
  explicit NTUPatternWidget(QWidget* parent = nullptr, QWidget* video = nullptr, QString toolName = "Pattern");
  virtual ~NTUPatternWidget();

public Q_SLOTS:

  virtual void onReceiverVisibilityShape(bool value) override;
  virtual void onReceiverSettingChanged() override;
  virtual void onReceiverResult(QString result) override;
  virtual void insertToolFixture(QString toolName) override;
  virtual QJsonObject* getJsonObject() override;
  virtual void updateSettingFromJsonObject(QJsonObject jsonObject) override;

  void onUpdateRectangleSearch(QRect rect);
  void onUpdateRectangleObject(QRect rect);

private Q_SLOTS:
  void on_pushButton_setObject_clicked();

  void on_comboBox_toolFixture_currentIndexChanged(int index);

private:
  void setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit);
  Ui::NTUPatternWidget* ui;
  NTURectangle* mRectangleSearch;
  NTURectangle* mRectangleObject;

  int mSelectMode;
};

#endif // NTUPATTERNWIDGET_H
