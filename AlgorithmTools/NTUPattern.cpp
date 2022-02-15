#include "NTUPattern.h"
#include "MatchDescriptor.h"
#include "cvtools.h"

NTUPattern::NTUPattern(QString name) : NTUTools(name) {}

bool NTUPattern::templateMachingMultilevel(const NTUDataPryDownImage scene, const NTUDataPryDownImage obj, NTUDataPryDownImage& out)
{
  cv::Point2f centerRoi;
  double score = NTUCVTools::templateMachingFast(scene.image, obj.image, centerRoi, LEVELDOWNSAMPLE);
  //std::cerr << "Score: " << score << "\n";
  if (score < TEMPLATEMATCHING_SCORE)
    return false;

  /*
  int level = 0;
  double score = 0.0;
  bool found = false;
  while (level < LEVELDOWNSAMPLE)
  {
    score = NTUCVTools::templateMachingSimple(scene.image[level], obj.image[level], centerRoi, TEMPLATEMATCHING_SCORE);

    // check level 1
    if (score > 0)
    {
      found = true;
    }
    else
    {
      found = false;
      break;
    }

    level++;
  }

  if (!found || score > mAceptThreshold)
    return false;
*/


  out.image = scene.image;
  out.centerROI = centerRoi;
  out.score = score;
  out.angle = scene.angle;

  return true;
}
/*
double NTUPattern::subFindRotate(Mat sr1, Mat sr2)
{
  vector<Point2f> contours1, contours2, Zdown1, Zdown2;

  getContourFromMat(sr1, contours1);
  getContourFromMat(sr2, contours2);

  dft(contours1, Zdown1, DFT_SCALE | DFT_REAL_OUTPUT);
  dft(contours2, Zdown2, DFT_SCALE | DFT_REAL_OUTPUT);

  MatchDescriptor md;
  md.sContour = Zdown1;
  md.nbDesFit = 20;
  vector<Point> ctrRotated;
  float alpha, phi, s;

  md.AjustementRtSafe(Zdown2, alpha, phi, s);
  complex<float> expitheta = s * complex<float>(cos(phi), sin(phi));
  cout << "Origin " << alpha << " and rotated of " << phi * 180 / md.pi << " and scale " << s << " Distance between contour is "
       << md.Distance(expitheta, alpha) << " " << endl;

  return phi * 180 / md.pi;
}

void NTUPattern::getContourFromMat(Mat src, vector<Point2f>& contour_out)
{
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  Mat mTest, mThresh1, mConnected;

  Mat m1, m2, m3, m4, m5;

  // bitwise_not(, m1);
  threshold(src, m2, 85, 255, THRESH_BINARY);

  findContours(m2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

  vector<int> ctrSelec1;
  for (unsigned long i = 0; i < contours.size(); i++)
  {
    if (contours[i].size() >= 10)
    {
      for (int j = 0; j < contours[i].size(); ++j)
      {
        contour_out.push_back(contours[i][j]);
      }
    }

    drawContours(src, contours, i, Scalar(0, 0, 0), 2);
  }
}
*/
bool sortbysec(const NTUDataPryDownImage& a, const NTUDataPryDownImage& b) { return (a.score > b.score); }

void NTUPattern::computer()
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
    sceneData.push_back(NTUDataPryDownImage(matRoi, i, mLevelMax));
    sceneData.push_back(NTUDataPryDownImage(matRoi, -1 * i, mLevelMax));
  }
  //! [7]

  //! [8] find template matching
  std::vector<NTUDataPryDownImage> dataMatching;

  for (size_t i = 0; i < sceneData.size(); i++)
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

  //! [10] if object matching is avaiable find best matching with smalless score
  std::sort(dataMatching.begin(), dataMatching.end(), sortbysec);
  NTUDataPryDownImage dataresult = dataMatching.front();
  //! [10]

  cv::Mat roiCandidateMat;
  Rect recroiFound(dataresult.centerROI.x - mRectObj.width/2.f, dataresult.centerROI.y-mRectObj.height/2.f, mRectObj.width, mRectObj.height);
  dataMatching.front().image.front()(recroiFound).copyTo(roiCandidateMat);

  double score = NTUCVTools::getSimilarity(roiCandidateMat, mDataTemplateImage.image.front());

  if (score > mAceptThreshold)
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

  //! [11] get result
  mScore = score;
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

  Point2f centerRotate = Point2f((dataresult.image.front().cols) / 2.f, (dataresult.image.front().rows) / 2.f);

  /// diffirence size after rotation
  float deltaWidth = (matRoi.cols - dataresult.image.front().cols) / 2.f;
  float deltaHeight = (matRoi.rows - dataresult.image.front().rows) / 2.f;

  Point2f translatePoint = Point2f(deltaWidth, deltaHeight) + pointTransform;

  //! [14] cross points
  vector<Point2f> crossPoinst = {{pt_center.x - cross_length, pt_center.y},
                                 {pt_center_offset.x + cross_length, pt_center.y},
                                 {pt_center_offset.x, pt_center.y - cross_length},
                                 {pt_center_offset.x, pt_center.y + cross_length}};

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
  mPoints = NTUCVTools::rotatePoints(pointsOfRectangleObject, centerRotate, -mAngle);

  for (size_t i = 0; i < mPoints.size(); ++i)
  {
    mPoints[i] += translatePoint;
  }
  //! [7]

  //! [8]

  vector<Point2f> regionPoints = {roiTransform.tl(), Point2f(roiTransform.tl().x, roiTransform.br().y), roiTransform.br(),
                                  Point2f(roiTransform.br().x, roiTransform.tl().y)};
  if (mToolFixture != nullptr)
  {

    mRegionPoints = NTUCVTools::rotatePoints(regionPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mPoints = NTUCVTools::rotatePoints(mPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mCoordinate = NTUCVTools::rotatePoint(mCoordinate, mToolFixture->mCoordinate, -mToolFixture->mAngle);
    mCrossPoints = NTUCVTools::rotatePoints(mCrossPoints, mToolFixture->mCoordinate, -mToolFixture->mAngle);
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

void NTUPattern::getDrawResult(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
    angle = mToolFixture->mAngle;
  if (mResultOK)
  {
    // NTUCVTools::drawPolygon(image, mPoints, color_OK, lineWidth);
    NTUCVTools::drawPolygon(image, mRegionPoints, color_OK, lineWidth);
    NTUCVTools::drawText(image, mName, mRegionPoints[0], 0, color_OK, FONT_SIZE, Point2f(-10, -10));
    NTUCVTools::drawArrowLine(image, mCrossPoints[0], mCrossPoints[1], color_OK, lineWidth);
    NTUCVTools::drawArrowLine(image, mCrossPoints[2], mCrossPoints[3], color_OK, lineWidth);
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
    NTUCVTools::drawText(image, mName, mRegionPoints[0], 0, color_NG, FONT_SIZE, Point2f(-10, -10));
    // NTUCVTools::drawPoint(image, mCoordinate, color_NG, lineWidth);
  }
}

void NTUPattern::getDrawRegion(QImage& image)
{
  double angle = 0;
  if (mToolFixture != nullptr)
    angle = mToolFixture->mAngle;
  Scalar color;
  if (mResultOK)
    color = color_OK;
  else
    color = color_NG;

  if (mRegionPoints.empty())
  {

    Rect roiTransform;
    Mat mat_tran;
    if (!tranformFollowToolFixture(roiTransform, mat_tran))
      return;

    //! [8]
    vector<Point2f> regionPoints = {roiTransform.tl(), Point2f(roiTransform.tl().x, roiTransform.br().y), roiTransform.br(),
                                    Point2f(roiTransform.br().x, roiTransform.tl().y)};

    if (mToolFixture != nullptr)
    {
      regionPoints = NTUCVTools::rotatePoints(regionPoints, mCoordinate, -mToolFixture->mAngle);
    }

    NTUCVTools::drawPolygon(image, regionPoints, color, lineWidth);
    NTUCVTools::drawText(image, mName, regionPoints[0], 0, color, FONT_SIZE, Point2f(-10, -10));
  }
  else
  {

    NTUCVTools::drawPolygon(image, mRegionPoints, color, lineWidth);
    NTUCVTools::drawText(image, mName, mRegionPoints[0], 0, color, FONT_SIZE, Point2f(-10, -10));
  }
}

void NTUPattern::setImageTemplate(const Mat src)
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

  blur(mat_tmp, mImageTemplate, cv::Size(3, 3));

  int npyrimg = 1;
  cv::Size imgpsize = mImageTemplate.size();
  while (imgpsize.width > (int)MIN_SIZE_DOWNSAMPLE)
  {
      imgpsize = cv::Size(imgpsize.width / 2, imgpsize.height / 2);
      npyrimg++;
  }

  mLevelMax = npyrimg-1;

  // bilateralFilter(mat_tmp, mImageTemplate, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);
  mDataTemplateImage = (NTUDataPryDownImage(mImageTemplate, 0, mLevelMax));

  mFirst = true;

  Q_EMIT newImageTemplate(mat_tmp);
}

void NTUPattern::onReadSettingFromJSon(QJsonObject* setting)
{

  mAngleTolerance = setting->value(PatternSettings[PATTERN_ANGLE_TOLERANCE]).toInt();
  mAceptThreshold = setting->value(PatternSettings[PATTERN_ACCEPT_THRESHOLD]).toDouble();
  mOffsetX = setting->value(PatternSettings[PATTERN_HORIZONTAL_OFFSET]).toDouble();
  mOffsetY = setting->value(PatternSettings[PATTERN_VERTICAL_OFFSET]).toDouble();
  QString rectSearch = setting->value(PatternSettings[PATTERN_SEARCH]).toString();
  QString rectcObj = setting->value(PatternSettings[PATTERN_OBJECT]).toString();
  // setting->value(PatternSettings[TIME_OUT], QJsonValue::fromVariant(timeOut));

  if (!cvtools::getRectFromQString(rectSearch, mRectRoi)) // not corect
    return;

  mFirstmCoordinate = Point2f(mRectRoi.x + mRectRoi.width / 2.0, mRectRoi.y + mRectRoi.height / 2.0);
  mCoordinate = mFirstmCoordinate;

  if (!cvtools::getRectFromQString(rectcObj, mRectObj)) // not corect
    return;

  mFirst = true;
}
