#-------------------------------------------------
#
# Project created by QtCreator 2017-12-11T11:33:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mycelium
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    new.cpp \
    old.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -lopencv_nonfree
