#include "NTUItemResult.h"

NTUItemResult::NTUItemResult(QString name ):QStandardItem(name)
{
    QFont font = this->font();
    font.setPointSize(11);
    this->setFont(font);
    this->setText("");
}

void NTUItemResult::setResult(QString result)
{
    this->setText(result);
}
