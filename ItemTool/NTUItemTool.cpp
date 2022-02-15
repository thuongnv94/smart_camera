#include "NTUItemTool.h"
#include "NTUToolWidget.h"


NTUItemTool::NTUItemTool( QString name, NTUToolWidget *widget):QStandardItem(name)
{
    QFont font = this->font();
    font.setPointSize(11);
    this->setFont(font);

    this->mItemToolWidget = dynamic_cast<NTUToolWidget*>(widget);
    //this->setText(mItemToolWidget->getName());
    this->mItemToolWidget->setNTUItemTool(this);

}


NTUItemTool::~NTUItemTool()
{

}


