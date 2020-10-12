#-------------------------------------------------
#
# Project created by QtCreator 2020-09-14T13:32:48
#
#-------------------------------------------------


#DEFINES += BEVONE
DEFINES += SENSOR


DEFINES += FULLSCREEN

QT       += core gui svg sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



VERSION = 1.0.0
RC_ICONS = Image/app.ico
QMAKE_TARGET_COMPANY = "CHEN"
QMAKE_TARGET_PRODUCT = "GDS"
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.10.0"
QMAKE_TARGET_COPYRIGHT = "Copyright 2018-2020 The Qt Company Ltd. All rights reserved."
RC_LANG = 0x0004


TARGET = SS_GDS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    RecordInfo/recordinfo.cpp \
    SystemConf/systemconf.cpp \
    ViewWidget/viewwidget.cpp \
    ViewWidget/sensoritem.cpp \
    ViewWidget/graphicsview.cpp \
    SqlManager/sqlmanager.cpp \
    MsgBox/infobox.cpp \
    MsgBox/msgbox.cpp \
    MsgBox/questbox.cpp \
    ViewWidget/udpthread.cpp \
    ViewWidget/sensoriteminfo.cpp \
    ViewWidget/dbthead.cpp \
    UserLgoin/userlogin.cpp

HEADERS += \
        mainwindow.h \
    RecordInfo/recordinfo.h \
    SystemConf/systemconf.h \
    ViewWidget/viewwidget.h \
    ViewWidget/sensoritem.h \
    ViewWidget/graphicsview.h \
    SqlManager/sqlmanager.h \
    MsgBox/infobox.h \
    MsgBox/msgbox.h \
    MsgBox/questbox.h \
    ViewWidget/udpthread.h \
    ViewWidget/sensoriteminfo.h \
    ViewWidget/dbthead.h \
    UserLgoin/userlogin.h \
    includes.h

FORMS += \
        mainwindow.ui \
    RecordInfo/recordinfo.ui \
    SystemConf/systemconf.ui \
    ViewWidget/viewwidget.ui \
    ViewWidget/graphicsview.ui \
    MsgBox/infobox.ui \
    MsgBox/questbox.ui \
    UserLgoin/userlogin.ui

RESOURCES += \
    image.qrc

