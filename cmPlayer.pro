#-------------------------------------------------
#
# Project created by QtCreator 2017-09-14T10:20:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += link_pkgconfig c++11
PKGCONFIG += opencv

unix {
    #http://www1.hikvision.com/cn/download_61.html

    pp = /home/wayne/webcam/CH_HCNetSDK_V5.2.7.4_build20170606_Linux64
    INCLUDEPATH += $$pp/incCn
    LIBS += -L$$pp/lib -L$$pp/lib/HCNetSDKCom/
    LIBS += -lhpr -lHCCore -lhcnetsdk -lPlayCtrl -lAudioRender -lSuperRender
}

TARGET = cmPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    hkplayer.cpp \
    deviceconfig.cpp \
    imageitem.cpp \
    cmview.cpp

HEADERS += \
        mainwindow.h \
    hkplayer.h \
    common.h \
    deviceconfig.h \
    imageitem.h \
    cmview.h

FORMS += \
        mainwindow.ui \
    deviceconfig.ui \
    hkplayer.ui

RESOURCES += \
    res.qrc
