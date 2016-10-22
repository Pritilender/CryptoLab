#-------------------------------------------------
#
# Project created by QtCreator 2016-10-22T23:46:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CryptographyLab
TEMPLATE = app


SOURCES += main.cpp\
        appgui.cpp \
    simplesubstitutioner.cpp \
    keyvalidator.cpp

HEADERS  += appgui.h \
    simplesubstitutioner.h \
    keyvalidator.h

FORMS    += appgui.ui
