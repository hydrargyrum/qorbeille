QT += core testlib
QT -= gui widgets

CONFIG += qtestlib
TARGET = test_qorbeille

#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app


INCLUDEPATH = include

SOURCES += src/xdg/dirs.cpp \
    src/xdg/trash.cpp \
    src/xdg/trashinfo.cpp \
    tests/main_test.cpp \
    tests/tst_dirs.cpp \

HEADERS += \
    include/xdg/trash.h \
    include/xdg/trashinfo.h \
    include/xdg/dirs.h \
    tests/tst_dirs.h \
    tests/tst_trash.h
