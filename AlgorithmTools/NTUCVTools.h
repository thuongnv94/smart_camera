#ifndef NTUCVTOOLS_H
#define NTUCVTOOLS_H

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

#include "shapeTools/NTUShape.h"
#include <QApplication>
#include <QColor>
#include <QImage>
#include <QLine>
#include <QPainter>
#include <QPointF>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

#define LEVELDOWNSAMPLE 2
#define MIN_SIZE_DOWNSAMPLE 32

enum class BhThresholdMethod
{
  OTSU,
  NIBLACK,
  SAUVOLA,
  WOLFJOLION
};

#define uget(x, y) at<unsigned char>(y, x)
#define uset(x, y, v) at<unsigned char>(y, x) = v;
#define fget(x, y) at<float>(y, x)
#define fset(x, y, v) at<float>(y, x) = v;

using namespace cv;
using namespace std;

namespace NTUCVTools
{

//   next define some frequently used functions:

template <typename T> inline T SQR(T t) { return t * t; };

struct Circle
{
  Circle()
  {
    a = 0.;
    b = 0.;
    r = 1.;
    s = 0.;
    i = 0;
    j = 0;
  }

  Circle(double aa, double bb, double rr)
  {
    a = aa;
    b = bb;
    r = rr;
  }
  // The fields of a Circle
  double a, b, r, s, g, Gx, Gy;
  int i, j;
};

///
/// \brief getSimilarity
/// \param A
/// \param B
/// \param mask
/// \return
///
double getSimilarity(const Mat A, const Mat B);

///
/// \brief templateMachingSimple
/// \param img_src
/// \param tmp
/// \param points
/// \param threshhold
/// \return
///
double templateMachingSimple(Mat img_src, Mat tmp, Point2f& pointCenter, double threshhold);

///
/// \brief rotateImage
/// \param src
/// \param angle
/// \param flag
/// \return
///
cv::Mat rotateImage(const cv::Mat src, double angle, bool flag = false, Point2f pc = Point2f(-1, -1));

///
/// \brief rotatePoints
/// \param src
/// \param roi_points
/// \param angle
/// \return
///
std::vector<cv::Point2f> rotatePoints(Size size_src, const std::vector<cv::Point2f> roi_points, float angle);

///
/// \brief rotatePoints
/// \param roi_points
/// \param center
/// \param angle
/// \return
///
std::vector<cv::Point2f> rotatePoints(const std::vector<cv::Point2f> roi_points, Point2f center, double angle);

///
/// \brief rotatePoint
/// \param inPoint
/// \param center
/// \param angRad
/// \return
///
cv::Point2f rotatePoint(const cv::Point2f& inPoint, const cv::Point2f& center, const double& angRad);

///
/// \brief rotate2d
/// \param inPoint
/// \param angRad
/// \return
///
cv::Point2f rotate2d(const cv::Point2f& inPoint, const double& angRad);

///
/// \brief CircleFitByLevenbergMarquardtFull
/// \param points
/// \param circleIni
/// \param LambdaIni
/// \param circle
/// \return
///
int CircleFitByLevenbergMarquardtFull(vector<Point2f>& points, Circle& circleIni, double LambdaIni, Circle& circle);

///
/// \brief means
/// \param points
/// \param meanX
/// \param meanY
///
void meansOfPoints(vector<Point2f> points, double& meanX, double& meanY);

///
/// \brief Sigma
/// \param points
/// \param circle
/// \return
///
double Sigma(vector<Point2f>& points, Circle& circle);

///
/// \brief isInside
/// \param centerbool filterWithRadius(cv::Mat src, cv::Mat& dst, Point2f center, float r_iner, float r_outer)
/// \param point
/// \param rad
/// \return
///
bool isInside(Point2f center, Point2f point, float rad);

///
/// \brief isOutside
/// \param center
/// \param point
/// \param rad
/// \return
///
bool isOutside(Point2f center, Point2f point, float rad);

///
/// \brief filterWithRadius
/// \param src
/// \param dst
/// \param center
/// \param r_iner
/// \param r_outer
/// \return
///
bool filterWithRadius(cv::Mat src, cv::Mat& dst, vector<Point2f>& points, Point2f center, float r_iner, float r_outer);

///
/// \brief NiblackSauvolaWolfJolion
/// \param _src
/// \param _dst
/// \param version
/// \param winx
/// \param winy
/// \param k
/// \param dR
///
void NiblackSauvolaWolfJolion(Mat im, Mat& output, BhThresholdMethod version, int winx, int winy, double k, double dR);

///
/// \brief calcLocalStats
/// \param im
/// \param map_m
/// \param map_s
/// \param win_x
/// \param win_y
/// \return
///
double calcLocalStats(Mat& im, Mat& map_m, Mat& map_s, int win_x, int win_y);

///
/// \brief createPoinsFromCennterAndSize
/// \param center
/// \param width
/// \param height
/// \return
///
vector<Point2f> createPoinsFromCennterAndSize(Point2f center, float width, float height);

///
/// \brief autoThreshold
/// \param src
/// \param dst
///
void autoThreshold(Mat src, Mat& dst, BhThresholdMethod method);

///
/// \brief drawArrowLine
/// \param image
/// \param point1
/// \param point2
/// \param color
/// \param thickness
///
void drawArrowLine(QImage& image, cv::Point2f pt1, Point2f pt2, Scalar color, int thickness);

///
/// \brief drawPolygon
/// \param image
/// \param pts
/// \param color
/// \param thickness
///
void drawPolygon(QImage& image, vector<Point2f> pts, Scalar color, int thickness);

///
/// \brief drawPolyLine
/// \param image
/// \param pts
/// \param color
/// \param thickness
///
void drawPolyLine(QImage& image, vector<Point> pts, Scalar color, int thickness);

void drawPolyLine(QImage& image, vector<Point2f> pts, Scalar color, int thickness);


///
/// \brief drawText
/// \param image
/// \param pt
/// \param color
/// \param thickness
/// \param shift
///
void drawText(QImage& image, QString text, Point2f pt, double angle, Scalar color, int fontsize, Point2f shift = Point2f(0, 0),
              int thickness = 3);

///
/// \brief drawPoint
/// \param image
/// \param pt
/// \param color
///
void drawPoint(QImage& image, Point2f pt, Scalar color, int thickness);

///
/// \brief drawCircle
/// \param image
/// \param pt
/// \param radius
/// \param color
/// \param thickness
///
void drawCircle(QImage &image, Point2f pt, float radius, Scalar color, int thickness);

///
/// \brief scalar2qcolor
/// \param color
/// \return
///
QColor scalar2qcolor(cv::Scalar color);

///
/// \brief qcolor2scalar
/// \param color
/// \return
///
cv::Scalar qcolor2scalar(QColor color);

///
/// \brief point2f2qpoint2f
/// \param point
/// \return
///
QPointF point2f2qpoint2f(Point2f point);

///
/// \brief transformRect
/// \param rect
/// \param delta_x
/// \param delta_y
/// \return
///
Rect transformRect(Rect rect, float delta_x, float delta_y);

///
/// \brief getPointsFromCenter
/// \param center
/// \param size
/// \return
///
vector<Point2f> getPointsFromCenter(Point2f center, Size size);

///
/// \brief getPointsFromRect
/// \param rect
/// \return
///
vector<Point2f> getPointsFromRect(Rect rect);

///
/// \brief calculatorHistogram
/// \param src
/// \return
///
vector<float> calculatorHistogram(cv::Mat src);

///
/// \brief getLinePointinImageBorder
/// \param p1_in
/// \param p2_in
/// \param p1_out
/// \param p2_out
/// \param rows
/// \param cols
/// \return
///
bool getLinePointinImageBorder(const cv::Point& p1_in, const cv::Point& p2_in, cv::Point& p1_out, cv::Point& p2_out, int rows, int cols);

///
/// \brief pointBetwenPoints
/// \param center
/// \param pt0
/// \param pt1
/// \return
///
bool pointBetwenPoints(Point2f center, Point2f pt0, Point2f pt1);

///
/// \brief fillterEdgeTrasition
/// \param src
/// \param edge
/// \param select
/// \param mask
///
void fillterEdgeTrasition(const Mat src, const Mat edge, int select, Mat& mask);

///
/// \brief getLine
/// \param pt1
/// \param pt2
/// \param a
/// \param b
/// \param c
///
void getLine(Point pt1, Point pt2, double& a, double& b, double& c);

///
/// \brief getDist
/// \param a
/// \param b
/// \param c
/// \param pt
/// \return
///
double getDist(double a, double b, double c, Point pt);

///
/// \brief matToJson
/// \param mat
/// \return
///
QJsonObject matToJson(const cv::Mat mat);

///
/// \brief jsonToMat
/// \param object
/// \return
///
Mat jsonToMat(const QJsonObject object);

///
/// \brief toJson
/// \param myVec
/// \return
///
QJsonArray toJson(const std::vector<uchar>& myVec);


///
/// \brief templateMachingFast
/// \param img_src
/// \param tmp
/// \param pointCenter
/// \param threshhold
/// \return
///
double templateMachingFast(vector<Mat> img_src, vector<Mat> tpls, Point2f& pointCenter, int maxlevel );


} // namespace NTUCVTools

struct NTUDataPryDownImage
{
  NTUDataPryDownImage() : scale(1), angle(0), score(0.0) {}

  NTUDataPryDownImage(cv::Mat src, int angleSet, int level=1)
  {
    angle = angleSet;

    cv::Mat rotMat = NTUCVTools::rotateImage(src, angle, false);
    scale = 1;
    buildPyramid(rotMat, image, level);

/*
    cv::Mat tmp;

    for (int i = 0; i < LEVELDOWNSAMPLE; i++)
    {
      if (tmp.empty())
      {
        rotMat.copyTo(tmp);
      }
      else
      {
        // down sample image
        cv::pyrDown(tmp, tmp, tmp.size() / 2);
      }

      image.insert(image.begin(), tmp);
    }*/
  }

  int scale;
  int angle;
  double score;
  std::vector<cv::Mat> image;
  cv::Point2f centerROI;
  vector<Point2f> points;
};
#endif // NTUCVTOOLS_H
