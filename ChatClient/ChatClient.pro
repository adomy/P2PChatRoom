#-------------------------------------------------
#
# Project created by QtCreator 2015-10-13T22:07:38
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chatconnection.cpp \
    chatdialog.cpp \
    chatserver.cpp \
    logindialog.cpp \
    registerconnection.cpp \
    peerinfo.cpp

HEADERS  += mainwindow.h \
    chatconnection.h \
    chatdialog.h \
    chatserver.h \
    header.h \
    logindialog.h \
    registerconnection.h \
    peerinfo.h

RC_FILE = images/icon.rc

OTHER_FILES += \
    images/icon.rc \
    images/chatroom.ico
