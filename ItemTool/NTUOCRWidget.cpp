#include "NTUOCRWidget.h"
#include "ui_NTUOCRWidget.h"

NTUOCRWidget::NTUOCRWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::NTUOCRWidget)
{
    ui->setupUi(this);
}

NTUOCRWidget::~NTUOCRWidget()
{
    delete ui;
}
