#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T09:35:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DTNOTronAdminGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chttpcommunicator.cpp \
    creportdelegate.cpp \
    cjsonarraytablemodel.cpp \
    cvariantmapdelegate.cpp \
    ceditablevariantlistdelegate.cpp \
    cnodetablemodel.cpp \
    creporttablemodel.cpp

HEADERS  += mainwindow.h \
    chttpcommunicator.h \
    creportdelegate.h \
    cjsonarraytablemodel.h \
    cjsonobjecttreemodel.h \
    cvariantmapdelegate.h \
    ceditablevariantlistdelegate.h \
    cnodetablemodel.h \
    creporttablemodel.h

FORMS    += mainwindow.ui
