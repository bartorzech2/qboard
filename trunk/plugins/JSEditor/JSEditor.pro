include(../../config.qmake)
TEMPLATE = lib
CONFIG += plugin
QT += script
DESTDIR = $$QBOARD_PLUGINS_DESTDIR
QMAKE_CXXFLAGS += $$QBOARD_CXXFLAGS
TARGET = $$qtLibraryTarget(JSEditor)
HEADERS = \
  JSEditor.h \
  highlighter.h

SOURCES = \
  JSEditor.cpp \
  highlighter.cpp

# OBJECTS += $$S11N_OBJECTS
