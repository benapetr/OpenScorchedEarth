#-------------------------------------------------
#
# Project created by QtCreator 2019-05-09T21:50:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scorche
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

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    world.cpp \
    renderer.cpp \
    tankbase.cpp \
    worldgenerator.cpp \
    qimagerenderer.cpp \
    game.cpp \
    vector.cpp \
    object.cpp \
    terrain.cpp \
    actor.cpp \
    pawn.cpp \
    demotank.cpp \
    collider.cpp \
    rigidbody.cpp \
    bitmapcollider.cpp \
    ringlog.cpp \
    engine.cpp \
    exception.cpp \
    nullpointerexception.cpp

HEADERS += \
        mainwindow.h \
    world.h \
    renderer.h \
    tankbase.h \
    worldgenerator.h \
    qimagerenderer.h \
    game.h \
    vector.h \
    object.h \
    terrain.h \
    actor.h \
    pawn.h \
    demotank.h \
    collider.h \
    rigidbody.h \
    bitmapcollider.h \
    ringlog.h \
    engine.h \
    exception.h \
    nullpointerexception.h \
    definitions.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
