#include "NTUShape.h"

NTUShape::NTUShape(QObject* parent, QString name) : QObject(parent)
{
  mColor = colorDefault;
  mState = NOTHING;
  mActive = false;
  mSelected = false;
  mName = name;
}

void NTUShape::setColor(QColor color) { mColor = color; }

void NTUShape::onMouseReleaseEvent()
{
  mState = NOTHING;
  mActive = false;
}
