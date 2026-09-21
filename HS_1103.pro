#-------------------------------------------------
#
# Project created by QtCreator 2023-12-23T11:57:04
# 全自动加氢仪
#-------------------------------------------------

QT       += core gui serialport opengl widgets  printsupport network

TARGET = HS_1103
TEMPLATE = app
RC_ICONS = logo.ico
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL

INCLUDEPATH += $$PWD
  

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS+=-lopengl32

SOURCES += $$PWD/common/ActionFactory.cpp \
    $$PWD/common/ActionItem.cpp \
	$$PWD/common/DlgSerialSettings.cpp \
	$$PWD/common/DlgSocketSettings.cpp \
	$$PWD/common/MaskWidget.cpp \
	$$PWD/common/ModubosProtocol.cpp \
	$$PWD/common/mymessagebox.cpp \
    $$PWD/collectorOp/collectorop.cpp \
    $$PWD/customGraph/customgraph.cpp \
    $$PWD/customGraph/debuggraph.cpp \
    $$PWD/customGraph/qcustomplot.cpp \
    $$PWD/customGraph/customTool/graphopen.cpp \
    $$PWD/customGraph/customTool/graphsave.cpp \
    $$PWD/customGraph/customTool/graphexport.cpp \
    $$PWD/deviceOp/deviceop.cpp \
    $$PWD/Diagram/diagram.cpp \
    $$PWD/Diagram/flowset.cpp \
    $$PWD/log/DeviceLog.cpp \
    $$PWD/manualOp/manualop.cpp \
    $$PWD/materialFeeder/FeederActionItem.cpp \
    $$PWD/materialFeeder/FeederMgr.cpp \
    $$PWD/materialFeeder/FeederStruct.cpp \
    $$PWD/materialFeeder/MaterialStore.cpp \
    $$PWD/materialFeeder/RobotMgr.cpp \
    $$PWD/materialFeeder/subMateUi/DemoWidget.cpp \
    $$PWD/materialFeeder/subMateUi/DlgFeedMaterial.cpp \
    $$PWD/materialFeeder/subMateUi/DlgMaterialModify.cpp \
    $$PWD/materialFeeder/subMateUi/DlgTubeBack.cpp \
    $$PWD/materialFeeder/subMateUi/DlgTubeFix.cpp \
    $$PWD/materialFeeder/subMateUi/MaterialsSelect.cpp \
    $$PWD/materialFeeder/subMateUi/MaterialTableWidget.cpp \
    $$PWD/materialFeeder/subMateUi/SpinCust.cpp \
    $$PWD/programConfig/hslist.cpp \
    $$PWD/programConfig/ProgmaMgr.cpp \
    $$PWD/programConfig/programlist.cpp \
    $$PWD/programConfig/progconfig.cpp \
    $$PWD/programConfig/customTool/ToolBox.cpp \
    $$PWD/programConfig/customTool/ToolPage.cpp \
    $$PWD/programConfig/customTool/objlist.cpp \
    $$PWD/programConfig/customTool/fileexport.cpp \
    $$PWD/programConfig/customTool/filesave.cpp \
    $$PWD/programConfig/customTool/fileopen.cpp \
    $$PWD/programConfig/progItem/AirWayGroupBox.cpp \
    $$PWD/programConfig/progItem/qcirculation.cpp \
    $$PWD/programConfig/progItem/qflowpanel.cpp \
    $$PWD/programConfig/progItem/qponittemp.cpp \
    $$PWD/programConfig/progItem/qslopetemp.cpp \
    $$PWD/programConfig/progItem/qtimedelay.cpp \
    $$PWD/programConfig/progItem/swpanel.cpp \
    $$PWD/programConfig/progItem/qtcdpanel.cpp \
    $$PWD/programConfig/progItem/QSFunction.cpp \
    $$PWD/programConfig/progItem/FeederGroupBox.cpp \
    $$PWD/programConfig/progItem/HeatGroupBox.cpp \
    $$PWD/programConfig/progItem/MotorGroupBox.cpp \
    $$PWD/programConfig/progItem/FeedLiquidGroupBox.cpp \
    $$PWD/programConfig/progItem/qpumpctrl.cpp \
    $$PWD/programConfig/progItem/qvalvectrl.cpp \
    $$PWD/programConfig/progItem/qcollectorctrl.cpp \
    $$PWD/programConfig/progItem/qfunlabel.cpp \
    $$PWD/projectMode/projectmode.cpp \
    $$PWD/stateBar/statebar.cpp \
    $$PWD/stove/stove.cpp \
    $$PWD/stove/tabeledit.cpp \
    $$PWD/stove/stovetable.cpp \
    $$PWD/DevContrlMgr/portthread.cpp \
    $$PWD/DevContrlMgr/strdecoder.cpp \
    $$PWD/toolTitle/tooltitle.cpp \
    $$PWD/toolTitle/hslogo.cpp \
    $$PWD/desktop.cpp \
    $$PWD/main.cpp

HEADERS += $$PWD/common/ActionFactory.h \
    $$PWD/common/ActionItem.h \
    $$PWD/common/DlgSerialSettings.h \
    $$PWD/common/DlgSocketSettings.h \
    $$PWD/common/MaskWidget.h \
    $$PWD/common/ModubosProtocol.h \
    $$PWD/common/mymessagebox.h \
    $$PWD/collectorOp/collectorop.h \
    $$PWD/customGraph/qcustomplot.h \
    $$PWD/customGraph/customgraph.h \
    $$PWD/customGraph/debuggraph.h \
    $$PWD/customGraph/customTool/graphopen.h \
    $$PWD/customGraph/customTool/graphsave.h \
    $$PWD/customGraph/customTool/graphexport.h \
    $$PWD/deviceOp/deviceop.h \
    $$PWD/Diagram/diagram.h \
    $$PWD/Diagram/flowset.h \
    $$PWD/log/DeviceLog.h \	
    $$PWD/manualOp/manualop.h \
    $$PWD/materialFeeder/FeederActionItem.h \
    $$PWD/materialFeeder/FeederMgr.h \
    $$PWD/materialFeeder/FeederStruct.h \
    $$PWD/materialFeeder/MaterialStore.h \
    $$PWD/materialFeeder/RobotMgr.h \
    $$PWD/materialFeeder/subMateUi/DemoWidget.h \
    $$PWD/materialFeeder/subMateUi/DlgFeedMaterial.h \
    $$PWD/materialFeeder/subMateUi/DlgMaterialModify.h \
    $$PWD/materialFeeder/subMateUi/DlgTubeBack.h \
    $$PWD/materialFeeder/subMateUi/DlgTubeFix.h \
    $$PWD/materialFeeder/subMateUi/MaterialsSelect.h \
    $$PWD/materialFeeder/subMateUi/MaterialTableWidget.h \
    $$PWD/materialFeeder/subMateUi/SpinCust.h \
    $$PWD/programConfig/hslist.h \
    $$PWD/programConfig/ProgmaMgr.h \
    $$PWD/programConfig/programlist.h \
    $$PWD/programConfig/progconfig.h \
    $$PWD/programConfig/progItem/AirWayGroupBox.h \
    $$PWD/programConfig/customTool/ToolBox.h \
    $$PWD/programConfig/customTool/ToolPage.h \
    $$PWD/programConfig/customTool/objlist.h \
    $$PWD/programConfig/customTool/fileexport.h \
    $$PWD/programConfig/customTool/filesave.h \
    $$PWD/programConfig/customTool/fileopen.h \
    $$PWD/programConfig/progItem/qcirculation.h \
    $$PWD/programConfig/progItem/qflowpanel.h \
    $$PWD/programConfig/progItem/qponittemp.h \
    $$PWD/programConfig/progItem/qslopetemp.h \
    $$PWD/programConfig/progItem/qtimedelay.h \
    $$PWD/programConfig/progItem/swpanel.h \
    $$PWD/programConfig/progItem/qtcdpanel.h \
    $$PWD/programConfig/progItem/qfunlabel.h \
    $$PWD/programConfig/progItem/QSFunction.h \
    $$PWD/programConfig/progItem/qpumpctrl.h \
    $$PWD/programConfig/progItem/FeederGroupBox.h \
    $$PWD/programConfig/progItem/HeatGroupBox.h \
    $$PWD/programConfig/progItem/MotorGroupBox.h \
    $$PWD/programConfig/progItem/FeedLiquidGroupBox.h \
    $$PWD/programConfig/progItem/qvalvectrl.h \
    $$PWD/programConfig/progItem/qcollectorctrl.h \
    $$PWD/projectMode/projectmode.h \
    $$PWD/stateBar/statebar.h \
    $$PWD/stove/stove.h \
    $$PWD/stove/stovetable.h \
    $$PWD/stove/tabeledit.h \
    $$PWD/DevContrlMgr/portthread.h \
    $$PWD/DevContrlMgr/strdecoder.h \
    $$PWD/toolTitle/tooltitle.h \
    $$PWD/toolTitle/hslogo.h \
    $$PWD/desktop.h

RESOURCES += \
    $$PWD/style.qrc \
    $$PWD/image.qrc \
    $$PWD/programConfig/customTool/img.qrc \
    $$PWD/programConfig/customTool/qss.qrc

FORMS += \
    $$PWD/common/DlgSerialSettings.ui \
    $$PWD/common/DlgSocketSettings.ui \
    $$PWD/customGraph/customgraph.ui \
    $$PWD/Diagram/diagram.ui \
    $$PWD/manualOp/manualop.ui \
    $$PWD/stove/stove.ui \
    $$PWD/desktop.ui \
    $$PWD/toolTitle/tooltitle.ui \
    $$PWD/stateBar/statebar.ui \
    $$PWD/customGraph/debuggraph.ui \
    $$PWD/deviceOp/deviceop.ui \
    $$PWD/programConfig/progconfig.ui \
    $$PWD/programConfig/customTool/Form.ui \
    $$PWD/programConfig/customTool/ToolBox.ui \
    $$PWD/programConfig/customTool/ToolPage.ui \
    $$PWD/programConfig/customTool/fileexport.ui \
    $$PWD/programConfig/customTool/filesave.ui \
    $$PWD/programConfig/customTool/fileopen.ui \
    $$PWD/programConfig/progItem/AirWayGroupBox.ui \
    $$PWD/programConfig/progItem/FeederGroupBox.ui \
    $$PWD/programConfig/progItem/HeatGroupBox.ui \
    $$PWD/programConfig/progItem/MotorGroupBox.ui \
    $$PWD/programConfig/progItem/FeedLiquidGroupBox.ui \
    $$PWD/programConfig/progItem/QSFunction.ui \
    $$PWD/customGraph/customTool/graphopen.ui \
    $$PWD/customGraph/customTool/graphsave.ui \
    $$PWD/customGraph/customTool/graphexport.ui \
    $$PWD/Diagram/flowset.ui \
    $$PWD/projectMode/projectmode.ui \
    $$PWD/collectorOp/collectorop.ui \
    $$PWD/materialFeeder/MaterialStore.ui \
    $$PWD/materialFeeder/subMateUi/DlgFeedMaterial.ui \
    $$PWD/materialFeeder/subMateUi/DlgMaterialModify.ui \
    $$PWD/materialFeeder/subMateUi/DlgTubeBack.ui \
    $$PWD/materialFeeder/subMateUi/DlgTubeFix.ui \
    $$PWD/materialFeeder/subMateUi/MaterialsSelect.ui \
    $$PWD/materialFeeder/subMateUi/SpinCust.ui
