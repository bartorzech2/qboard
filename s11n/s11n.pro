########################################################################
# This file exists only to build the s11n .o file separately from
# the rest of the apps.
# The generated library will not work on Windows because s11n's
# classloader model can't work under Win32. Thus this project
# file exists only to create the .o files, and we ignore
# the generated .so/.dll altogether.
include(../config.qmake)
VERSION = QBoardDummy
TEMPLATE = app
TARGET = s11nconvert
QT =
CONFIG = debug
OBJECTS_DIR = $$S11N_OBJ_DIR
QMAKE_CXXFLAGS = $$S11N_CXXFLAGS
unix:{
	QMAKE_LFLAGS += -lstdc++
	QMAKE_LFLAGS += -lz
}

HEADERS = \
 $$S11N_HEADERS \
 argv_parser.hpp \

SOURCES = \
 $$S11N_SOURCES \
 argv_parser.cpp \
 main.cpp
