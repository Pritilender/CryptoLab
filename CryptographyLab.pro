#-------------------------------------------------
#
# Project created by QtCreator 2016-10-22T23:46:26
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CryptographyLab
TEMPLATE = app


SOURCES += main.cpp\
        appgui.cpp \
    keyvalidator.cpp \
    cryptoqueue.cpp \
    cryptowatcher.cpp \
    cryptodispatcher.cpp \
    cryptoworker.cpp \
    cryptoconfig.cpp \
    tea.cpp

HEADERS  += appgui.h \
    keyvalidator.h \
    cryptoalgorithm.h \
    cryptoqueue.h \
    cryptowatcher.h \
    cryptodispatcher.h \
    cryptoworker.h \
    cryptoconfig.h \
    tea.h

FORMS    += appgui.ui
