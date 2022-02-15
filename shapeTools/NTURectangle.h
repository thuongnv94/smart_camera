#ifndef NTURECTANGLE_H
#define NTURECTANGLE_H

#include <QObject>

#include "NTUShape.h"

class NTURectangle : public NTUShape
{
  Q_OBJECT
public:
  explicit NTURectangle(QString name = "uname", int x = 50, int y = 50, int w = 200, int h = 200);
  virtual ~NTURectangle() override;

public Q_SLOTS:
  ///
  /// \brief onReceiverMousePosition
  /// \param pos
  ///
  virtual void onReceiverMousePosition(QPointF pos) override;

  ///
  /// \brief onMousePressEvent
  /// \param pos
  ///
  virtual void onMousePressEvent(QPointF pos) override;

  ///
  /// \brief draw
  /// \param image
  ///
  virtual void draw(QImage& image) override;

  ///
  /// \brief isContain
  /// \param pos
  /// \param index
  /// \return
  ///
  virtual bool isContain(QPointF pos) override;

  ///
  /// \brief getRegtangle
  /// \return
  ///
  QRect getRegtangle();

  ///
  /// \brief onReceiverCenter
  /// \param x
  /// \param y
  ///
  virtual void onReceiverCenter(double x, double y) override {}

  ///
  /// \brief setDimension
  /// \param text
  ///s
  virtual void setDimension(QString text) override;

Q_SIGNALS:
  void newUpdateRectangle(QRect rect);

public Q_SLOTS:
  virtual void onMouseReleaseEvent() override;

private:
  ///
  /// \brief getRectanglesFromPoints
  /// \param tl
  /// \param br
  /// \return
  ///
  vector<QRectF> getRectanglesFromPoints(QPointF tl, QPointF br);

  ///
  /// \brief onUpdatePose
  /// \param pos
  ///
  virtual void updateShapes(QPointF pos) override;

  ///
  /// \brief rectangle Top left
  ///
  QPointF mTl;

  ///
  /// \brief rectangle Bottom Right
  ///
  QPointF mBR;
};

#endif // NTURECTANGLE_H
