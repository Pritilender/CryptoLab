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
    simplesubstitutioner.cpp \
    keyvalidator.cpp \
 #   a51.cpp \
    #tea.cpp \
    cryptoqueue.cpp \
    cryptowatcher.cpp \
    cryptodispatcher.cpp \
    cryptoworker.cpp \
    cryptoconfig.cpp

HEADERS  += appgui.h \
    simplesubstitutioner.h \
    keyvalidator.h \
    cryptoalgorithm.h \
#    a51.h \
#    tea.h \
    cryptoqueue.h \
    cryptowatcher.h \
    cryptodispatcher.h \
    cryptoworker.h \
    cryptoconfig.h

FORMS    += appgui.ui
