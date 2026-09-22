# Qt6 form-designer wrapper (Phase 6, part A).
#
# The wrapper is built on the public Qt6 Designer API. QtDesignerComponents
# has no qmake module of its own, so it is linked explicitly (like the legacy
# Qt4 build did with -lQtDesignerComponents).
TARGET = designer-smoke
TEMPLATE = app
CONFIG += c++17 console
QT += designer

DESTDIR = ../../../bin

INCLUDEPATH += ..

LIBS += -lQt6DesignerComponents

HEADERS += aworkbench.h ../formdesigner.h
SOURCES += aworkbench.cpp ../formdesigner.cpp designer_smoke.cpp
