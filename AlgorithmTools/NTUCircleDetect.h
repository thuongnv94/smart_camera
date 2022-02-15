#ifndef NTUCIRCLEDETECT_H
#define NTUCIRCLEDETECT_H

#include "NTUTools.h"
#include <QObject>

using namespace cv;
using namespace std;

static QString const CircleDetectSettings[] = {"ThresholdMethod", "EdgeContrast", "EdgeTransition", "EdgeWidth", "CircleIn", "CircleOut"};
enum CircleDetect_param
{CIRCLE_DETECT_THRESHOLD_MEDTHOD,
    CIRCLE_DETECT_EDGE_CONTRAST,
    CIRCLE_DETECT_EDGETRANSITION,
    CIRCLE_DETECT_EDGE_WIDTH,
    CIRCLE_DETECT_CERCLE_IN,
    CIRCLE_DETECT_CERCLE_OUT
};


class NTUCircleDetect : public NTUTools
{

public:
  NTUCircleDetect(QString name);

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
  float mRadiusIner;
  float mRadiusOuter;
  float mRadius;
  int mEdgeContrast;
  int mEdgeWidth;
  int mEdgeTransition;
  int mThresholdMethod;

protected:
};

#endif // NTUCIRCLEDETECT_H
