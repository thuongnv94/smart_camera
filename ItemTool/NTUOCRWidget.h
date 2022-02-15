#ifndef NTUOCRWIDGET_H
#define NTUOCRWIDGET_H

#include <QGroupBox>

namespace Ui {
class NTUOCRWidget;
}

class NTUOCRWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit NTUOCRWidget(QWidget *parent = nullptr);
    ~NTUOCRWidget();

private:
    Ui::NTUOCRWidget *ui;
};

#endif // NTUOCRWIDGET_H
