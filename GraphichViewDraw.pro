QT       += core gui svg testlib printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AlgorithmTools/GeoMatch.cpp \
    AlgorithmTools/MatchDescriptor.cpp \
    AlgorithmTools/NTUBlob.cpp \
    AlgorithmTools/NTUCVTools.cpp \
    AlgorithmTools/NTUCircleDetect.cpp \
    AlgorithmTools/NTUColor.cpp \
    AlgorithmTools/NTUPatMax.cpp \
    AlgorithmTools/NTUPattern.cpp \
    AlgorithmTools/NTUTools.cpp \
    Interface/NTUIspectionTools.cpp \
    Interface/NTULoadImagePC.cpp \
    Interface/NTUSaveJob.cpp \
    ItemTool/NTUBlobWidget.cpp \
    ItemTool/NTUColorWidget.cpp \
    ItemTool/NTUIndicator.cpp \
    ItemTool/NTUItemIcon.cpp \
    ItemTool/NTUItemResult.cpp \
    ItemTool/NTUItemTool.cpp \
    ItemTool/NTUItemToolName.cpp \
    ItemTool/NTUOCRWidget.cpp \
    ItemTool/NTUPatMaxWidget.cpp \
    ItemTool/NTUPatternWidget.cpp \
    ItemTool/NTUToolWidget.cpp \
    ItemTool/NTUCircleDetectWidget.cpp \
    NTUVideoWidget.cpp \
    listTools/NTUListToolItem.cpp \
    listTools/NTUSignalMultiplexer.cpp \
    plot/qcustomplot.cpp \
    projectManager/NTUToolBrowser.cpp \
    SMVideoZoomWidget.cpp \
    cvtools.cpp \
    listTools/NTUListToolBrowser.cpp \
    listTools/NTUListToolManager.cpp \
    main.cpp \
    mainwindow.cpp \
    projectManager/NTUProjectManager.cpp \
    projectManager/NTUProjectWidget.cpp \
    shapeTools/NTUCircle.cpp \
    shapeTools/NTURectangle.cpp \
    shapeTools/NTUShape.cpp

HEADERS += \
    AlgorithmTools/GeoMatch.h \
    AlgorithmTools/MatchDescriptor.h \
    AlgorithmTools/NTUBlob.h \
    AlgorithmTools/NTUCVTools.h \
    AlgorithmTools/NTUCircleDetect.h \
    AlgorithmTools/NTUColor.h \
    AlgorithmTools/NTUPatMax.h \
    AlgorithmTools/NTUPattern.h \
    AlgorithmTools/NTUTools.h \
    Interface/NTUIspectionTools.h \
    Interface/NTULoadImagePC.h \
    Interface/NTUSaveJob.h \
    ItemTool/NTUBlobWidget.h \
    ItemTool/NTUColorWidget.h \
    ItemTool/NTUIndicator.h \
    ItemTool/NTUItemIcon.h \
    ItemTool/NTUItemResult.h \
    ItemTool/NTUItemTool.h \
    ItemTool/NTUItemToolName.h \
    ItemTool/NTUOCRWidget.h \
    ItemTool/NTUPatMaxWidget.h \
    ItemTool/NTUPatternWidget.h \
    ItemTool/NTUToolWidget.h \
    ItemTool/NTUCircleDetectWidget.h \
    NTUVideoWidget.h \
    listTools/NTUListToolItem.h \
    listTools/NTUSignalMultiplexer.h \
    plot/qcustomplot.h \
    projectManager/NTUToolBrowser.h \
    SMVideoZoomWidget.h \
    cvtools.h \
    listTools/NTUListToolBrowser.h \
    listTools/NTUListToolManager.h \
    mainwindow.h \
    projectManager/NTUProjectManager.h \
    projectManager/NTUProjectWidget.h \
    shapeTools/NTUCircle.h \
    shapeTools/NTURectangle.h \
    shapeTools/NTUShape.h

FORMS += \
    Interface/NTUIspectionTools.ui \
    Interface/NTULoadImagePC.ui \
    Interface/NTUSaveJob.ui \
    ItemTool/NTUBlobWidget.ui \
    ItemTool/NTUColorWidget.ui \
    ItemTool/NTUOCRWidget.ui \
    ItemTool/NTUPatMaxWidget.ui \
    ItemTool/NTUPatternWidget.ui \
    ItemTool/NTUCircleDetectWidget.ui \
    mainwindow.ui \
    projectManager/NTUProjectWidget.ui

    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  NTUResource.qrc
