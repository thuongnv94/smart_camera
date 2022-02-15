#ifndef NTUBLOBWIDGET_H
#define NTUBLOBWIDGET_H

#include <QWidget>
#include "NTUToolWidget.h"
#include "shapeTools/NTURectangle.h"
#include "AlgorithmTools/NTUBlob.h"

namespace Ui {
class NTUBlobWidget;
}

class NTUBlobWidget : public NTUToolWidget
{
    Q_OBJECT

public:
    explicit NTUBlobWidget(QWidget *parent = nullptr, QWidget*video = nullptr, QString toolName = "Blob");
    ~NTUBlobWidget();

public Q_SLOTS:
    virtual void onReceiverVisibilityShape(bool value) override;
    virtual void onReceiverSettingChanged() override;
    virtual void onReceiverResult(QString result) override;
    virtual void insertToolFixture(QString toolName) override;
    virtual QJsonObject * getJsonObject() override;
    virtual void updateSettingFromJsonObject(QJsonObject jsonObject) override;
    void onUpdateRectangleSearch(QRect rect);

private slots:
    void on_comboBox_toolFixture_currentTextChanged(const QString &arg1);
    void onReceiverMat(Mat image);

private:
    void setTextFromRectToLineEdit(QRect rect, QLineEdit* lineEdit);

    Ui::NTUBlobWidget *ui;
    NTURectangle *mRectangleSearch;
};

#endif // NTUBLOBWIDGET_H
