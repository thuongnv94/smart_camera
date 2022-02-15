#ifndef NTUPATTERN_H
#define NTUPATTERN_H
#include "NTUCVTools.h"
#include "NTUTools.h"
#include <QDebug>
#include <QObject>

static QString const PatternSettings[] = {"angleTolerance",   "acceptThreshold", "search", "object",
                                          "horizontalOffset", "verticalOffset",  "timeOut", "toolFixture", };

enum Pattern_param
{
  PATTERN_ANGLE_TOLERANCE,
  PATTERN_ACCEPT_THRESHOLD,
  PATTERN_SEARCH,
  PATTERN_OBJECT,
  PATTERN_HORIZONTAL_OFFSET,
  PATTERN_VERTICAL_OFFSET,
  PATTERN_TIME_OUT,
  PATTERN_TOOLFIXTURE
};

class NTUPattern : public NTUTools
{
#define TEMPLATEMATCHING_SCORE 0.5
#define BLUR_SIZE 3
public:
  NTUPattern(QString name);

  ///
  /// \brief getDrawResult
  /// \param src
  ///
  virtual void getDrawResult(QImage& image) override;

  ///
  /// \brief getDrawRegion
  /// \param image
  ///
  virtual void getDrawRegion(QImage &image) override;

  ///
  /// \brief setImageTemplate
  /// \param src
  ///
  virtual void setImageTemplate(const Mat src) override;


public Q_SLOTS:
  ///
  /// \brief onReadSettingFromJSon
  /// \param setting
  ///
  virtual void onReadSettingFromJSon(QJsonObject* setting) override;


  ///
  /// \brief computer
  /// \return
  ///
  virtual void computer() override;

private:
  ///
  /// \brief templateMachingMultilevel
  /// \param scene
  /// \param obj
  /// \param out
  /// \return
  ///
  bool templateMachingMultilevel(const NTUDataPryDownImage scene, const NTUDataPryDownImage obj, NTUDataPryDownImage& out);
/*
  ///
  /// \brief subFindRotate
  /// \param sr1
  /// \param sr2
  /// \return
  ///
  double subFindRotate(Mat sr1, Mat sr2);

  ///
  /// \brief getContourFromMat
  /// \param src
  /// \param contour
  ///
  void getContourFromMat(Mat src, vector<Point2f>& contour_out);
*/
  int mAngleTolerance;
  double mAceptThreshold;
  int mLevelMax;


  NTUDataPryDownImage mDataTemplateImage;
};
//Q_DECLARE_METATYPE(NTUPattern);
#endif // NTUPATTERN_H
