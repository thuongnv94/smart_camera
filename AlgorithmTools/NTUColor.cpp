#include "NTUColor.h"
#include "cvtools.h"

NTUColor::NTUColor(QString name) : NTUTools(name)
{

  mHMin = 0;
  mHMax = 176;
  mSMin = 0;
  mSMax = 255;
  mVMax = 255;
  mVMin = 0;
  mCount = 100;
  mRectRoi.x = 0;
  mRectRoi.y = 0;
  mRectRoi.width = 20 * 2;
  mRectRoi.width = 20 * 2;
}

void NTUColor::computer()
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

  cv::Mat filterRange, mThres_Gray, mat_blur, mat_mask, mat_HSV;

  blur(matRoi, mat_blur, Size(3, 3));

  cvtColor(mat_blur, mat_HSV, COLOR_BGR2HSV);

  cv::inRange(mat_HSV, cv::Scalar(mHMin, mSMin, mVMin), cv::Scalar(mHMax, mSMax, mVMax), filterRange);

  cv::Mat img_result = cv::Mat::zeros(mat_blur.size(), CV_8UC3);
  mat_blur.copyTo(img_result, filterRange);

  Q_EMIT newSendImageResult(img_result);

  int nonzero = cv::countNonZero(filterRange);

  if (nonzero < mCount)
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

  vector<Point2f> regionPoints = NTUCVTools::getPointsFromRect(roiTransform);

  //! [12] coordinate top left of rectangle Search
  Point2f pointTransform = mRectRoi.tl();
  if (this->mToolFixture != nullptr)
  {
    // qDebug() << mToolFixture->mDeltaX << ", " << mToolFixture->mDeltaY << "\n";
    pointTransform += Point2f(mToolFixture->mDeltaX, mToolFixture->mDeltaY);
  }

  mCoordinate = Point2f(roiTransform.width / 2.0, roiTransform.height / 2.0) + pointTransform;

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
  QString textResult = tr("counter = %3").arg(nonzero);

  Q_EMIT newResult(textResult);
  Q_EMIT newExecutionTime(QString::number(mExecutionTime));
}

void NTUColor::onReceiverInputImage(Mat src)
{
  mProcessMutex.lock();
  // 8-bits unsigned, raw bayer image
  if (src.type() == CV_8UC3)
  {
    src.copyTo(mImage);
    // 8-bit unsigned rgb image
  }
  else
  {
    std::cerr << "Unsupport type!\n";
  }
  mProcessMutex.unlock();
  // this->computer();
}

void NTUColor::getDrawResult(QImage& image)
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

void NTUColor::getDrawRegion(QImage& image)
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

void NTUColor::onReadSettingFromJSon(QJsonObject* setting)
{
  QString hsvMin = setting->value(ColorSettings[NTU_COLOR_HSV_MIN]).toString();
  QString hsvMax = setting->value(ColorSettings[NTU_COLOR_HSV_MAX]).toString();

  if (!cvtools::getBGRFromQString(hsvMin, mHMin, mSMin, mVMin))
    return;
  if (!cvtools::getBGRFromQString(hsvMax, mHMax, mSMax, mVMax))
    return;

  mCount = setting->value(ColorSettings[NTU_COLOR_COUNT]).toInt();

  QString rectSearch = setting->value(ColorSettings[NTU_COLOR_SEARCH]).toString();

  if (!cvtools::getRectFromQString(rectSearch, mRectRoi)) // not corect
    return;

  mFirstmCoordinate = Point2f(mRectRoi.x + mRectRoi.width / 2.0, mRectRoi.y + mRectRoi.height / 2.0);
  mCoordinate = mFirstmCoordinate;

  mFirst = true;
}
