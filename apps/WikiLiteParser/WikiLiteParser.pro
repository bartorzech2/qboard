include(../../config.qmake)
TEMPLATE = app
QMAKE_CXXFLAGS += $$QBOARD_CXXFLAGS
HEADERS = \
 $$SRC_DIR/WikiLiteParser.h

SOURCES = \
 $$SRC_DIR/WikiLiteParser.cpp \
 wiki-main.cpp
