TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../dlib-19.24/include/dlib/all/source.cpp \
        audioplayer.cpp \
        camera.cpp \
        entrancemanager.cpp \
        exitmanager.cpp \
        facehasher.cpp \
        main.cpp \
        simplefacedetector.cpp
INCLUDEPATH += /usr/include/opencv4

INCLUDEPATH += /home/bigpi/Documents/cplusplus_version/dlib-19.24/include
INCLUDEPATH += -I/usr/local/include
LIBS += -L/usr/local/lib -lfmod
LIBS += -pthread

#QT_CONFIG -= no-pkg-config

CONFIG += link_pkgconfig
PKGCONFIG += opencv4
PKGCONFIG += x11

HEADERS += \
    audioplayer.h \
    camera.h \
    entrancemanager.h \
    exitmanager.h \
    facehasher.h \
    simplefacedetector.h

