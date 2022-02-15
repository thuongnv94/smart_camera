#include "NTUVideoWidget.h"
#include "cvtools.h"

NTUVideoWidget::NTUVideoWidget(QWidget* parent) : QLabel(parent) {}
/*
static QImage cvMat2qImage(cv::Mat mat)
{

  // 8-bits unsigned, raw bayer image
  if (mat.type() == CV_8UC1)
  {
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, CV_BayerBG2RGB);
    QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    img = img.copy();
    return img;
    // 8-bit unsigned rgb image
  }
  else if (mat.type() == CV_8UC3)
  {
    // Copy input Mat
    QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    return img;
    // 16-bit unsigned, raw bayer image
  }
  else if (mat.type() == CV_16UC1)
  {
    cv::Mat mat8bit = mat.clone();
    cvtools::rshift(mat8bit, 8);
    mat8bit.convertTo(mat8bit, CV_8UC1);
    cv::Mat rgb;
    cv::cvtColor(mat8bit, rgb, CV_BayerBG2RGB);
    QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    img = img.copy();
    return img;
    // 16-bit unsigned rgb image
  }
  else if (mat.type() == CV_16UC3)
  {
    mat.convertTo(mat, CV_8UC3, 1.0 / 256.0);
    return cvMat2qImage(mat);
    // 32bit floating point gray-scale image
  }
  else if (mat.type() == CV_32FC1)
  {
    cv::Mat rgb(mat.size(), CV_32FC3);
    cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB32);
    img = img.copy();
    return img;
  }
  else if (mat.type() == CV_32FC3)
  {
    QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
    img = img.copy();
    return img;
  }
  else
  {
    std::cerr << "SMVideoWidget: cv::Mat could not be converted to QImage!";
    return QImage();
  }

  // 8-bit unsigned gray-scale image
  //    } else if(mat.type()==CV_8UC1) {
  //        // Set the color table (used to tranMVate colour indexes to qRgb values)
  //        QVector<QRgb> colorTable;
  //        for (int i=0; i<256; i++)
  //            colorTable.push_back(qRgb(i,i,i));
  //        // Copy input Mat
  //        QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
  //        img.setColorTable(colorTable);
  //        return img;
}
*/
void NTUVideoWidget::showImageCV(cv::Mat image)
{
  if (image.rows)
  {
    QImage qimage = cvtools::cvMat2qImage(image);

    // correct size only if label has no borders/frame!
    int w = this->width();
    int h = this->height();

    pixmap = QPixmap::fromImage(qimage);
    this->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
  }
}

void NTUVideoWidget::resizeEvent(QResizeEvent* event)
{

  if (!pixmap.isNull())
  {
    // correct size only if label has no borders/frame!
    int w = event->size().width();
    int h = event->size().height();
    this->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
  }
}
