#include "NTUBlob.h"
#include "NTUCVTools.h"
#include "cvtools.h"

using namespace std::chrono;

NTUBlob::NTUBlob(QString name) : NTUTools(name) {}

void NTUBlob::setImageTemplate(const Mat src) {}

void NTUBlob::onReadSettingFromJSon(QJsonObject* setting)
{
  mThresholdMode = setting->value(BlobSettings[BLOB_THRESHOLD_MODE]).toInt();
  mThreshold = setting->value(BlobSettings[BLOB_THRESHOLD]).toInt();
  mBlobColor = setting->value(BlobSettings[BLOB_COLOR]).toInt();
  mMinimumArea = setting->value(BlobSettings[BLOB_MINIMUM_AREA]).toInt();
  mMaximumArea = setting->value(BlobSettings[BLOB_MAXIMUM_AREA]).toInt();
  QString rectSearch = setting->value(BlobSettings[BLOB_SEARCH]).toString();

  if (!cvtools::getRectFromQString(rectSearch, mRectRoi)) // not corect
    return;

  mFirstmCoordinate = Point2f(mRectRoi.x + mRectRoi.width / 2.0, mRectRoi.y + mRectRoi.height / 2.0);
  mCoordinate = mFirstmCoordinate;

  mFirst = true;
}

void NTUBlob::computer()
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
  cv::imshow("RoiTranSform", matRoi);
  //! [5]

  cv::Mat imageProcess, mat_bur;
  vector<vector<cv::Point>> contours;
  vector<Vec4i> hierarchy;

  // blur image
  blur(matRoi, mat_bur, Size(3, 3));

  /// Detect edges using Threshold
  switch (mThresholdMode)
  {
  case MANUAL:
    threshold(mat_bur, imageProcess, mThreshold, 255, THRESH_BINARY);
    break;
  case AUTO:
    NTUCVTools::autoThreshold(mat_bur, imageProcess, BhThresholdMethod::WOLFJOLION);
    break;
  }

  /// Find contours
  findContours(imageProcess, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));

  vector<vector<Point>> contours_filter;
  for (size_t i = 0; i < contours.size(); i++)
  {
    vector<Point> points;
    for (size_t j = 0; j < contours[i].size(); ++j)
    {
      if (contours[i][j].x <= 1 || contours[i][j].y <= 1 || contours[i][j].x >= imageProcess.cols - 2 ||
          contours[i][j].y >= imageProcess.rows - 2)
        continue;
      points.push_back(contours[i][j]);
    }
    if (points.size() > 0)
      contours_filter.push_back(points);
  }

  vector<vector<Point>> contours_poly(contours_filter.size());
  vector<Point2f> centers(contours_filter.size());
  vector<float> radius(contours_filter.size());
  for (size_t i = 0; i < contours_filter.size(); i++)
  {
    approxPolyDP(contours_filter[i], contours_poly[i], 1, false);
    minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
  }

  for (int i = 0; i < contours_poly.size(); i++)
  {
    Moments moms = moments(contours_poly[i]);
    double area = moms.m00;

    // filter area
    if (area < mMinimumArea || area >= mMaximumArea)
      continue;

    // filter nertia
    double denominator = std::sqrt(std::pow(2 * moms.mu11, 2) + std::pow(moms.mu20 - moms.mu02, 2));
    const double eps = 1e-2;
    double ratio;
    if (denominator > eps)
    {
      double cosmin = (moms.mu20 - moms.mu02) / denominator;
      double sinmin = 2 * moms.mu11 / denominator;
      double cosmax = -cosmin;
      double sinmax = -sinmin;

      double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
      double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
      ratio = imin / imax;
    }
    else
    {
      ratio = 1;
    }

    if (ratio < 0.2 || ratio >= std::numeric_limits<float>::max())
      continue;

    //! [1]
    mContours.clear();
    //! [1]

    //! [4] white or black
    switch (mBlobColor)
    {
    case BLACK:
    {
      if (contourArea(contours_poly[i], true) >= 0)
      {
        vector<Point2f> tmp(contours_poly[i].begin(), contours_poly[i].end());
        mContours.push_back(tmp);
      }
      break;
    }
    case WHITE:
    {
      if (contourArea(contours_poly[i], true) < 0)
      {
        vector<Point2f> tmp(contours_poly[i].begin(), contours_poly[i].end());
        mContours.push_back(tmp);
      }
      break;
    }
    default:
    {
      vector<Point2f> tmp(contours_poly[i].begin(), contours_poly[i].end());
      mContours.push_back(tmp);
    }
    break;
    }
  }

  if (mContours.empty())
  {
    mIsBusy = false;
    mResultOK = false;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    mExecutionTime = duration.count() / 1000.0;
    Q_EMIT newResult("");
    Q_EMIT newExecutionTime(QString::number(mExecutionTime));
    // qDebug() << "Empty!!!\n";
    return;
  }

  //! [12] coordinate top left of rectangle Search
  Point2f pointTransform = roiTransform.tl();

  for (int i = 0; i < mContours.size(); ++i)
  {
    for (int j = 0; j < mContours[i].size(); ++j)
    {
      mContours[i][j] += pointTransform;
    }
  }

  cv::Point2f centerX(0.f, 0.f);

  for (int i = 0; i < mContours.size(); i++)
  {
    Moments moms = moments(mContours[i]);

    cv::Point2f center = Point2f(moms.m10 / moms.m00, moms.m01 / moms.m00);
    centerX.x += center.x;
    centerX.y += center.y;
  }

  //! [12]
  mCoordinate = centerX / static_cast<float>(mContours.size());
  vector<Point2f> regionPoints = NTUCVTools::getPointsFromRect(roiTransform);

  mCrossPoints = {{mCoordinate.x - cross_length, mCoordinate.y},
                  {mCoordinate.x + cross_length, mCoordinate.y},
                  {mCoordinate.x, mCoordinate.y - cross_length},
                  {mCoordinate.x, mCoordinate.y + cross_length}};
  if (mToolFixture != nullptr)
  {
    mCoordinate = NTUCVTools::rotatePoint(mCoordinate, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mCrossPoints = NTUCVTools::rotatePoints(mCrossPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);

    for (int i = 0; i < mContours.size(); i++)
    {
      mContours[i] = NTUCVTools::rotatePoints(mContours[i], mToolFixture->mCoordinate, -mToolFixture->mAngle);
    }
  }
  else {
      mRegionPoints = regionPoints;
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
  QString textResult = tr("(%1 %2)").arg(mCoordinate.x).arg(mCoordinate.y);

  Q_EMIT newResult(textResult);
  Q_EMIT newExecutionTime(QString::number(mExecutionTime));
}

void NTUBlob::getDrawResult(QImage& image)
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

void NTUBlob::getDrawRegion(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
  {
    angle = mToolFixture->mAngle;
  }
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

  for (size_t i = 0; i < mContours.size(); i++)
  {
    NTUCVTools::drawPolyLine(image, mContours[i], color_OK, lineWidth);
  }

  NTUCVTools::drawPolygon(image, mRegionPoints, color_OK, lineWidth);
   NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, color_OK, FONT_SIZE, Point2f(-10, -10));
}
