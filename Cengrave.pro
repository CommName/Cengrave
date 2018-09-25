#-------------------------------------------------
#
# Project created by QtCreator 2018-08-31T17:05:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cengrave
TEMPLATE = app
include(C:/Qt/3rdparty/qextserialport-master/src/qextserialport.pri)

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
    mainwindow/main.cpp \
    mainwindow/mainwindow.cpp \
    Image2Machine/graphimage.cpp \
    Image2Machine/commandcontainer.cpp \
    mainwindow/settings.cpp \
    Image2Machine/hwf.cpp \
    Image2Machine/tmcl.cpp
    #Image2Machine/hwf.cpp

HEADERS += \
    mainwindow/mainwindow.h \
    Image2Machine/graphimage.h \
    Image2Machine/commandcontainer.h \
    mainwindow/settings.h \
    Image2Machine/hwf.h \
    Image2Machine/tmcl.h
    #Image2Machine/hwf.h

FORMS += \
        mainwindow/mainwindow.ui \
    mainwindow/settings.ui

INCLUDEPATH += $(OPENCV3_DIR)\install\include

LIBS += $(OPENCV3_DIR)\bin\libopencv_core342.dll
LIBS += $(OPENCV3_DIR)\bin\libopencv_highgui342.dll
LIBS += $(OPENCV3_DIR)\bin\libopencv_imgcodecs342.dll
LIBS += $(OPENCV3_DIR)\bin\libopencv_imgproc342.dll
LIBS += $(OPENCV3_DIR)\bin\libopencv_features2d342.dll
LIBS += $(OPENCV3_DIR)\bin\libopencv_calib3d342.dll
