#ifndef NTUTOOLS_H
#define NTUTOOLS_H

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QTest>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <chrono>

using namespace std::chrono;
using namespace cv;
using namespace std;

static cv::Scalar color_OK(0, 102, 0);
static cv::Scalar color_NG(0, 0, 255);

static int lineWidth = 2;
static int cross_length = 20;

class NTUTools : public QObject
{
  Q_OBJECT
public:
  explicit NTUTools(QString name);

  ///
  /// \brief getDrawResult
  /// \param src
  ///
  virtual void getDrawResult(QImage& image) = 0;

  ///
  /// \brief getDrawRegion
  /// \param image
  ///
  virtual void getDrawRegion(QImage& image) = 0;

  ///
  /// \brief setImageTemplate
  /// \param src
  ///
  virtual void setImageTemplate(const Mat src) = 0;

  ///
  /// \brief tranformFollowToolFixture
  /// \param roiTran
  /// \return
  ///
  bool tranformFollowToolFixture(Rect& roiTranform, Mat& matTranform);

  ///
  /// \brief setToolFixture
  /// \param tools
  ///
  void setToolFixture(NTUTools* tools);

  ///
  /// \brief getToolFixture
  /// \return
  ///
  NTUTools* getToolFixture();

  ///
  /// \brief getExecutionTime
  /// \return
  ///
  int getExecutionTime();

  ///
  /// \brief setToolName
  /// \param name
  ///
  void setToolName(QString name);

  QString mName;
  double mDeltaX;
  double mDeltaY;
  double mScale;
  double mAngle;
  bool mIsBusy;
  bool mResultOK;
  Point2f mCoordinate;
  Point2f mFirstmCoordinate;
  Mat mImage;
  Mat mImageTemplate;

Q_SIGNALS:
  void newResult(QString result);
  void newImageTemplate(cv::Mat image);
  void newExecutionTime(QString time);
  void newSendImageResult(cv::Mat img);

public Q_SLOTS:
  ///
  /// \brief onReadSettingFromJSon
  /// \param setting
  ///
  virtual void onReadSettingFromJSon(QJsonObject* setting) = 0;

  ///
  /// \brief computer
  /// \return
  ///
  virtual void computer() = 0;

  ///
  /// \brief setInputImage
  /// \param src
  ///
  virtual void onReceiverInputImage(cv::Mat src);

protected:
  Rect2f mRectRoi;
  Rect2f mRectObj;

  NTUTools* mToolFixture;

  int mExecutionTime;
  bool mFirst;

  double mOffsetX;
  double mOffsetY;
  double mScore;

  vector<Point2f> mPoints;
  vector<Point2f> mCrossPoints;
  vector<Point2f> mRegionPoints;
  QMutex mProcessMutex;
};

// Q_DECLARE_METATYPE(NTUTools);
#endif // NTUTOOLS_H
