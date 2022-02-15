#ifndef NTUVideoWidget_H
#define NTUVideoWidget_H

#include <QLabel>
#include <QResizeEvent>

#include <opencv2/opencv.hpp>

class NTUVideoWidget : public QLabel{

    Q_OBJECT
    public:
        explicit NTUVideoWidget(QWidget *parent = 0);
    public Q_SLOTS:
        void showImageCV(cv::Mat image);
        void resizeEvent(QResizeEvent *event);
    private:
        QPixmap pixmap;
};

#endif // NTUVideoWidget_H
