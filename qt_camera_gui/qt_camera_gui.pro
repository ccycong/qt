#-------------------------------------------------
#
# Project created by QtCreator 2019-07-13T05:40:21
#
#-------------------------------------------------

QT       += core gui
QT       += core gui network
QT       += network
TARGET = qt_camera_gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qthreadmessgge.cpp \
    qthreadfile.cpp \
    qthreadcamera.cpp

HEADERS  += mainwindow.h \
    qthreadmessgge.h \
    qthreadfile.h \
    qthreadcamera.h

FORMS    += mainwindow.ui
