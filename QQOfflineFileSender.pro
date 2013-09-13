#-------------------------------------------------
#
# Project created by QtCreator 2013-09-12T21:26:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QQOfflineFileSender
TEMPLATE = app

INCLUDEPATH = build/

SOURCES += main.cpp\
        dialog.cpp \
    askvc.cpp

HEADERS  += dialog.hpp \
    askvc.hpp

FORMS    += dialog.ui \
    askvc.ui
