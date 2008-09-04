include(../../config.qmake)
TEMPLATE = app
QMAKE_CXXFLAGS += $$QBOARD_CXXFLAGS
HEADERS = \
 WikiLiteParser.h

SOURCES = \
 WikiLiteParser.cpp \
 wiki-main.cpp
