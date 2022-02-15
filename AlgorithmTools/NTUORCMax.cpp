#include "NTUORCMax.h"
#include "NTUCVTools.h"

#define CVBIL_D 7
#define CVBIL_SIG_COLOR 50
#define CVBIL_SIG_SPACE 50
#define CVCANNY_KERNEL 3

NTUORCMax::NTUORCMax(QString name) : NTUTools(name)
{

}

bool sortbyLargesRec(const Rect& a, const Rect& b) { return (a.x < b.x); }

void NTUORCMax::computer()
{
  auto start = high_resolution_clock::now();

  Rect roiTransform = mRectRoi;
  Mat mat_ROT;
  if (this->mToolFixture != nullptr)
  {
    mCorrdinate.x = mFirstmCorrdinate.x + mToolFixture->mDeltaX;
    mCorrdinate.y = mFirstmCorrdinate.y + mToolFixture->mDeltaY;
    mat_ROT = NTUCVTools::rotateImage(mImage, mToolFixture->mAngle, true, mToolFixture->getCorrdinate());

    roiTransform.x = mRectRoi.x + mToolFixture->mDeltaX;
    roiTransform.y = mRectRoi.y + mToolFixture->mDeltaY;
  }
  else
  {
    mCorrdinate = mFirstmCorrdinate;
    roiTransform = mRectRoi;
  }

  //! [1]
  cv::Mat threshold_output, imageProcess, mat_bur, mBilFilter;
  vector<vector<cv::Point>> contours, contoursFilter;
  vector<Vec4i> hierarchy;

  // blur image

  mat_ROT(roiTransform).copyTo(imageProcess);

  //  equalizeHist(imageProcess, hist_equalized_image);
  // blur(mat_bur, imageProcess, Size(3, 3));

  cv::Mat mat_canny, mThres_Gray;
  bilateralFilter(imageProcess, mBilFilter, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);

  // imshow("mBilFilter", mBilFilter);

  double CannyAccThresh = threshold(mBilFilter, mThres_Gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
  double CannyThresh = 0.1 * CannyAccThresh;

  Canny(mBilFilter, mat_canny, CannyThresh, CannyAccThresh, CVCANNY_KERNEL);
  /// Find contours
  findContours(mat_canny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<vector<Point>> contours_poly(contours.size());
  vector<Point2f> centers(contours.size());
  vector<float> radius(contours.size());
  vector<Rect> boundRect(contours.size());

  for (size_t i = 0; i < contours.size(); i++)
  {
    approxPolyDP(contours[i], contours_poly[i], 1, false);
    boundRect[i] = boundingRect(contours_poly[i]);
    minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
  }

  mRectCharacter.clear();

  for (int i = 0; i < contours_poly.size(); i++)
  {
    Moments moms = moments(contours_poly[i]);
    double area = moms.m00;

    // filter area
    //    if (area < 20)
    //      continue;

    // drawContours(drawing, contours_poly, (int)i, Scalar(0, 255, 0));
    // rectangle(drawing, boundRect[i].tl(), boundRect[i ].br(), Scalar(255, 0, 0), 2);
    contoursFilter.push_back(contours_poly[i]);
    mRectCharacter.push_back(boundRect[i]);
  }

  std::sort(mRectCharacter.begin(), mRectCharacter.end(), sortbyLargesRec);
  int borderSize = 3;

  mCharacter.clear();
  mRectanglePoints.clear();

  for (int i = 0; i < mRectCharacter.size(); ++i)
  {
    Rect rec = mRectCharacter[i];
    if (rec.area() < 100) // remove small area
      continue;

    Mat tmp, mask;
    imageProcess(rec).copyTo(tmp);

    Mat mat_thresh;

    NTUCVTools::autoThreshold(tmp, mat_thresh, BhThresholdMethod::WOLFJOLION);

    cv::copyMakeBorder(mat_thresh, mask, borderSize * 2, borderSize * 2, borderSize * 2, borderSize * 2, BORDER_CONSTANT, 255);

    std::string output_text;
    std::vector<cv::Rect> component_rects;
    std::vector<std::string> component_texts;
    std::vector<float> component_confidences;

    mOCR->run(mask, output_text, &component_rects, &component_texts, &component_confidences, cv::text::OCR_LEVEL_WORD);

    //    imshow("Mask", mask);
    //    waitKey(200);
    mCharacter.append(QString::fromStdString(component_texts[0]));

    /// rectangle move to new coordinate

    vector<Point2f> points = {{(float)rec.tl().x + roiTransform.x, (float)rec.tl().y + roiTransform.y},
                              {(float)rec.br().x + roiTransform.x, (float)rec.tl().y + roiTransform.y},
                              {(float)rec.br().x + roiTransform.x, (float)rec.br().y + roiTransform.y},
                              {(float)rec.tl().x + roiTransform.x, (float)rec.br().y + roiTransform.y}}; /// Show me what you got
    if (mToolFixture != nullptr)
    {
      vector<Point2f> pointsRot = NTUCVTools::rotatePoints(points, mToolFixture->getCorrdinate(), -mToolFixture->mAngle);

      mRectanglePoints.push_back(pointsRot);
    }
    else
      mRectanglePoints.push_back(points);
  }

  if (mFirst)
  {
    mFirstCharacter = mCharacter;
    mFirst = false;
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  mExecutionTime = duration.count();

  if (!compareListString(mFirstCharacter, mCharacter))
  {
    return false;
  }

  return true;
}

void NTUORCMax::getDrawResult(Mat& src)
{

  for (int i = 0; i < mRectanglePoints.size(); ++i)
  {
    putText(src, mCharacter.at(i).toStdString(), mRectanglePoints[i][0], FONT_HERSHEY_DUPLEX, 1.0, Scalar(0, 255, 0));
    cv::line(src, mRectanglePoints[i][0], mRectanglePoints[i][1], cv::Scalar(0, 255, 0), 1);
    cv::line(src, mRectanglePoints[i][1], mRectanglePoints[i][2], cv::Scalar(0, 255, 0), 1);
    cv::line(src, mRectanglePoints[i][2], mRectanglePoints[i][3], cv::Scalar(0, 255, 0), 1);
    cv::line(src, mRectanglePoints[i][3], mRectanglePoints[i][0], cv::Scalar(0, 255, 0), 1);
  }
}

bool NTUORCMax::compareListString(QStringList src1, QStringList& src2)
{
  if (src1.empty() || src2.empty() || src1.size() != src2.size())
    return false;

  int countOK = 0;

  for (int i = 0; i < src1.size(); ++i)
  {
    const QString text1 = src1[i];
    QString& text2 = src2[i];

    int tl1 = text1.length();
    int tl2 = text2.length();
    QString textOut;

    for (int j = 0; j < tl1; ++j)
    {
      for (int k = 0; k < tl2; ++k)
      {
        if (text1.at(j) == text2.at(k))
        {
          countOK += 1;
          textOut = QString(text2.at(k));
          goto ENDWHEN_OK;
        }
      }
    }
  ENDWHEN_OK:
    text2 = textOut;
  }

  if (countOK != src1.size())
    return false;

  return true;
}
