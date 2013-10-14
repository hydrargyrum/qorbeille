QT += core
QT -= gui widgets

# CONFIG += 
TARGET = qorbeille

#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = lib

VERSION = 0.1.0


INCLUDEPATH = include

SOURCES += src/xdg/dirs.cpp \
    src/xdg/trash.cpp \
    src/xdg/trashinfo.cpp

HEADERS += \
    include/xdg/trash.h \
    include/xdg/trashinfo.h \
    include/xdg/dirs.h

