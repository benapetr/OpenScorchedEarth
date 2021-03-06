#-------------------------------------------------
#
# Project created by QtCreator 2019-05-24T08:40:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scorche_qmake
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

CONFIG += c++11
CONFIG += object_parallel_to_source

INCLUDEPATH += ../../src/PixelEngine/src/

SOURCES += \
        ../../src/PixelEngine/src/PixelEngine/*.cpp \
        ../../src/PixelEngine/src/PixelEngine/GC/*.cpp \
        ../../src/PixelEngine/src/PixelEngine/Physics/*.cpp \
        ../../src/PixelEngine/src/PixelEngine/Graphics/*.cpp \
        ../../src/Scorche/bots/*.cpp \
        ../../src/Scorche/weapons/*.cpp \
        ../../src/Scorche/projectiles/*.cpp \
        ../../src/Scorche/tanks/*.cpp \
        ../../src/Scorche/scenes/*.cpp \
        ../../src/Scorche/effects/*.cpp \
        ../../src/Scorche/*.cpp

HEADERS += \
        ../../src/PixelEngine/src/PixelEngine/*.h \
        ../../src/PixelEngine/src/PixelEngine/GC/*.h \
        ../../src/PixelEngine/src/PixelEngine/Physics/*.h \
        ../../src/PixelEngine/src/PixelEngine/Graphics/*.h \
        ../../src/Scorche/bots/*.cpp \
        ../../src/Scorche/scenes/*.h \
        ../../src/Scorche/projectiles/*.h \
        ../../src/Scorche/tanks/*.h \
        ../../src/Scorche/weapons/*.h \
        ../../src/Scorche/effects/*.h \
        ../../src/Scorche/*.h

RESOURCES += ../../src/Scorche/*.qrc

FORMS += \
      ../../src/Scorche/*.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
