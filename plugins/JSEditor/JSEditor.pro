include(../../config.qmake)
TEMPLATE = lib
CONFIG += plugin
QT += script
DEFINES += QT_DLL
DESTDIR = $$QBOARD_PLUGINS_DESTDIR
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
TARGET = $$qtLibraryTarget(JSEditor)
# In qt4.4.3 they apparently added the -Wl,--no-undefined linker arg, which breaks plugin builds unless they link back to their "master" DLL:
LIBS += $$QBOARD_LIBS
HEADERS = \
  JSEditor.h \
  highlighter.h

SOURCES = \
  JSEditor.cpp \
  highlighter.cpp

# OBJECTS += $$S11N_OBJECTS
