#include "NTUTools.h"
#include "NTUCVTools.h"

NTUTools::NTUTools(QString name) : QObject(nullptr), mName(name) //, mRectObj(Rect(0, 0, 0, 0)), mRectRoi(Rect(0, 0, 0, 0))
{
  mToolFixture = nullptr;
  mCoordinate = cv::Point2f(-1.0, -1.0);
  mFirstmCoordinate = cv::Point2f(-1.0, -1.0);

  mDeltaX = 0.;
  mDeltaY = 0.;
  mScale = 1.;
  mAngle = 0.;

  mOffsetX = 0.;
  mOffsetY = 0.;

  mFirst = true;
  mResultOK = false;
  mIsBusy = false;
  mRectRoi = Rect2d(50, 50, 200, 200);
  mRectObj = Rect2d(60, 60, 100, 100);
}

bool NTUTools::tranformFollowToolFixture(Rect& roiTranform, Mat& matTranform)
{
  if (mRectRoi.x <= 0 || mRectRoi.y <= 0 || mRectRoi.width <= 0 || mRectRoi.height <= 0)
  {
      std::cerr << this->mName.toStdString() << " ,Recroi: ERROR!\n";
    return false;
  }
  roiTranform = mRectRoi;

  if (this->mToolFixture != nullptr)
  {
    matTranform = NTUCVTools::rotateImage(mImage, mToolFixture->mAngle, true, mToolFixture->mCoordinate);
    // roiTranform = NTUCVTools::transformRect(mRectRoi, mToolFixture->mDeltaX, mToolFixture->mDeltaY);
    roiTranform.x += mToolFixture->mDeltaX;
    roiTranform.y += mToolFixture->mDeltaY;
    std::cerr << "Tool Fix Tran: " << mToolFixture->mDeltaX << ", " << mToolFixture->mDeltaY << "\n";
    if (roiTranform.x < 0 || roiTranform.y < 0 || roiTranform.br().x > matTranform.cols - 1 || roiTranform.br().y > matTranform.rows - 1)
      {
        std::cerr << this->mName.toStdString() << " ROI after Tran ERROR\n";
        return false;
    }
    //imshow("TranSfrom", matTranform);
    //    Mat mat_show;
    //    matTranform(roiTranform).copyTo(mat_show);
    //    imshow("ROI", mat_show);
    return true;

  }
  else
  {
    roiTranform = mRectRoi;
    mImage.copyTo(matTranform);
  }
  return true;
}

NTUTools* NTUTools::getToolFixture() { return mToolFixture; }

int NTUTools::getExecutionTime() { return mExecutionTime; }

void NTUTools::setToolName(QString name) { this->mName = name; }

void NTUTools::onReadSettingFromJSon(QJsonObject* setting) {}

void NTUTools::setToolFixture(NTUTools* tools)
{
  if (tools)
    mToolFixture = dynamic_cast<NTUTools*>(tools);
  else
  {
    if (mToolFixture != nullptr)
      mToolFixture = nullptr;
  }
}

void NTUTools::onReceiverInputImage(cv::Mat src)
{
  mProcessMutex.lock();
  // 8-bits unsigned, raw bayer image
  if (src.type() == CV_8UC1)
  {
    src.copyTo(mImage);
    // 8-bit unsigned rgb image
  }
  else if (src.type() == CV_8UC3)
  {
    cvtColor(src, mImage, COLOR_BGR2GRAY);
  }
  mProcessMutex.unlock();
  // this->computer();
}
