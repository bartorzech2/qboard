include(../../config.qmake)
# OBJECTS_DIR = obj
TEMPLATE = app
QMAKE_CXXFLAGS += $$QBOARD_CXXFLAGS
HEADERS = \
 $$QBOARD_INCLUDES_DIR/WikiLiteParser.h

SOURCES = \
 $$SRC_DIR/WikiLiteParser.cpp \
 wiki-main.cpp

