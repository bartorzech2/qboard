include(../../config.qmake)
TEMPLATE = lib
CONFIG += plugin script
RESOURCES = counters.qrc

# Kludge to work around missing QtScript includes (qmake bug, i think)
INCLUDEPATH -= $$QMAKE_INCDIR_QT/QtScript
INCLUDEPATH = $$QMAKE_INCDIR_QT/QtScript $$INCLUDEPATH

# TARGET = CGMEJoe
# DLLDESTDIR = $$DESTDIR
DESTDIR = $$QBOARD_PLUGINS_DESTDIR
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
TARGET = $$qtLibraryTarget(CGMEJoe)
HEADERS = \
  CGMEJoe.h

SOURCES = \
  CGMEJoe.cpp

# OBJECTS += $$S11N_OBJECTS
