#include "SMVideoZoomWidget.h"
#include "AlgorithmTools/NTUTools.h"

SMVideoZoomWidget::SMVideoZoomWidget(QWidget* parent)
    : QGraphicsView(parent),

      mTotal(0), mOK(0), mNG(0), mPass(false), mFisrtSet(false)
{
  scene = new QGraphicsScene(this);
  item = new QGraphicsPixmapItem;

  scene->addItem(item);

  setScene(scene);

  // Causes weird scroll bars
  //this->setStyleSheet("background: transparent");
  this->setStyleSheet("background-color: rgb(102, 102, 102);");
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &SMVideoZoomWidget::customContextMenuRequested, this, &SMVideoZoomWidget::showContextMenu);
  this->setCacheMode(QGraphicsView::CacheBackground);
}

void SMVideoZoomWidget::showImageCV(cv::Mat image)
{

  if (image.empty())
    return;

  image.copyTo(mImage);

  Q_EMIT newImageInput(image);

  // ru nprogram
  Q_EMIT newExcureProject();

  this->drawImage();
}

void SMVideoZoomWidget::showPixmap(QPixmap image)
{

  // If first ever camera image, fit in view
  if (item->pixmap().isNull())
  {
    item->setPixmap(image);
    this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
  }
  else
  {
    item->setPixmap(image);
  }

  Q_EMIT newResolution(image.width(), image.height());
}

void SMVideoZoomWidget::mousePressEvent(QMouseEvent* event)
{
  //    // Drag mode : change the cursor's shape
  //    if (event->button() == Qt::LeftButton)
  //        this->setDragMode(QGraphicsView::ScrollHandDrag);
  // mapToScene(event->pos());

  Q_EMIT newMousePressPosition(mapToScene(event->pos()));

  this->drawImage();
  QGraphicsView::mousePressEvent(event);
}

// Called when a mouse button is pressed
void SMVideoZoomWidget::mouseReleaseEvent(QMouseEvent* event)
{
  //    // Exit drag mode : change the cursor's shape
  //    if (event->button() == Qt::LeftButton)
  //    this->setDragMode(QGraphicsView::NoDrag);

  Q_EMIT newMouseRelease();

  this->drawImage();

  QGraphicsView::mouseReleaseEvent(event);
}

void SMVideoZoomWidget::wheelEvent(QWheelEvent* event)
{
  // When zooming, the view stay centered over the mouse
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  double factor = 1.3;
  if (event->delta() > 0)
    // Zoom in
    scale(factor, factor);
  else
    // Zooming out
    scale(1.0 / factor, 1.0 / factor);

  // The event is processed
  event->accept();

//  if (this->horizontalScrollBar()->isVisible() || this->verticalScrollBar()->isVisible())
//    this->setDragMode(QGraphicsView::ScrollHandDrag);
//  else
//    this->setDragMode(QGraphicsView::NoDrag);

  this->drawImage();

  QGraphicsView::wheelEvent(event);
}

void SMVideoZoomWidget::mouseMoveEvent(QMouseEvent* event)
{

  Q_EMIT newMouseMovePosition(mapToScene(event->pos()));
  this->drawImage();
  QGraphicsView::mouseMoveEvent(event);
}

// Overloaded functionthat catch the resize event
void SMVideoZoomWidget::resizeEvent(QResizeEvent* event)
{
  //    // First call, the scene is created
  //    if(event->oldSize().width() == -1 || event->oldSize().height() == -1)
  //    return;

  //    // Get the previous rectangle of the scene in the viewport
  //    QPointF P1=mapToScene(QPoint(0,0));
  //    QPointF
  //    P2=mapToScene(QPoint(event->oldSize().width(),event->oldSize().height()));

  //    // Stretch the rectangle around the scene
  //    if (P1.x()<0) P1.setX(0);
  //    if (P1.y()<0) P1.setY(0);
  //    if (P2.x()>scene->width()) P2.setX(scene->width());
  //    if (P2.y()>scene->height()) P2.setY(scene->height());

  //    // Fit the previous area in the scene
  //    this->fitInView(QRect(P1.toPoint(),P2.toPoint()),Qt::KeepAspectRatio);

//  if (this->horizontalScrollBar()->isVisible() || this->verticalScrollBar()->isVisible())
//    this->setDragMode(QGraphicsView::ScrollHandDrag);
//  else
//    this->setDragMode(QGraphicsView::NoDrag);

  this->drawImage();
  // QGraphicsView::resizeEvent(event);
}

// Display contextual menu
void SMVideoZoomWidget::showContextMenu(const QPoint& pos)
{
  // Get the mouse position in the scene
  QPoint globalPos = mapToGlobal(pos);
  // Create the menu and add action
  QMenu contextMenu;
  contextMenu.addAction("Reset view", this, SLOT(fitImage()));
  // Display the menu
  contextMenu.exec(globalPos);
}

void SMVideoZoomWidget::drawImage()
{

  if (mImage.empty())
    return;

  QImage image = cvtools::cvMat2qImage(mImage);

  for (QMap<QString, NTUShape*>::iterator it = mDrawShapeManager.begin(); it != mDrawShapeManager.end(); ++it)
  {
    (*it)->draw(image);
  }

  for (QMap<QString, NTUTools*>::iterator it = mNTUTools.begin(); it != mNTUTools.end(); it++)
  {
    while ((*it)->mIsBusy)
    {
      QTest::qSleep(1);
    }
    (*it)->getDrawResult(image);
  }
  for (QMap<QString, NTUTools*>::iterator it = mNToolsDrawRegion.begin(); it != mNToolsDrawRegion.end(); it++)
  {
    (*it)->getDrawRegion(image);
  }

  int imageWidth = image.width() - 1;
  int imageHeigh = image.height() - 1;

  QFont font = QApplication::font();

  // If first ever camera image, fit in view
  if (item->pixmap().isNull())
  {
    item->setPixmap(QPixmap::fromImage(image));
    this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    fitImage();
  }
  else
  {
    item->setPixmap(QPixmap::fromImage(image));
  }

  Q_EMIT newResolution(image.width(), image.height());
}

void SMVideoZoomWidget::fitImage()
{
  // Turn scroll bars off temporarily to get correct widget size
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

  // Restore scroll bar policy
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QApplication::processEvents();
}

void SMVideoZoomWidget::zoomIn()
{
  // When zooming, the view stay centered over the mouse
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  scale(1.25, 1.25);
//  if (this->horizontalScrollBar()->isVisible() || this->verticalScrollBar()->isVisible())
//    this->setDragMode(QGraphicsView::ScrollHandDrag);
//  else
//    this->setDragMode(QGraphicsView::NoDrag);

  this->drawImage();
  QApplication::processEvents();
}

void SMVideoZoomWidget::zoomOut()
{
  // When zooming, the view stay centered over the mouse
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  scale(0.8, 0.8);

//  if (this->horizontalScrollBar()->isVisible() || this->verticalScrollBar()->isVisible())
//    this->setDragMode(QGraphicsView::ScrollHandDrag);
//  else
//    this->setDragMode(QGraphicsView::NoDrag);
  this->drawImage();
  QApplication::processEvents();
}

void SMVideoZoomWidget::onReceiveResult(bool pass, int total, int ok, int ng)
{
  mPass = pass;
  mTotal = total;
  mOK = ok;
  mNG = ng;
}

void SMVideoZoomWidget::onReceiveCursor(QCursor type)
{
  this->setCursor(type);
  QApplication::processEvents();
}

SMVideoZoomWidget::~SMVideoZoomWidget()
{
  if (item)
    delete item;
  if (scene)
    delete scene;
}

void SMVideoZoomWidget::addNewShape(QString toolName, NTUShape* shape)
{
  mDrawShapeManager.insert(toolName, shape);
  connect(shape, &NTUShape::newMouseCursor, this, &SMVideoZoomWidget::onReceiveCursor);
  connect(this, &SMVideoZoomWidget::newMouseRelease, shape, &NTUShape::onMouseReleaseEvent);
  connect(this, &SMVideoZoomWidget::newMousePressPosition, shape, &NTUShape::onMousePressEvent);
  connect(this, &SMVideoZoomWidget::newMouseMovePosition, shape, &NTUShape::onReceiverMousePosition);

  this->drawImage();
}

void SMVideoZoomWidget::removeShape(QString toolName, NTUShape* shape)
{
  QMap<QString, NTUShape*>::iterator it = mDrawShapeManager.begin();
  for (; it != mDrawShapeManager.end(); it++)
  {
    if (it.key() == toolName && it.value()->getName() == shape->getName())
    {
      disconnect(it.value(), &NTUShape::newMouseCursor, this, &SMVideoZoomWidget::onReceiveCursor);
      disconnect(this, &SMVideoZoomWidget::newMouseRelease, it.value(), &NTUShape::onMouseReleaseEvent);
      disconnect(this, &SMVideoZoomWidget::newMousePressPosition, it.value(), &NTUShape::onMousePressEvent);
      disconnect(this, &SMVideoZoomWidget::newMouseMovePosition, it.value(), &NTUShape::onReceiverMousePosition);

      mDrawShapeManager.erase(it);
    }
  }
  this->drawImage();
}

void SMVideoZoomWidget::addNewNTUTool(QString toolName, NTUTools* tool)
{
  //! [1] insert to manager tool
  mNTUTools.insert(toolName, tool);
  //! [1]
  //! [2] set input image to NTUTool
  tool->onReceiverInputImage(mImage);
  //! [2]
    connect(this, &SMVideoZoomWidget::newImageInput, tool, &NTUTools::onReceiverInputImage);
    Q_EMIT newImageInput(mImage);
}

void SMVideoZoomWidget::addNewNTUToolDrawRegion(QString toolName, NTUTools* tool) { mNToolsDrawRegion.insert(toolName, tool); }

void SMVideoZoomWidget::removeNTUToolDrawRegion(QString toolName, NTUTools* tool)
{
  QMap<QString, NTUTools*>::iterator it = mNToolsDrawRegion.begin();
  for (; it != mNToolsDrawRegion.end(); it++)
  {
    if (it.key() == toolName)
      mNToolsDrawRegion.erase(it);
  }
}

void SMVideoZoomWidget::removeToolWithName(QString name)
{
  //! [1] remove draw shape
  QMap<QString, NTUShape*>::iterator it = mDrawShapeManager.find(name);
  while (it != mDrawShapeManager.end())
  {
    mDrawShapeManager.erase(it);
    it = mDrawShapeManager.find(name);
  }

  //! [1]
  //! [2]
  QMap<QString, NTUTools*>::iterator itToold = mNTUTools.find(name);
  if (itToold != mNTUTools.end())
  {
    mNTUTools.erase(itToold);
  }
  //! [2]
  //! [3]
  QMap<QString, NTUTools*>::iterator itToolDR = mNToolsDrawRegion.find(name);

  if (itToolDR != mNToolsDrawRegion.end())
    mNToolsDrawRegion.erase(itToolDR);
  //! [3]
}
