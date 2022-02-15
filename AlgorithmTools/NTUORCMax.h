#ifndef NTUORCMAX_H
#define NTUORCMAX_H

#include "NTUTools.h"
#include "opencv2/text.hpp"

using namespace cv;
using namespace cv::text;

using namespace std;

class NTUORCMax : public NTUTools
{
public:
  NTUORCMax(QString name);

  ///
  /// \brief getDrawResult
  /// \param image
  ///
  virtual void getDrawResult(QImage& image) override;
  ///
  /// \brief getDrawRegion
  /// \param image
  ///
  virtual void getDrawRegion(QImage& image) override;

  ////
  /// \brief setImageTemplate
  /// \param src
  ///
  virtual void setImageTemplate(const Mat src) override {}

  public Q_SLOTS:
  ///
  /// \brief onReadSettingFromJSon
  /// \param setting
  ///
  virtual void onReadSettingFromJSon(QJsonObject* setting) override;

  ////
  /// \brief computer
  ///
  virtual void computer() override;

private:
  bool compareListString(QStringList src1, QStringList &src2);
  QStringList mCharacter;
  QStringList mFirstCharacter;
  double mThreshold;
  vector<Rect> mRectCharacter;
  Ptr<OCRTesseract> mOCR;
  vector<vector<Point2f>> mRectanglePoints;
};

#endif // NTUORCMAX_H
