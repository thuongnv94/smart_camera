#include "NTUPatMax.h"
#include "GeoMatch.h"
#include "cvtools.h"

#define CVBIL_D 7
#define CVBIL_SIG_COLOR 50
#define CVBIL_SIG_SPACE 50
NTUPatMax::NTUPatMax(QString name) : NTUTools(name)
{
  mGM = new GeoMatch(); // object to implent geometric matching
}

NTUPatMax::~NTUPatMax() { delete mGM; }

bool NTUPatMax::templateMachingMultilevel(const NTUDataPryDownImage scene, const NTUDataPryDownImage obj, NTUDataPryDownImage& out)
{
  // cv::Point2f centerRoi;
  // int level = 0;
  double score = 0.0;
  // int lowThreshold = 10;    // deafult value
  int highThreashold = 100; // deafult value

  // double minScore = 0.7;   // deafult value
  double greediness = 0.9; // deafult value

  // double total_time = 0;
  Point result;
  if (mFirst)
  {
    if (!mGM->CreateGeoMatchModel(obj.image[1], mContrastThreshold, HIGH_THRESHOLD))
    {
      cout << "ERROR: could not create model...";
      return false;
    }
  }

  score = mGM->FindGeoMatchModel(scene.image[1], mAceptThreshold, greediness, &result);

  // check level 1

  //    Mat draw, draw2;

  //    cvtColor(obj.image[1], draw, COLOR_GRAY2BGR);
  //    GM->DrawContours(draw, CV_RGB(255, 0, 0), 1);

  //    imshow("draw", draw);

  //    cvtColor(scene.image[1], draw2, cv::COLOR_GRAY2BGR);
  //    GM->DrawContours(draw2, result, Scalar(0, 255, 0), 1);

  //    imshow("Contour", draw2);

  //    waitKey(100);

  if (score < mAceptThreshold)
    return false;

  out.image = scene.image;
  out.centerROI = result;
  out.score = score;
  out.angle = scene.angle;
  return true;
}

bool sortbyLargesScore(const NTUDataPryDownImage& a, const NTUDataPryDownImage& b) { return (a.score > b.score); }

void NTUPatMax::computer()
{

  mIsBusy = true;
  auto start = high_resolution_clock::now();

  //! [1] check if rectangle of ROI or rectangle of Object is avaiable
  if (mImage.empty() || mImageTemplate.empty() || mRectObj.width >= mRectRoi.width || mRectObj.height >= mRectRoi.height)
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
  //! [1]

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

  //! [6] fillter noise of image
  cv::blur(matRoi, matRoi, cv::Size(BLUR_SIZE, BLUR_SIZE));
  // bilateralFilter(matRoi, mat_tmp, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);
  //! [6]

  //! [7] initializer scene image matching
  vector<NTUDataPryDownImage> sceneData;
  for (int i = 0; i < mAngleTolerance; ++i)
  {
    sceneData.push_back(NTUDataPryDownImage(matRoi, i));
    sceneData.push_back(NTUDataPryDownImage(matRoi, -1 * i));
  }
  //! [7]
  //! [8]
  std::vector<NTUDataPryDownImage> dataMatching;

  for (int i = 0; i < sceneData.size(); i++)
  {
    NTUDataPryDownImage result;

    if (templateMachingMultilevel(sceneData[i], mDataTemplateImage, result))
    {
      dataMatching.push_back(result);
    }
  }

  //! [8]

  //! [9] data matching if empty
  if (dataMatching.empty())
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
  //! [9]
  //! [10]

  std::sort(dataMatching.begin(), dataMatching.end(), sortbyLargesScore);
  NTUDataPryDownImage dataresult = dataMatching.front();

  //! [11] get result
  mScore = dataresult.score;
  mAngle = dataresult.angle;
  Point2f pt_center = dataresult.centerROI;
  Point2f pt_center_offset = pt_center + Point2f(mOffsetX, mOffsetY);
  //! [11]

  //! [12] coordinate top left of rectangle Search
  Point2f pointTransform = mRectRoi.tl();
  if (this->mToolFixture != nullptr)
    pointTransform += Point2f(mToolFixture->mDeltaX, mToolFixture->mDeltaY);

  //! [12]

  //! [13] points of rectangle object
  vector<Point2f> pointsOfRectangleObject = NTUCVTools::getPointsFromCenter(pt_center, mRectObj.size());
  //! [13]

  //    Rect rec1(points[0].x, points[0].y, mRectObj.width, mRectObj.height);
  //    Mat searchMat;
  //    dataresult.image[1](rec1).copyTo(searchMat);
  //    subFindRotate(mImageTemplate, searchMat);

  Point2f centerRotate = Point2f((dataresult.image[1].cols) / 2.f, (dataresult.image[1].rows) / 2.f);

  /// diffirence size after rotation
  float deltaWidth = (matRoi.cols - dataresult.image[1].cols) / 2.f;
  float deltaHeight = (matRoi.rows - dataresult.image[1].rows) / 2.f;

  Point2f translatePoint = Point2f(deltaWidth, deltaHeight) + pointTransform;

  //! [14] cross points
  vector<Point2f> crossPoinst = {{pt_center_offset.x - cross_length, pt_center_offset.y},
                                 {pt_center_offset.x + cross_length, pt_center_offset.y},
                                 {pt_center_offset.x, pt_center_offset.y - cross_length},
                                 {pt_center_offset.x, pt_center_offset.y + cross_length}};

  mCrossPoints = NTUCVTools::rotatePoints(crossPoinst, centerRotate, -mAngle);
  // transform to original coordinate image
  for (size_t i = 0; i < mCrossPoints.size(); ++i)
  {
    mCrossPoints[i] += translatePoint;
  }
  //! [14]

  //! [6] coordinate
  mCoordinate = NTUCVTools::rotatePoint(pt_center_offset, centerRotate, -mAngle) + translatePoint;
  //! [6]

  //! [7]
  //  mPoints = NTUCVTools::rotatePoints(pointsOfRectangleObject, centerRotate, -mAngle);

  //  for (size_t i = 0; i < mPoints.size(); ++i)
  //  {
  //    mPoints[i] += translatePoint;
  //  }
  //! [7]

  //! [8]
  vector<Point2f> regionPoints = NTUCVTools::getPointsFromRect(roiTransform);

  if (mToolFixture != nullptr)
  {
    mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    // mPoints = NTUCVTools::rotatePoints(mPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mCoordinate = NTUCVTools::rotatePoint(mCoordinate, mToolFixture->mCoordinate, -mToolFixture->mAngle);
  }
  else
    mRegionPoints = regionPoints;

  //! [8]

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
  QString textResult = tr("(%1 %2) %3° score=%4").arg(mCoordinate.x).arg(mCoordinate.y).arg(mAngle).arg(mScore);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  mExecutionTime = duration.count() / 1000.0;
  Q_EMIT newResult(textResult);
  Q_EMIT newExecutionTime(QString::number(mExecutionTime));
}

void NTUPatMax::getDrawResult(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
    angle = mToolFixture->mAngle;
  if (mResultOK)
  {

    // NTUCVTools::drawPolygon(image, mPoints, color_OK, lineWidth);
    NTUCVTools::drawPolygon(image, mRegionPoints, color_OK, lineWidth);
    NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, color_OK, FONT_SIZE, Point2f(-10, -10));
    NTUCVTools::drawArrowLine(image, mCrossPoints[0], mCrossPoints[1], color_OK, lineWidth);
    NTUCVTools::drawArrowLine(image, mCrossPoints[2], mCrossPoints[3], color_OK, lineWidth);
    // NTUCVTools::drawPoint(image, mCoordinate, color_OK, lineWidth);
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
    // NTUCVTools::drawPoint(image, mCoordinate, color_NG, lineWidth);
  }
}

void NTUPatMax::getDrawRegion(QImage& image)
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

  Scalar color;
  if (mResultOK)
    color = color_OK;
  else
    color = color_NG;
  NTUCVTools::drawPolygon(image, mRegionPoints, color, lineWidth);
  NTUCVTools::drawText(image, mName, mRegionPoints[0], angle, color, FONT_SIZE, Point2f(-10, -10));
}

void NTUPatMax::setImageTemplate(const Mat src)
{
  Mat mat_gray, mat_tmp;

  // 8-bits unsigned, raw bayer image
  if (src.type() == CV_8UC1)
  {
    src.copyTo(mat_gray);
    // 8-bit unsigned rgb image
  }
  else if (src.type() == CV_8UC3)
  {
    cvtColor(src, mat_gray, COLOR_BGR2GRAY);
  }
  if (mRectObj.x <= 0 || mRectObj.y <= 0 || mRectObj.width <= 0 || mRectObj.height <= 0)
    return;
  if (mat_gray.cols == mRectObj.width && mat_gray.rows == mRectObj.height)
    mat_gray.copyTo(mat_tmp);
  else
    mat_gray(mRectObj).copyTo(mat_tmp);

  blur(mat_tmp, mImageTemplate, cv::Size(BLUR_SIZE, BLUR_SIZE));
  // bilateralFilter(mat_tmp, mImageTemplate, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);
  mDataTemplateImage = (NTUDataPryDownImage(mImageTemplate, 0));

  mFirst = true;

  Q_EMIT newImageTemplate(mat_tmp);
}

void NTUPatMax::onReadSettingFromJSon(QJsonObject* setting)
{
  mContrastThreshold = setting->value(PatMaxSettings[PATMAX_CONTRAST_THRESHOLD]).toInt();
  mAngleTolerance = setting->value(PatMaxSettings[PATMAX_ANGLE_TOLERANCE]).toInt();
  mAceptThreshold = setting->value(PatMaxSettings[PATMAX_ACCEPT_THRESHOLD]).toDouble();
  mOffsetX = setting->value(PatMaxSettings[PATMAX_HORIZONTAL_OFFSET]).toDouble();
  mOffsetY = setting->value(PatMaxSettings[PATMAX_VERTICAL_OFFSET]).toDouble();
  QString rectSearch = setting->value(PatMaxSettings[PATMAX_SEARCH]).toString();
  QString rectcObj = setting->value(PatMaxSettings[PATMAX_OBJECT]).toString();
  // setting->value(PatternSettings[TIME_OUT], QJsonValue::fromVariant(timeOut));

  if (!cvtools::getRectFromQString(rectSearch, mRectRoi)) // not corect
    return;

  mFirstmCoordinate = Point2f(mRectRoi.x + mRectRoi.width / 2.0, mRectRoi.y + mRectRoi.height / 2.0);
  mCoordinate = mFirstmCoordinate;

  if (!cvtools::getRectFromQString(rectcObj, mRectObj)) // not corect
    return;
  mFirst = true;
}
