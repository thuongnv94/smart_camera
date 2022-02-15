#ifndef NTUSHAPE_H
#define NTUSHAPE_H

#include <QObject>

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QPointF>
#include <QRect>

using namespace std;

#define RECT_SPACE 40
#define LINE_WIDTH 4
#define FONT_SIZE 18

static QColor colorDefault(43, 134, 38);
static QColor colorSelected(33, 206, 36);
static QColor colorActive(136, 32, 199);

enum MOUSE_STATE
{
  TOP_LEFT,
  MIDLE_TOP,
  TOP_RIGHT,
  MIDLE_LEFT,
  MIDLE_RIGHT,
  BOTTOM_LEFT,
  MIDLE_BOTTOM,
  BOTTOM_RIGHT,
  ALL,
  NOTHING
};

class NTUShape : public QObject
{
  Q_OBJECT
public:
  explicit NTUShape(QObject* parent = nullptr, QString name = "unname");
  virtual ~NTUShape() {}

  ///
  /// \brief draw
  /// \param image
  ///
  virtual void draw(QImage& image) = 0;

  ///
  /// \brief setColor
  /// \param color
  ///
  void setColor(QColor color);

  ///
  /// \brief isContain
  /// \param pos
  ///
  virtual bool isContain(QPointF pos) = 0;

  ///
  /// \brief getIsActive
  /// \return
  ///
  bool getIsActive() { return mActive; }

  ///
  /// \brief getState
  /// \return
  ///
  MOUSE_STATE getState() { return mState; }

  ///
  /// \brief getName
  /// \return
  ///
  QString getName() { return mName; }

  ///
  /// \brief setDimension
  /// \param text
  ///
  virtual void setDimension(QString text) = 0;

Q_SIGNALS:
  void newMouseCursor(QCursor type);
  void newCenter(double x, double y);

public Q_SLOTS:
  ///
  /// \brief onReceiverMousePosition
  /// \param pos
  ///
  virtual void onReceiverMousePosition(QPointF pos) = 0;

  ///
  /// \brief pressEvent
  /// \param pos
  ///
  virtual void onMousePressEvent(QPointF pos) = 0;

  ///
  /// \brief releaseEvent
  ///
  virtual void onMouseReleaseEvent() = 0;

  ///
  /// \brief onReceiverCenter
  /// \param x
  /// \param y
  ///
  virtual void onReceiverCenter(double x, double y) = 0;

protected:
  virtual void updateShapes(QPointF pos) = 0;

  ///
  /// \brief mImage
  ///
  QImage mImage;

  ///
  /// \brief mColor
  ///
  QColor mColor;

  ///
  /// \brief mState
  ///
  MOUSE_STATE mState;

  ///
  /// \brief mBeginPosition
  ///
  QPointF mBeginPosition;

  ///
  /// \brief mActive
  ///
  bool mActive;

  ///
  /// \brief mImageWidth
  ///
  int mImageWidth;

  ///
  /// \brief mImageHeight
  ///
  int mImageHeight;

  ///
  /// \brief mSelected
  ///
  bool mSelected;

  ///
  /// \brief mName
  ///
  QString mName;
};

#endif // NTUSHAPE_H
