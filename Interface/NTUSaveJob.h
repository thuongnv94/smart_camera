#ifndef NTUSAVEJOB_H
#define NTUSAVEJOB_H

#include <QGroupBox>
#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QSettings>

namespace Ui {
class NTUSaveJob;
}

class NTUSaveJob : public QGroupBox
{
    Q_OBJECT

public:
    explicit NTUSaveJob(QWidget *parent = nullptr);
    ~NTUSaveJob();

private Q_SLOTS:
    void on_pushButton_save_clicked();

Q_SIGNALS:
    void newSaveJobFile(QString fileName);
private:
    Ui::NTUSaveJob *ui;
};

#endif // NTUSAVEJOB_H
