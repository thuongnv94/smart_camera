#ifndef CVTOOLS_H
#define CVTOOLS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#if CV_VERSION_MAJOR < 4
#include <opencv/cv.h>
#include <opencv/highgui.h>
#else
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#endif

#include <string>
#include <sys/stat.h>

#include <QColor>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QSettings>
#include <QString>

namespace cvtools
{
struct Rgb
{
  uchar r;
  uchar g;
  uchar b;

  Rgb() {}
  Rgb(uchar r_, uchar g_, uchar b_)
  {
    r = r_;
    g = g_;
    b = b_;
  }
};

struct Xyz
{
  double x;
  double y;
  double z;
  Xyz() {}
  Xyz(double x_, double y_, double z_)
  {
    x = x_;
    y = y_;
    z = z_;
  }
};
struct Lab
{
  double l;
  double a;
  double b;
  Lab() {}
  Lab(double l_, double a_, double b_)
  {
    l = l_;
    a = a_;
    b = b_;
  }
};

struct ObjectFeature
{
  Lab lab;
  Rgb rgb;
  double deltaE;
  double OD;
  double Density;
  cv::Mat image;
  cv::Mat r_hist;
  cv::Mat g_hist;
  cv::Mat b_hist;
  QString getRGBText() { return QString("%1,%2,%3").arg(rgb.r).arg(rgb.g).arg(rgb.b); }
  QColor getQColor() { return QColor::fromRgb(rgb.r, rgb.g, rgb.b); }
};

struct ItemRectangleDraw
{
  ItemRectangleDraw() { found = false; }
  QRectF posData;
  QRectF posDetect;
  QString name;
  bool found;
};

void cvtColorBGRToBayerBG(const cv::Mat& imBGR, cv::Mat& imBayerBG);
void removeIncorrectMatches(const cv::Mat F, const std::vector<cv::Point2f>& q0, const std::vector<cv::Point2f>& q1, const float maxD,
                            std::vector<cv::Point2f> q0Correct, std::vector<cv::Point2f> q1Correct);
void rshift(cv::Mat& I, unsigned int shift);
void drawChessboardCorners(cv::InputOutputArray _image, cv::Size patternSize, cv::InputArray _corners, bool patternWasFound,
                           int line_width = 1);
void cvDrawChessboardCorners(CvArr* _image, CvSize pattern_size, CvPoint2D32f* corners, int count, int found, int line_width = 1);
cv::Mat modulo(const cv::Mat& mat, float n);
void matToPoints3f(const cv::Mat& mat, std::vector<cv::Point3f>& points);
void convertMatFromHomogeneous(cv::Mat& src, cv::Mat& dst);
void handEyeCalibrationTsai(const std::vector<cv::Matx33f> R, const std::vector<cv::Vec3f> t, const std::vector<cv::Matx33f> R_mark,
                            const std::vector<cv::Vec3f> t_mark, cv::Matx33f& Omega, cv::Vec3f& tau);
void fitSixDofData(const std::vector<cv::Matx33f> R, const std::vector<cv::Vec3f> t, const std::vector<cv::Matx33f> R_mark,
                   const std::vector<cv::Vec3f> t_mark, cv::Matx33f& Omega, cv::Vec3f& tau);
void initDistortMap(const cv::Matx33f cameraMatrix, const cv::Vec<float, 5> distCoeffs, const cv::Size size, cv::Mat& map1, cv::Mat& map2);
cv::Mat diamondDownsample(cv::Mat& pattern);
void imshow(const char* windowName, cv::Mat im, unsigned int x, unsigned int y);
void imagesc(const char* windowName, cv::Mat im);
cv::Mat histimage(cv::Mat histogram);
void hist(const char* windowName, cv::Mat im, unsigned int x, unsigned int y);
void writeMat(cv::Mat const& mat, const char* filename, const char* varName = "A", bool bgr2rgb = true);
void convertRGBtoLab(Rgb* color, Lab* item);
void convertRGBtoXYZColor(Rgb* color, Xyz* item);

double median(cv::Mat channel);

bool makeDirectory(const char* directory);
bool removeFile(const char* file);
QString getStringFromDateTime(QDateTimeEdit* datetime);
double getDeltaE(Lab a, Lab b);
cv::Mat accumImage(std::vector<cv::Mat> input);

void analyticsImage(ObjectFeature& obj);

double getSimilarity(const cv::Mat A, const cv::Mat B);

bool getRectFromQString(QString input, cv::Rect2f& rect);

bool getCircleFromQString(QString input, cv::Point2f& center, float& radius);

bool getBGRFromQString(const QString input, int& b, int& g, int& r);
QImage cvMat2qImage(cv::Mat mat);
cv::Scalar ScalarHSV2BGR(uchar H, uchar S, uchar V);
cv::Scalar ScalarBGR2HSV(uchar B, uchar G, uchar R);
} // namespace cvtools

#endif // CVTOOLS_H
