#include "NTURectangle.h"
#include "cvtools.h"

NTURectangle::NTURectangle(QString name, int x, int y, int w, int h) : NTUShape(nullptr, name)
{

  mTl = QPointF(x, y);
  mBR = QPointF(w, h);
}

NTURectangle::~NTURectangle() {}

void NTURectangle::draw(QImage& image)
{
  //! [1] get size of Image
  mImageWidth = image.width();
  mImageHeight = image.height();
  //! [1]

  QPainter painter(&image);
  painter.setPen(QPen(mColor, LINE_WIDTH));

  vector<QRectF> rects = getRectanglesFromPoints(mTl, mBR);
  if (mSelected)
  {
    for (int i = 0; i < rects.size(); ++i)
    {
      if (mState == (MOUSE_STATE)i)
      {
        painter.setPen(QPen(colorActive, LINE_WIDTH));
      }
      else
      {
        painter.setPen(QPen(mColor, LINE_WIDTH));
      }
      painter.drawRect(rects[i]);
    }
  }
  else
  { // draw only main rectangle
    painter.drawRect(rects.back());
  }
  QFont font = QApplication::font();
  font.setPixelSize(FONT_SIZE);
  painter.setFont(font);
  painter.drawText(rects.back().topLeft() - QPointF(10, 10), mName);
}

bool NTURectangle::isContain(QPointF pos)
{
  vector<QRectF> rects = getRectanglesFromPoints(mTl, mBR);

  for (int i = 0; i < rects.size(); ++i)
  {
    if (rects.at(i).contains(pos))
    {
      mState = (MOUSE_STATE)i;
      return true;
    }
  }

  mState = NOTHING;
  return false;
}

QRect NTURectangle::getRegtangle()
{
  QRect rect;
  rect.setTopLeft(QPoint(mTl.x(), mTl.y()));
  rect.setBottomRight(QPoint(mBR.x(), mBR.y()));
  return rect;
}

void NTURectangle::setDimension(QString text)
{
  cv::Rect2f rect;
  if (!cvtools::getRectFromQString(text, rect)) // not corect
    return;

  mTl.setX(rect.x);
  mTl.setY(rect.y);
  mBR.setX(rect.x + rect.width);
  mBR.setY(rect.y + rect.height);
}

void NTURectangle::onMouseReleaseEvent()
{
  if (mState != NOTHING)
  {
    Q_EMIT newUpdateRectangle(this->getRegtangle());
  }
  NTUShape::onMouseReleaseEvent();
}

void NTURectangle::updateShapes(QPointF pos)
{

  QPointF endPoint = pos;

  if ((pos.x() <= 0 || pos.y() <= 0 || pos.x() >= mImageWidth || pos.y() >= mImageHeight) && mState != ALL)
    return;

  float disX = endPoint.x() - mBeginPosition.x();
  float disY = endPoint.y() - mBeginPosition.y();

  mBeginPosition = endPoint;

  switch (mState)
  {
  case TOP_LEFT:

    if (pos.x() >= mBR.x() || pos.y() >= mBR.y())
      return;

    mTl.setX(pos.x());
    mTl.setY(pos.y());

    break;
  case MIDLE_TOP:
    if (pos.y() >= mBR.y())
      return;
    mTl.setY(pos.y());
    break;
  case TOP_RIGHT:
    if (pos.x() <= mTl.x() || pos.y() >= mBR.y())
      return;

    mTl.setY(pos.y());
    mBR.setX(pos.x());
    break;
  case MIDLE_LEFT:
    if (pos.x() >= mBR.x())
      return;

    mTl.setX(pos.x());
    break;
  case MIDLE_RIGHT:
    if (pos.x() <= mTl.x())
      return;
    mBR.setX(pos.x());
    break;
  case BOTTOM_LEFT:
    if (pos.x() >= mBR.x() || pos.y() <= mTl.y())
      return;
    mTl.setX(pos.x());
    mBR.setY(pos.y());
    break;
  case MIDLE_BOTTOM:
    if (pos.y() <= mTl.y())
      return;
    mBR.setY(pos.y());
    break;
  case BOTTOM_RIGHT:
    if (pos.x() <= mTl.x() || pos.y() <= mTl.y())
      return;
    mBR.setX(pos.x());
    mBR.setY(pos.y());
    break;
  case ALL:
  {
    int tlX = mTl.x() + disX;
    int tlY = mTl.y() + disY;
    int brX = mBR.x() + disX;
    int brY = mBR.y() + disY;

    if (tlX <= 0 || tlY <= 0 || tlX >= mImageWidth - 1 || tlY >= mImageHeight - 1 || brX <= 0 || brY <= 0 || brX >= mImageWidth - 1 ||
        brY >= mImageHeight)
      return;

    mTl.setX((mTl.x() + disX));
    mTl.setY((mTl.y() + disY));
    mBR.setX((mBR.x() + disX));
    mBR.setY((mBR.y() + disY));
    break;
  }
  default:
    break;
  }
}

vector<QRectF> NTURectangle::getRectanglesFromPoints(QPointF tl, QPointF br)
{
  vector<QRectF> rects;

  QRectF rect;

  int width = 10;
  // top left
  rect.setTopLeft(QPointF(0, 0));
  rect.setWidth(width);
  rect.setHeight(width);

  width /= 2;
  rect.moveTo(tl.x() - width, tl.y() - width);
  rects.push_back(rect);

  // midle top
  rect.moveTo((br.x() - tl.x()) / 2.0 + tl.x() - width, tl.y() - width);
  rects.push_back(rect);

  // right top
  rect.moveTo(br.x() - width, tl.y() - width);
  rects.push_back(rect);

  // MIDLE_LEFT
  rect.moveTo(tl.x() - width, (br.y() - tl.y()) / 2.0 + tl.y() - width);
  rects.push_back(rect);

  // MIDLE_RIGHT
  rect.moveTo(br.x() - width, (br.y() - tl.y()) / 2.0 + tl.y() - width);
  rects.push_back(rect);

  // bottom left
  rect.moveTo(tl.x() - width, br.y() - width);
  rects.push_back(rect);

  // bottom midle
  rect.moveTo((br.x() - tl.x()) / 2.0 + tl.x() - width, br.y() - width);
  rects.push_back(rect);

  // bottom right
  rect.moveTo(br.x() - width, br.y() - width);
  rects.push_back(rect);

  rect.setTopLeft(tl);
  rect.setBottomRight(br);
  rects.push_back(rect);
  return rects;
}

void NTURectangle::onReceiverMousePosition(QPointF pos)
{
  if (mSelected)
  {
    if (!mActive)
    {
      int lasState = mState;
      QCursor type;
      if (this->isContain(pos))
      {
        // qDebug() << "Contain\n";
        switch (mState)
        {
        case TOP_LEFT:
          type = Qt::SizeFDiagCursor;
          break;
        case MIDLE_TOP:
          type = Qt::SizeVerCursor;
          break;
        case TOP_RIGHT:
          type = Qt::SizeBDiagCursor;
          break;
        case MIDLE_LEFT:
          type = Qt::SizeHorCursor;
          break;
        case MIDLE_RIGHT:
          type = Qt::SizeHorCursor;
          break;
        case BOTTOM_LEFT:
          type = Qt::SizeBDiagCursor;
          break;
        case MIDLE_BOTTOM:
          type = Qt::SizeVerCursor;
          break;
        case BOTTOM_RIGHT:
          type = Qt::SizeFDiagCursor;
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

      if (lasState == NOTHING && mState == NOTHING)
        ;
      else
        Q_EMIT newMouseCursor(type);

      QApplication::processEvents();
    }
    else
    {
      this->updateShapes(pos);
    }
  }
}

void NTURectangle::onMousePressEvent(QPointF pos)
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
    if (((pos.x() <= mTl.x() + pad) && (pos.x() > mTl.x() - pad)) || ((pos.x() <= mBR.x() + pad) && (pos.x() > mBR.x() - pad)) ||
        ((pos.y() <= mTl.y() + pad) && (pos.y() > mTl.y() - pad)) || ((pos.y() <= mBR.y() + pad) && (pos.y() > mBR.y() - pad)))
    {
      mColor = colorSelected;
      mSelected = true;
      mActive = false;
    }
  }
}
