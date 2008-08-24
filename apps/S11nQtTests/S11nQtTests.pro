include(../../config.qmake)
TEMPLATE = app
QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS

HEADERS = \
 $$S11NQT_HEADERS

SOURCES = \
 $$S11NQT_SOURCES \
 S11nQtTests.cpp

OBJECTS += $$S11N_OBJECTS
