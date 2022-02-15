#ifndef NTUPATMAX_H
#define NTUPATMAX_H

#include "NTUCVTools.h"
#include "NTUTools.h"
#include <QObject>

static QString const PatMaxSettings[] = {"contrastThreshold", "angleTolerance", "acceptThreshold", "search", "object",
                                         "horizontalOffset",  "verticalOffset", "timeOut"};

enum PatMax_param
{
  PATMAX_CONTRAST_THRESHOLD,
  PATMAX_ANGLE_TOLERANCE,
  PATMAX_ACCEPT_THRESHOLD,
  PATMAX_SEARCH,
  PATMAX_OBJECT,
  PATMAX_HORIZONTAL_OFFSET,
  PATMAX_VERTICAL_OFFSET,
  PATMAX_TIME_OUT
};

class GeoMatch;
class NTUPatMax : public NTUTools
{
#define BLUR_SIZE  3
#define HIGH_THRESHOLD 100
public:
  NTUPatMax(QString name);
  virtual ~NTUPatMax();

  virtual void computer() override;
  virtual void getDrawResult(QImage& image) override;
  virtual void getDrawRegion(QImage& image) override;
  virtual void setImageTemplate(const Mat src) override;
  virtual void onReadSettingFromJSon(QJsonObject* setting) override;

private:
  bool templateMachingMultilevel(const NTUDataPryDownImage scene, const NTUDataPryDownImage obj, NTUDataPryDownImage& out);

  int mAngleTolerance;
  double mAceptThreshold;
  int mContrastThreshold;
  GeoMatch* mGM;
  NTUDataPryDownImage mDataTemplateImage;
};

#endif // NTUPATMAX_H
