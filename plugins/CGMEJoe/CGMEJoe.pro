include(../../config.qmake)
TEMPLATE = lib
CONFIG += plugin
RESOURCES = counters.qrc
# TARGET = CGMEJoe
# DLLDESTDIR = $$DESTDIR
# QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
TARGET = $$qtLibraryTarget(CGMEJoe)

HEADERS = \
  CGMEJoe.h

SOURCES = \
  CGMEJoe.cpp

# OBJECTS += $$S11N_OBJECTS
