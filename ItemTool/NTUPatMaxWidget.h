#ifndef NTUPATMAXWIDGET_H
#define NTUPATMAXWIDGET_H

#include <QLineEdit>
#include <QWidget>

#include "NTUToolWidget.h"
#include "shapeTools/NTURectangle.h"

namespace Ui
{
class NTUPatMaxWidget;
}

class NTUPatMaxWidget : public NTUToolWidget
{
  Q_OBJECT

public:
  explicit NTUPatMaxWidget(QWidget* parent = nullptr, QWidget* video = nullptr, QString toolName = "PatMax");
  virtual ~NTUPatMaxWidget() override;

public Q_SLOTS:

  virtual void onReceiverVisibilityShape(bool value) override;
  virtual void onReceiverSettingChanged() override;
  virtual void onReceiverResult(QString result) override;
  virtual void insertToolFixture(QString toolName) override;
  virtual QJsonObject* getJsonObject() override;
  virtual void updateSettingFromJsonObject(QJsonObject jsonObject) override;

  void onUpdateRectangleSearch(QRect rect);
  void onUpdateRectangleObject(QRect rect);

private slots:
  void on_comboBox_toolFixture_currentIndexChanged(const QString& arg1);

  void on_pushButton_setObject_clicked();

private:
  Ui::NTUPatMaxWidget* ui;

  void setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit);
  NTURectangle* mRectangleSearch;
  NTURectangle* mRectangleObject;

  int mSelectMode;
};

#endif // NTUPATMAXWIDGET_H
