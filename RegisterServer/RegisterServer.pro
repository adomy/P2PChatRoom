#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T10:36:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RegisterServer
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    registerconnection.cpp \
    registerserver.cpp \
    peerinfo.cpp

HEADERS  += window.h \
    registerconnection.h \
    header.h \
    registerserver.h \
    peerinfo.h

RC_FILE += images/icon.rc

OTHER_FILES += \
    images/icon.rc \
    images/server.ico


