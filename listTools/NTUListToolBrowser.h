#ifndef NTULISTTOOLBROWSER_H
#define NTULISTTOOLBROWSER_H

#include <QObject>
#include <QTreeView>
#include <QStyledItemDelegate>
#include <QAbstractItemModel>
#include <QMouseEvent>

class NTUListToolManager;
class NTUListToolBrowser : public QTreeView
{
public:
    NTUListToolBrowser(QWidget *parent = nullptr);
    virtual void setModel(QAbstractItemModel *model) override;

    virtual void mousePressEvent(QMouseEvent *event) override;

//    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    NTUListToolManager *mCurrentListToolManager;
};




#endif // NTULISTTOOLBROWSER_H
