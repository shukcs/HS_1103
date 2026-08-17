#-------------------------------------------------
#
# Project created by QtCreator 2023-12-23T11:57:04
# 全自动加氢仪
#-------------------------------------------------

QT       += core gui serialport opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  printsupport

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

SOURCES += $$PWD/common/DlgSerialSettings.cpp \
	$$PWD/common/MaskWidget.cpp \
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
    $$PWD/manualOp/manualop.cpp \
	$$PWD/materialFeeder/FeederDecoder.cpp \
	$$PWD/materialFeeder/MaterialStore.cpp \
	$$PWD/materialFeeder/RobotMgr.cpp \
	$$PWD/materialFeeder/subMateUi/DemoWidget.cpp \
	$$PWD/materialFeeder/subMateUi/DlgFeedMaterial.cpp \
	$$PWD/materialFeeder/subMateUi/DlgMaterialModify.cpp \
	$$PWD/materialFeeder/subMateUi/DlgTubeBack.cpp \
	$$PWD/materialFeeder/subMateUi/MaterialsSelect.cpp \
	$$PWD/materialFeeder/subMateUi/MaterialTableWidget.cpp \
	$$PWD/materialFeeder/subMateUi/SpinCust.cpp \
    $$PWD/programConfig/hslist.cpp \
    $$PWD/programConfig/programlist.cpp \
    $$PWD/programConfig/qcirculation.cpp \
    $$PWD/programConfig/qflowpanel.cpp \
    $$PWD/programConfig/qponittemp.cpp \
    $$PWD/programConfig/qslopetemp.cpp \
    $$PWD/programConfig/qtimedelay.cpp \
    $$PWD/programConfig/swpanel.cpp \
    $$PWD/programConfig/qtcdpanel.cpp \
    $$PWD/programConfig/qsfunction.cpp \
	$$PWD/programConfig/FeederGroupBox.cpp \
	$$PWD/programConfig/HeatGroupBox.cpp \
	$$PWD/programConfig/MotorGroupBox.cpp \
	$$PWD/programConfig/Valve3ChGroupBox.cpp \
    $$PWD/programConfig/qpumpctrl.cpp \
    $$PWD/programConfig/progconfig.cpp \
    $$PWD/programConfig/qvalvectrl.cpp \
    $$PWD/programConfig/qcollectorctrl.cpp \
    $$PWD/programConfig/customTool/ToolBox.cpp \
    $$PWD/programConfig/customTool/ToolPage.cpp \
    $$PWD/programConfig/customTool/objlist.cpp \
    $$PWD/programConfig/customTool/fileexport.cpp \
    $$PWD/programConfig/customTool/filesave.cpp \
    $$PWD/programConfig/customTool/fileopen.cpp \
    $$PWD/programConfig/qfunlabel.cpp \
    $$PWD/projectMode/projectmode.cpp \
    $$PWD/stateBar/statebar.cpp \
    $$PWD/stove/stove.cpp \
    $$PWD/stove/tabeledit.cpp \
    $$PWD/stove/stovetable.cpp \
    $$PWD/strDecoder/portthread.cpp \
    $$PWD/strDecoder/strdecoder.cpp \
    $$PWD/toolTitle/tooltitle.cpp \
    $$PWD/toolTitle/hslogo.cpp \
    $$PWD/desktop.cpp \
    $$PWD/main.cpp

HEADERS += $$PWD/common/DlgSerialSettings.h \
    $$PWD/common/MaskWidget.h \
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
    $$PWD/manualOp/manualop.h \
	$$PWD/materialFeeder/FeederDecoder.h \
	$$PWD/materialFeeder/MaterialStore.h \
	$$PWD/materialFeeder/RobotMgr.h \
	$$PWD/materialFeeder/subMateUi/DemoWidget.h \
	$$PWD/materialFeeder/subMateUi/DlgFeedMaterial.h \
	$$PWD/materialFeeder/subMateUi/DlgMaterialModify.h \
	$$PWD/materialFeeder/subMateUi/DlgTubeBack.h \
	$$PWD/materialFeeder/subMateUi/MaterialsSelect.h \
	$$PWD/materialFeeder/subMateUi/MaterialTableWidget.h \
	$$PWD/materialFeeder/subMateUi/SpinCust.h \
    $$PWD/programConfig/hslist.h \
    $$PWD/programConfig/programlist.h \
    $$PWD/programConfig/qcirculation.h \
    $$PWD/programConfig/qflowpanel.h \
    $$PWD/programConfig/qponittemp.h \
    $$PWD/programConfig/qslopetemp.h \
    $$PWD/programConfig/qtimedelay.h \
    $$PWD/programConfig/swpanel.h \
    $$PWD/programConfig/qtcdpanel.h \
    $$PWD/programConfig/qfunlabel.h \
    $$PWD/programConfig/qsfunction.h \
    $$PWD/programConfig/progconfig.h \
    $$PWD/programConfig/qpumpctrl.h \
	$$PWD/programConfig/FeederGroupBox.h \
	$$PWD/programConfig/HeatGroupBox.h \
	$$PWD/programConfig/MotorGroupBox.h \
	$$PWD/programConfig/Valve3ChGroupBox.h \
    $$PWD/programConfig/qvalvectrl.h \
    $$PWD/programConfig/qcollectorctrl.h \
    $$PWD/programConfig/customTool/ToolBox.h \
    $$PWD/programConfig/customTool/ToolPage.h \
    $$PWD/programConfig/customTool/objlist.h \
    $$PWD/programConfig/customTool/fileexport.h \
    $$PWD/programConfig/customTool/filesave.h \
    $$PWD/programConfig/customTool/fileopen.h \
    $$PWD/projectMode/projectmode.h \
    $$PWD/stateBar/statebar.h \
    $$PWD/stove/stove.h \
    $$PWD/stove/stovetable.h \
    $$PWD/stove/tabeledit.h \
    $$PWD/strDecoder/portthread.h \
    $$PWD/strDecoder/strdecoder.h \
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
    $$PWD/customGraph/customgraph.ui \
    $$PWD/Diagram/diagram.ui \
    $$PWD/manualOp/manualop.ui \
    $$PWD/stove/stove.ui \
    $$PWD/desktop.ui \
    $$PWD/toolTitle/tooltitle.ui \
    $$PWD/stateBar/statebar.ui \
    $$PWD/customGraph/debuggraph.ui \
    $$PWD/deviceOp/deviceop.ui \
    $$PWD/programConfig/customTool/Form.ui \
    $$PWD/programConfig/customTool/ToolBox.ui \
    $$PWD/programConfig/customTool/ToolPage.ui \
    $$PWD/programConfig/progconfig.ui \
	$$PWD/programConfig/FeederGroupBox.ui \
	$$PWD/programConfig/HeatGroupBox.ui \
	$$PWD/programConfig/MotorGroupBox.ui \
	$$PWD/programConfig/Valve3ChGroupBox.ui \
    $$PWD/programConfig/customTool/fileexport.ui \
    $$PWD/programConfig/customTool/filesave.ui \
    $$PWD/programConfig/customTool/fileopen.ui \
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
	$$PWD/materialFeeder/subMateUi/MaterialsSelect.ui \
	$$PWD/materialFeeder/subMateUi/SpinCust.ui
