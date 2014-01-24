#*************************************************************************
# NegativeViewer                                                         *
# It is a tool for viewing photographic negatives and slides by using    *
# a webcam or digital single lens reflex camera.                         *
# You can apply various transformations on the live image like inverting *
# and rotating a picture, croping the frame and saving the result.       *
#                                                                        *
# Copyright (c) 2013 Johannes Biermann code(at)johannesb.eu              *
# ---------------------------------------------------------------------- *

#-------------------------------------------------
#
# Project created by QtCreator 2013-01-18T20:59:20
#
#-------------------------------------------------

QT       += core gui

TARGET = QTNegativeViewer
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    MatToQImage.cpp \
    videoprocessing.cpp \
    dslr.cpp \
    webcam.cpp

HEADERS  += dialog.h \
    MatToQImage.h \
    videoprocessing.h \
    dslr.h \
    webcam.h

FORMS    += dialog.ui

INCLUDEPATH += /usr/local/include/opencv \
                /usr/include

LIBS += -L/usr/local/lib \
    -L/usr/lib \
    -lopencv_calib3d \
    -lopencv_contrib \
    -lopencv_core \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_gpu \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_legacy \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_photo \
    -lopencv_stitching \
    -lopencv_ts \
    -lopencv_video \
    -lopencv_videostab \
    -lgphoto2 \
    -lm \
    -lgphoto2_port \
    -ldl

RESOURCES += \
    qtressource.qrc

OTHER_FILES +=


