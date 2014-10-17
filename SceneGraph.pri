#INCLUDEPATH += $$PWD/src
#LIBRARYPATH += $$PWD/lib
#QMAKE_RPATHDIR = $$OUT_PWD/lib

#QT += opengl

#QMAKE_INCDIR += $$INCLUDEPATH
#QMAKE_LIBDIR += $$LIBRARYPATH
#LIBS += -L$$QMAKE_LIBDIR -L. -lscenegraph

#win32:DEFINES += Q_SCENEGRAPH_EXPORT=__declspec(dllimport)
#unix:DEFINES += Q_SCENEGRAPH_EXPORT=""

#include (./SceneGraph.pri)
#CONFIG -= app_bundle


# Just include the sources

QT += opengl
INCLUDEPATH += ../SceneGraph/src
HEADERS += ../SceneGraph/src/scenegraph.h ../SceneGraph/src/rasterizer.h
SOURCES += ../SceneGraph/src/scenegraph.cpp ../SceneGraph/src/rasterizer.cpp
