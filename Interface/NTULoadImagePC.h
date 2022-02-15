#ifndef NTULOADIMAGEPC_H
#define NTULOADIMAGEPC_H

#include <QWidget>
#include <QSettings>
#include <QGroupBox>
#include <QDir>
#include <QFileDialog>

namespace Ui {
class NTULoadImagePC;
}

class NTULoadImagePC : public QGroupBox
{
    Q_OBJECT

public:
    explicit NTULoadImagePC(QWidget *parent = nullptr);
    ~NTULoadImagePC();

    void loadDefaultImageFromPath();

    Q_SIGNALS:
        void newDirectory(QDir directory);
private Q_SLOTS:
    void on_pushButton_choseDirectory_clicked();

private:
    Ui::NTULoadImagePC *ui;
};

#endif // NTULOADIMAGEPC_H
