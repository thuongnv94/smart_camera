#ifndef NTUBLOB_H
#define NTUBLOB_H

#include "NTUTools.h"

using namespace cv;
using namespace std;

static QString const BlobSettings[] = {"ThresholdMode", "BlobThreshold", "BlobColor", "MinimunArea", "MaximumArea", "Search"};
enum Blob_param
{
  BLOB_THRESHOLD_MODE,
  BLOB_THRESHOLD,
  BLOB_COLOR,
  BLOB_MINIMUM_AREA,
  BLOB_MAXIMUM_AREA,
  BLOB_SEARCH
};

class NTUBlob : public NTUTools
{
  enum thresholdMode
  {
    MANUAL,
    AUTO
  };
  enum blobColor
  {
    BLACK,
    WHITE,
    EITHER
  };

public:
  NTUBlob(QString name);

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
  ///
  virtual void computer() override;

private:
  double mThreshold;
  int mMinimumArea;
  int mMaximumArea;
  int mBlobColor;
  int mThresholdMode;
  vector<vector<cv::Point2f>> mContours;
};

#endif // NTUBLOB_H
