#include "NTUCircle.h"
#include "cvtools.h"
#include <cmath>
#include <iostream>
#include <stdlib.h>

NTUCircle::NTUCircle(QString name, int x, int y, int radius) : NTUShape(nullptr, name)
{
  mCenter.setX(x);
  mCenter.setY(y);
  mRadius = radius;
}

void NTUCircle::onReceiverMousePosition(QPointF pos)
{
  if (mSelected)
  {
    if (!mActive)
    {
      int lastState = mState;
      QCursor type;
      if (this->isContain(pos))
      {
        switch (mState)
        {
        case MIDLE_TOP:
          type = Qt::SizeVerCursor;
          break;
        case MIDLE_LEFT:
          type = Qt::SizeHorCursor;
          break;
        case MIDLE_RIGHT:
          type = Qt::SizeHorCursor;
          break;
        case MIDLE_BOTTOM:
          type = Qt::SizeVerCursor;
          break;
        case ALL:
          type = Qt::SizeAllCursor;
          break;
        default:
          type = Qt::ArrowCursor;
          break;
        }
      }
      else
      {
        type = Qt::ArrowCursor;
      }

      if (lastState == NOTHING && mState == NOTHING)
        ;
      else
        Q_EMIT newMouseCursor(type);
      QApplication::processEvents();
    }
    else
    {
      this->updateShapes(pos);
      QApplication::processEvents();
    }
  }
}

void NTUCircle::onMousePressEvent(QPointF pos)
{
  int pad = 4;

  if (mSelected)
  {
    if (this->isContain(pos) && mState != NOTHING)
    {
      mBeginPosition = pos;
      mActive = true;
    }
    else
    {
      mColor = colorDefault;
      mSelected = false;
    }
  }
  else
  {
    double distance = sqrt((double)(mCenter.x() - pos.x()) * (mCenter.x() - pos.x()) + (mCenter.y() - pos.y()) * (mCenter.y() - pos.y()));

    if ((distance <= mRadius + pad) && (distance > mRadius - pad))
    {
      mColor = colorSelected;
      mSelected = true;
      mActive = false;
    }
  }
}

void NTUCircle::draw(QImage& image)
{
  //! [1] get size of Image
  mImageWidth = image.width();
  mImageHeight = image.height();
  //! [1]

  QPainter painter(&image);
  painter.setPen(QPen(mColor, LINE_WIDTH));

  vector<QRectF> rects = getRectanglesFromPoints(mCenter, mRadius);
  QBrush brush;

  if (mSelected)
  {
    for (int i = 0; i < rects.size(); ++i)
    {
      if (mState == getStateFromID(i))
      {
        brush = QBrush(colorActive);
      }
      else
        brush = QBrush(mColor);

      painter.fillRect(rects[i], brush);
    }

    if (mState == ALL)
    {
      painter.setPen(QPen(colorActive, LINE_WIDTH));
    }
    else
      painter.setPen(QPen(mColor, LINE_WIDTH));
  }
  else
  {
    painter.setPen(QPen(mColor, LINE_WIDTH));
  }

  painter.drawEllipse(mCenter, mRadius, mRadius);
}

bool NTUCircle::isContain(QPointF pos)
{
  vector<QRectF> rects = getRectanglesFromPoints(mCenter, mRadius);

  for (int i = 0; i < rects.size(); ++i)
  {
    if (rects.at(i).contains(pos))
    {
      mState = getStateFromID(i);
      return true;
    }
  }

  double distance = sqrt((double)(mCenter.x() - pos.x()) * (mCenter.x() - pos.x()) + (mCenter.y() - pos.y()) * (mCenter.y() - pos.y()));

  if (distance <= mRadius)
  {
    mState = ALL;
    return true;
  }

  mState = NOTHING;
  return false;
}

void NTUCircle::onReceiverCenter(double x, double y)
{
  mCenter.setX(x);
  mCenter.setY(y);
  // Q_EMIT newUpdateRectangle(QRect(mCenter.x() - mRadius, mCenter.y() - mRadius, mRadius * 2.f, mRadius * 2.f));
}

QRect NTUCircle::getRegtangle() { return QRect(mCenter.x() - mRadius, mCenter.y() - mRadius, mRadius * 2.f, mRadius * 2.f); }

void NTUCircle::setDimension(QString text)
{
  cv::Point2f point;
  float radius;
  if (!cvtools::getCircleFromQString(text, point, radius))
    return;
  mCenter.setX(point.x);
  mCenter.setY(point.y);
  mRadius = radius;
}

void NTUCircle::onMouseReleaseEvent()
{

  if (mState != NOTHING)
  {
    Q_EMIT newUpdateRectangle(QRect(mCenter.x() - mRadius, mCenter.y() - mRadius, mRadius * 2.f, mRadius * 2.f));
  }
  NTUShape::onMouseReleaseEvent();
}

void NTUCircle::updateShapes(QPointF pos)
{

  QPointF endPoint = pos;

  if ((pos.x() <= 0 || pos.y() <= 0 || pos.x() >= mImageWidth - 1 || pos.y() >= mImageHeight - 1) && mState != ALL)
    return;

  float disX = endPoint.x() - mBeginPosition.x();
  float disY = endPoint.y() - mBeginPosition.y();

  mBeginPosition = endPoint;

  switch (mState)
  {

  case MIDLE_TOP:
  {
    int radius = std::abs(mCenter.y() - pos.y());

    if (pos.y() >= mCenter.y() || (mCenter.y() - radius) <= 0 || (mCenter.y() + radius) >= mImageHeight ||
        (mCenter.x() + radius) >= mImageWidth)
      return;
    mRadius = mCenter.y() - pos.y();
    break;
  }
  case MIDLE_LEFT:
  {
    int radius = std::abs(mCenter.x() - pos.x());
    if (pos.x() >= mCenter.x() || (mCenter.x() - radius) <= 0 || (mCenter.x() + radius) >= mImageWidth | (mCenter.y() - radius) <= 0 ||
        (mCenter.y() + radius) >= mImageHeight)
      return;

    mRadius = mCenter.x() - pos.x();
    break;
  }
  case MIDLE_RIGHT:
  {
    int radius = std::abs(mCenter.x() - pos.x());
    if (pos.x() <= mCenter.x() || (mCenter.x() - radius) <= 0 || (mCenter.x() + radius) >= mImageWidth | (mCenter.y() - radius) <= 0 ||
        (mCenter.y() + radius) >= mImageHeight)
      return;
    mRadius = pos.x() - mCenter.x();
    break;
  }
  case MIDLE_BOTTOM:
  {
    int radius = std::abs(mCenter.y() - pos.y());

    if (pos.y() <= mCenter.y() || (mCenter.y() - radius) <= 0 || (mCenter.y() + radius) >= mImageHeight)
      return;
    mRadius = pos.y() - mCenter.y();
    break;
  }
  case ALL:
  {

    int tlX = mCenter.x() + disX - mRadius;
    int tlY = mCenter.y() + disY - mRadius;
    int brX = mCenter.x() + disX + mRadius;
    int brY = mCenter.y() + disY + mRadius;

    if (tlX <= 0 || tlY <= 0 || tlX >= mImageWidth || tlY >= mImageHeight || brX <= 0 || brY <= 0 || brX >= mImageWidth ||
        brY >= mImageHeight)
      return;

    double x = (mCenter.x() + disX);
    double y = (mCenter.y() + disY);
    mCenter.setX(x);
    mCenter.setY(y);
    Q_EMIT newCenter(x, y);

    break;
  }
  default:
    break;
  }
}

MOUSE_STATE NTUCircle::getStateFromID(int index)
{
  MOUSE_STATE state = NOTHING;
  switch (index)
  {
  case 0:
    state = MIDLE_TOP;
    break;
  case 1:
    state = MIDLE_LEFT;
    break;
  case 2:
    state = MIDLE_RIGHT;
    break;
  case 3:
    state = MIDLE_BOTTOM;
    break;
  case 4:
    state = ALL;
    break;
  default:
    state = NOTHING;
    break;
  }
  return state;
}

vector<QRectF> NTUCircle::getRectanglesFromPoints(QPointF center, int radius)
{
  vector<QRectF> rects;

  QRectF rect;

  int width = 10;
  // top left
  rect.setTopLeft(QPointF(0, 0));
  rect.setWidth(width);
  rect.setHeight(width);

  width /= 2;

  // midle top
  rect.moveTo(center.x() - width, (center.y() - radius) - width);
  rects.push_back(rect);

  // MIDLE_LEFT
  rect.moveTo((center.x() - radius) - width, center.y() - width);
  rects.push_back(rect);

  // MIDLE_RIGHT
  rect.moveTo(center.x() + radius - width, center.y() - width);
  rects.push_back(rect);

  // bottom midle
  rect.moveTo(center.x() - width, center.y() + radius - width);
  rects.push_back(rect);

  return rects;
}
