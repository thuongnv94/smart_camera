#ifndef NTUTOOLWIDGET_H
#define NTUTOOLWIDGET_H
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QObject>
#include <QWidget>

#include "AlgorithmTools/NTUTools.h"
#include "NTUIndicator.h"
#include "NTUItemIcon.h"
#include "NTUItemResult.h"
#include "NTUItemTool.h"
#include "NTUItemToolName.h"

class SMVideoZoomWidget;
class NTUToolWidget : public QGroupBox
{
  Q_OBJECT
public:
  explicit NTUToolWidget(QWidget* parent = nullptr, QWidget* video = nullptr);
  virtual ~NTUToolWidget();

  inline QString getName() { return mName; }

  ///
  /// \brief getToolType
  /// \return
  ///
  NTUTOOLS getToolType() { return mToolType; }

  ///
  /// \brief setNTUItemTool
  /// \param itemTool
  ///
  void setNTUItemTool(NTUItemTool* itemTool);

  ///
  /// \brief getListStandarItems
  /// \return
  ///
  QList<QStandardItem*> getListStandarItems(NTUTOOLS type);

  ///
  /// \brief getItemTool
  /// \return
  ///
  NTUItemTool* getItemTool();

  ///
  /// \brief newExcureToolAlgorithm
  ///
  void excureToolAlgorithm();

  ///
  /// \brief showEvent
  /// \param event
  ///
  virtual void showEvent(QShowEvent* event) override;

  ///
  /// \brief hideEvent
  /// \param event
  ///
  virtual void hideEvent(QHideEvent* event) override;

  ///
  /// \brief insertToolFixture
  /// \param tool
  ///
  virtual void insertToolFixture(QString toolName) = 0;

  ///
  /// \brief getJsonArray
  /// \return
  ///
  virtual QJsonObject* getJsonObject() = 0;

  ///
  /// \brief updateSettingFromJsonObject
  /// \param jsonObject
  ///
  virtual void updateSettingFromJsonObject(QJsonObject jsonObject) = 0;

  ///
  /// \brief mToolAlgorithm
  ///
  NTUTools* mToolAlgorithm;

  ///
  /// \brief setupCombobox
  /// \param ccbox
  ///
  void setupCombobox(QComboBox* ccbox);

Q_SIGNALS:
  void newWidgetVisibilityChanged(bool avaiable);
  void newJSonSetting(QJsonObject* setting);
  void newExureToolAlgorithm();
  void newFixtureTool(NTUTools* myTool, QString fixtureName);
public Q_SLOTS:

  void onUpdateName(QString name);

  virtual void onReceiverVisibilityShape(bool value) = 0;
  virtual void onReceiverSettingChanged() = 0;
  virtual void onReceiverResult(QString result) = 0;

protected:
  NTUTOOLS mToolType;
  QString mName;
  SMVideoZoomWidget* mVideoWidget;
  NTUItemTool* mNTUItemMyselft;
  NTUIndicator* mIndicatorItem;
  NTUItemIcon* mIconItem;
  NTUItemResult* mResultItem;
  NTUItemToolName* mToolNameItem;
  QJsonObject* mJsonObject;
};
// Q_DECLARE_METATYPE(NTUToolWidget);

#endif // NTUTOOLWIDGET_H
