#ifndef SMVIDEOZOOMWIDGET_H
#define SMVIDEOZOOMWIDGET_H

#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QObject>
#include <QTest>

#include <QDebug>
#include <QMenu>
#include <QScrollBar>
#include <opencv2/opencv.hpp>

#include "cvtools.h"
#include "shapeTools/NTUShape.h"

class NTURectangle;
class NTUTools;

class SMVideoZoomWidget : public QGraphicsView
{
  Q_OBJECT

public:
  explicit SMVideoZoomWidget(QWidget* parent = nullptr);
  ~SMVideoZoomWidget();

  cv::Mat getImage(void) { return mImage; }

  void addNewShape(QString toolName, NTUShape* shape);
  void removeShape(QString toolName, NTUShape* shape);

  void addNewNTUTool(QString toolName, NTUTools* tool);

  void addNewNTUToolDrawRegion(QString toolName, NTUTools* tool);
  void removeNTUToolDrawRegion(QString toolName, NTUTools* tool);

  void removeToolWithName(QString name);

public Q_SLOTS:
  void showImageCV(cv::Mat image);
  void showPixmap(QPixmap image);
  void fitImage();

  void zoomIn();
  void zoomOut();

  void drawImage();

  void onReceiveResult(bool pass, int total, int ok, int ng);

  void onReceiveCursor(QCursor type);

Q_SIGNALS:
  void newResolution(int w, int h);
  void newMouseMovePosition(QPointF pos);
  void newMousePressPosition(QPointF pos);
  void newMouseRelease();
  void newImageInput(cv::Mat image);
  void newExcureProject();

protected:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void resizeEvent(QResizeEvent* event);

protected Q_SLOTS:
  virtual void showContextMenu(const QPoint& pos);

private:
  QGraphicsScene* scene = nullptr;
  QGraphicsPixmapItem* item = nullptr;

  cv::Mat mImage;

  int mTotal;
  int mOK;
  int mNG;
  bool mPass;

  QMap<QString, NTUShape*> mDrawShapeManager;
  QMap<QString, NTUTools*> mNTUTools;
  QMap<QString, NTUTools*> mNToolsDrawRegion;

  bool mFisrtSet;
};

#endif // SMVIDEOZOOMWIDGET_H
