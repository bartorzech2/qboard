include(../../config.qmake)
TEMPLATE = app
QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS

HEADERS = \
 $$QBOARD_HEADERS_LIB

#  parsepp_append.hpp parsepp_numeric.hpp


SOURCES = \
 $$QBOARD_SOURCES_LIB \
 qboardbatch.cpp

OBJECTS += $$S11N_OBJECTS
