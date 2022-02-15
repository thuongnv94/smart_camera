
#ifndef NTUCOLOR_H
#define NTUCOLOR_H

#include "NTUTools.h"
#include <QObject>
#include "NTUCVTools.h"

using namespace cv;
using namespace std;

static QString const ColorSettings[] = {"ColorHSVMin", "ColorHSVMax", "ColorCount", "ColorSearch", "ColorTimeOut"};
enum Color_param
{
  NTU_COLOR_HSV_MIN,
  NTU_COLOR_HSV_MAX,
  NTU_COLOR_COUNT,
  NTU_COLOR_SEARCH,
  NTU_COLOR_TIME_OUT
};

class NTUColor : public NTUTools
{

public:
  NTUColor(QString name);

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

  virtual void onReceiverInputImage(cv::Mat src) override;


private:
  int mHMin;
  int mHMax;
  int mSMin;
  int mSMax;
  int mVMin;
  int mVMax;
  int mCount;

protected:
};

#endif // NTUCOLOR_H
