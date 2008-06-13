include(../../config.qmake)
TEMPLATE = app
QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS

HEADERS = \
  parsepp_append.hpp \
  parsepp.hpp \
  parsepp_numeric.hpp \
  parsepp_strings.hpp \
  parsepp_typelist_10.hpp \
  parsepp_typelist.hpp \
  QBBatch.h \
 $$QBOARD_HEADERS_LIB

SOURCES = \
 $$QBOARD_SOURCES_LIB \
 QBBatch.cpp \
 qboardbatch.cpp

OBJECTS += $$S11N_OBJECTS
