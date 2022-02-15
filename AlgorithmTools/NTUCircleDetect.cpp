#include "NTUCircleDetect.h"
#include "NTUCVTools.h"
#include "cvtools.h"

#define CVBIL_D 7
#define CVBIL_SIG_COLOR 50
#define CVBIL_SIG_SPACE 50
#define CVCANNY_KERNEL 3

NTUCircleDetect::NTUCircleDetect(QString name) : NTUTools(name)
{
  mEdgeWidth = 3;
  mEdgeContrast = 5;
  mEdgeTransition = 2;
  mRectRoi.x = 0;
  mRectRoi.y = 0;
  mRectRoi.width = 100 * 2;
  mRectRoi.width = 100 * 2;
  mRadiusOuter = 100;
  mRadiusIner = 50;
}

void NTUCircleDetect::computer()
{

  mIsBusy = true;
  auto start = high_resolution_clock::now();

  //! [1] check if rectangle of ROI or rectangle of Object is avaiable
  if (mImage.empty())
  {
    mIsBusy = false;
    mResultOK = false;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    return;
  }

  //! [3] check if tool fixture is busy
  if (mToolFixture != nullptr)
  {
    while (mToolFixture->mIsBusy)
    {
      QTest::qSleep(1);
    }
    if (!mToolFixture->mResultOK)
    {
      mIsBusy = false;
      mResultOK = false;
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      mExecutionTime = duration.count() / 1000.0;
      Q_EMIT newResult("");
      Q_EMIT newExecutionTime(QString::number(mExecutionTime));
      return;
    }
  }
  //! [3]

  //! [4] transform follow tool fixture
  Rect roiTransform;
  Mat mat_tran, matRoi;
  if (!tranformFollowToolFixture(roiTransform, mat_tran))
  {
    mIsBusy = false;
    mResultOK = false;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    return;
  }
  //! [4]
  //! [5] copy ROI of input image
  mat_tran(roiTransform).copyTo(matRoi);
  //! [5]

  cv::Mat mat_canny, filterRadius, mThres_Gray, mat_blur, mat_mask;
  // bilateralFilter(matRoi, mBilFilter, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);

  if (mEdgeWidth <= 0)
  {
    mIsBusy = false;
    mResultOK = false;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    return;
  }

  // blur(matRoi, mat_blur, Size(mEdgeWidth, mEdgeWidth));
  bilateralFilter(matRoi, mat_blur, mEdgeWidth, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);

  switch (mThresholdMethod)
  {
  case 0: // manual
    Canny(mat_blur, mat_canny, mEdgeContrast, 100, CVCANNY_KERNEL);

    break;
  case 1: // auto
  {
    double CannyAccThresh = threshold(mat_blur, mThres_Gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    double CannyThresh = 0.1 * CannyAccThresh;
    Canny(mat_blur, mat_canny, CannyThresh, CannyAccThresh, CVCANNY_KERNEL);
    break;
  }
  default:
    break;
  }

  NTUCVTools::fillterEdgeTrasition(mat_blur, mat_canny, mEdgeTransition, mat_mask);

  mat_canny = mat_mask;
  vector<Point2f> points;
  Point2f center_point = Point2f(mat_canny.size() / 2);

  if (!NTUCVTools::filterWithRadius(mat_canny, filterRadius, points, center_point, mRadiusIner, mRadiusOuter))

  {
    mIsBusy = false;
    mResultOK = false;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    return;
  }

  double initRadius = mRadiusOuter - mRadiusIner;
  NTUCVTools::Circle circle;
  NTUCVTools::Circle circleIni(center_point.x, center_point.y, initRadius);

  if (CircleFitByLevenbergMarquardtFull(points, circleIni, 0.001, circle) != 0)
  {
    mIsBusy = false;
    mResultOK = false;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    return;
  }
  mRadius = circle.r;

  vector<Point2f> regionPoints = NTUCVTools::getPointsFromRect(roiTransform);

  //! [12] coordinate top left of rectangle Search
  Point2f pointTransform = mRectRoi.tl();
  if (this->mToolFixture != nullptr)
  {
    // qDebug() << mToolFixture->mDeltaX << ", " << mToolFixture->mDeltaY << "\n";
    pointTransform += Point2f(mToolFixture->mDeltaX, mToolFixture->mDeltaY);
  }

  mCoordinate = Point2f(circle.a, circle.b) + pointTransform;

  mCrossPoints = {{mCoordinate.x - cross_length, mCoordinate.y},
                  {mCoordinate.x + cross_length, mCoordinate.y},
                  {mCoordinate.x, mCoordinate.y - cross_length},
                  {mCoordinate.x, mCoordinate.y + cross_length}};

  if (mToolFixture != nullptr)
  {
    mCrossPoints = NTUCVTools::rotatePoints(mCrossPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mCoordinate = NTUCVTools::rotatePoint(mCoordinate, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
  }

  if (mFirst)
  {
    mFirstmCoordinate = mCoordinate;
    mFirst = false;
    mDeltaX = 0;
    mDeltaY = 0;
  }
  else
  {
    mDeltaX = mCoordinate.x - mFirstmCoordinate.x;
    mDeltaY = mCoordinate.y - mFirstmCoordinate.y;
  }

  mIsBusy = false;
  mResultOK = true;
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  mExecutionTime = duration.count() / 1000.0;
  QString textResult = tr("(%1,%2) diameter = %3").arg(mCoordinate.x).arg(mCoordinate.y).arg(mRadius * 2.f);

  Q_EMIT newResult(textResult);
  Q_EMIT newExecutionTime(QString::number(mExecutionTime));
}

void NTUCircleDetect::getDrawResult(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
    angle = mToolFixture->mAngle;

  if (mResultOK)
  {

    if (!mRegionPoints.empty())
    {
      NTUCVTools::drawPolygon(image, mRegionPoints, color_OK, lineWidth);
      NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, color_OK, FONT_SIZE, Point2f(-10, -10));
    }
    if (!mCrossPoints.empty())
    {
      NTUCVTools::drawArrowLine(image, mCrossPoints[0], mCrossPoints[1], color_OK, lineWidth);
      NTUCVTools::drawArrowLine(image, mCrossPoints[2], mCrossPoints[3], color_OK, lineWidth);
    }
    NTUCVTools::drawPoint(image, mCoordinate, color_OK, lineWidth);
    NTUCVTools::drawCircle(image, mCoordinate, mRadius, color_OK, lineWidth);
  }
  else
  {
    if (mImageTemplate.empty())
      return;

    Rect roiTransform;
    Mat mat_tran;
    if (!tranformFollowToolFixture(roiTransform, mat_tran))
      return;

    //! [8]
    vector<Point2f> regionPoints = {roiTransform.tl(), Point2f(roiTransform.tl().x, roiTransform.br().y), roiTransform.br(),
                                    Point2f(roiTransform.br().x, roiTransform.tl().y)};

    if (mToolFixture != nullptr)
    {
      mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    }
    else
      mRegionPoints = regionPoints;

    NTUCVTools::drawPolygon(image, mRegionPoints, color_NG, lineWidth);
    NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, color_NG, FONT_SIZE, Point2f(-10, -10));
    NTUCVTools::drawPoint(image, mCoordinate, color_NG, lineWidth);
  }
}

void NTUCircleDetect::getDrawRegion(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
    angle = mToolFixture->mAngle;

  Rect roiTransform;
  Mat mat_tran;
  if (!tranformFollowToolFixture(roiTransform, mat_tran))
    return;

  //! [8]
  vector<Point2f> regionPoints = {roiTransform.tl(), Point2f(roiTransform.tl().x, roiTransform.br().y), roiTransform.br(),
                                  Point2f(roiTransform.br().x, roiTransform.tl().y)};

  if (mToolFixture != nullptr)
  {
    mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mCoordinate, -mToolFixture->mAngle);
  }
  else
    mRegionPoints = regionPoints;
  Scalar colorDisk;
  if (mResultOK)
    colorDisk = color_OK;
  else
    colorDisk = color_NG;

  NTUCVTools::drawPolygon(image, mRegionPoints, colorDisk, lineWidth);
  NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, colorDisk, FONT_SIZE, Point2f(-10, -10));
}

void NTUCircleDetect::onReadSettingFromJSon(QJsonObject* setting)
{
  mThresholdMethod = setting->value(CircleDetectSettings[CIRCLE_DETECT_THRESHOLD_MEDTHOD]).toInt();
  mEdgeContrast = setting->value(CircleDetectSettings[CIRCLE_DETECT_EDGE_CONTRAST]).toInt();
  mEdgeTransition = setting->value(CircleDetectSettings[CIRCLE_DETECT_EDGETRANSITION]).toInt();
  mEdgeWidth = setting->value(CircleDetectSettings[CIRCLE_DETECT_EDGE_WIDTH]).toInt();
  QString circleIn = setting->value(CircleDetectSettings[CIRCLE_DETECT_CERCLE_IN]).toString();
  QString circleOut = setting->value(CircleDetectSettings[CIRCLE_DETECT_CERCLE_OUT]).toString();

  if (!cvtools::getCircleFromQString(circleIn, mCoordinate, mRadiusIner))
    return;

  if (!cvtools::getCircleFromQString(circleOut, mCoordinate, mRadiusOuter))
    return;
  mRectRoi.x = mCoordinate.x - mRadiusOuter;
  mRectRoi.y = mCoordinate.y - mRadiusOuter;
  mRectRoi.width = mRadiusOuter * 2;
  mRectRoi.height = mRadiusOuter * 2;

  mFirstmCoordinate = mCoordinate;
  mCoordinate = mFirstmCoordinate;

  mFirst = true;
}
