#ifndef NTUITEMRESULT_H
#define NTUITEMRESULT_H
#include <QStandardItem>
#include <QString>
class NTUItemResult :public QStandardItem
{
public:
    NTUItemResult(QString name = "");

    void setResult(QString result);
};

#endif // NTUITEMRESULT_H
