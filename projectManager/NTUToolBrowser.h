#ifndef NTUTOOLBROWSER_H
#define NTUTOOLBROWSER_H

#include <QObject>
#include <QTreeView>
#include <QStyledItemDelegate>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QMouseEvent>
#include <QMenu>

class NTUProjectManager;
class NTUToolBrowser : public QTreeView
{
    Q_OBJECT

public:
    NTUToolBrowser(QWidget *parent = nullptr);

    void removeModel();

    virtual void setModel(QAbstractItemModel *model)override;

    virtual void mousePressEvent(QMouseEvent *event) override;
private Q_SLOTS:
    void contextualMenuTreeView(const QPoint& point);
    void onReceiverDeleteFromIndex();


Q_SIGNALS:
    void newClearVBoxLayout();
    void newShowItemFromIndex(QModelIndex index);
    void newDeleteItem(QModelIndex index);

private:
    NTUProjectManager *mCurrentProjectManager;
};

class BackgroundDelegate : public QStyledItemDelegate
{
public:
    explicit BackgroundDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


#endif // NTUTOOLBROWSER_H
