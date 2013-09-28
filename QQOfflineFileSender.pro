#-------------------------------------------------
#
# Project created by QtCreator 2013-09-12T21:26:08
#
#-------------------------------------------------
TEMPLATE = app
CONFIG -= debug

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QQOfflineFileSender

QMAKE_CXXFLAGS = -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -Os -DBOOST_THREAD_USE_LIB -DBOOST_FILESYSTEM_STATIC_LINK -DBOOST_ALL_STATIC_LINK -DWIN32_LEAN_AND_MEAN

INCLUDEPATH = build/ c:/mingw/include avhttp/include  avboost/include



SOURCES += main.cpp\
        dialog.cpp \
        askvc.cpp \
        buddymodel.cpp

HEADERS  += dialog.hpp \
            askvc.hpp \
            buddymodel.hpp


FORMS    += dialog.ui \
    askvc.ui

LIBS +=  -static-libstdc++ -static-libgcc -lws2_32 -lboost_system -lboost_regex -lboost_thread -lwebqq -L libwebqq
