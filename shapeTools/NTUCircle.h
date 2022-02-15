#ifndef NTUCIRCLE_H
#define NTUCIRCLE_H

#include "NTUShape.h"
#include <QObject>

class NTUCircle : public NTUShape
{
  Q_OBJECT
public:
  explicit NTUCircle(QString name = "unname", int x = 100, int y = 100, int radius = 100);

public Q_SLOTS:
  ///
  /// \brief onReceiverMousePosition
  /// \param pos
  ///
  virtual void onReceiverMousePosition(QPointF pos) override;

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
  /// \brief onReceiverCenter
  /// \param x
  /// \param y
  ///
  virtual void onReceiverCenter(double x, double y) override;

  ///
  /// \brief getRegtangle
  /// \return
///
  QRect getRegtangle();

  virtual void setDimension(QString text) override;

  Q_SIGNALS:
  void newUpdateRectangle(QRect rect);

  public Q_SLOTS:
      virtual void onMouseReleaseEvent() override;

private:
  virtual void updateShapes(QPointF pos) override;

  MOUSE_STATE getStateFromID(int index);

  vector<QRectF> getRectanglesFromPoints(QPointF center, int radius);
  int mRadius;
  QPointF mCenter;
  bool mChildrent;
};

#endif // NTUCIRCLE_H
