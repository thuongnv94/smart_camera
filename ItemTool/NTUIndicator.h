#ifndef NTUINDICATOR_H
#define NTUINDICATOR_H

#include <QObject>
#include <QStandardItem>
enum ItemIndocator
{
    NONE, OK, NG
};
class NTUIndicator: public QStandardItem
{
public:
    NTUIndicator(QString name = nullptr, ItemIndocator indicator = NONE);
    void setStatus(ItemIndocator idicator);
};

Q_DECLARE_METATYPE(NTUIndicator)
#endif // NTUINDICATOR_H
