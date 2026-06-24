#-------------------------------------------------
#
# Project created by QtCreator 2025-12-06T00:32:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cliente
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        crearcuenta.cpp \
        dashboard.cpp \
        principal.cpp \
        select.cpp

HEADERS  += principal.h \
    crearcuenta.h \
    dashboard.h \
    select.h

FORMS    += principal.ui \
    crearcuenta.ui \
    dashboard.ui

RESOURCES += \
    resources.qrc
