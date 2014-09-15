TEMPLATE = lib
CONFIG += staticlib
TARGET = scenegraph
DESTDIR = $$OUT_PWD/../lib
DEFINES += QT_BUILD_SCENEGRAPH_LIB
HEADERS += scenegraph.h rasterizer.h
SOURCES += scenegraph.cpp rasterizer.cpp
QT += opengl
