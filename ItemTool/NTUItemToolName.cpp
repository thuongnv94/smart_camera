#include "NTUItemToolName.h"

NTUItemToolName::NTUItemToolName(QString name, NTUTOOLS type):QStandardItem(name)
{
    QFont font = this->font();
    font.setPointSize(11);
    this->setFont(font);

    switch (type) {
    case PATTERN:
        this->setText("Pattern");
        break;
    case BLOB:
        this->setText("Blob");
        break;
    case CIRCLEDETECT:
        this->setText("Circle");
        break;
    case PATMAX:
        this->setText("PatMax");
        break;
    case COLOR:
        this->setText("Color");
        break;
    }
}
