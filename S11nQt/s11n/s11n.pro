########################################################################
# qmake project file for libs11n + S11nQt extensions.
# The generated library will not work on Windows because s11n's
# classloader model can't work under Win32.
include(../config.qmake)
TEMPLATE = lib
TARGET = S11nQt
OBJECTS_DIR = $$OBJECTS_DIR/obj-s11n
MOC_DIR = $$OBJECTS_DIR
MAIN_INCLUDES_DIR = $$REL_SRCDIR/include
DESTDIR = $$BUILD_DEST_DIR

message(libS11nQt version $$VERSION)
message(QT_VERSION $$QT_VERSION)

#QT = core
CONFIG += dll debug warn_on no_keywords
#OBJECTS_DIR = $$S11N_OBJ_DIR
unix: QMAKE_LFLAGS += -lstdc++

#S11N_CXXFLAGS = -I$$MAIN_INCLUDES_DIR
S11N_CXXFLAGS =
S11N_SOURCES_CORE = \
  data_node_io.cpp \
  exception.cpp \
  path_finder.cpp \
  plugin.cpp \
  s11n.cpp \
  s11n_node.cpp \
  s11nlite.cpp \
  strtool.cpp
unix:{
	S11N_SOURCES_CORE += gzstream.cpp
	QMAKE_LFLAGS += -lz
	S11N_CXXFLAGS += -DHAVE_ZLIB=1
}
S11N_SOURCES_SERIALIZERS = \
 funtxt.flex.cpp \
 funtxt_serializer.cpp \
 funxml.flex.cpp \
 funxml_serializer.cpp \
 js_serializer.cpp \
 parens.flex.cpp \
 parens_serializer.cpp


H=$$MAIN_INCLUDES_DIR/s11n.net/s11n/qt
S11N_HEADERS_QT = \
 $$H/Serializable.h \
 $$H/S11n.h \
 $$H/S11nClipboard.h \
 $$H/S11nQt.h
H=
S11N_SOURCES_QT = \
 S11nClipboard.cpp \
 S11nQt.cpp \
 S11nQtStream.cpp \
 Serializable.cpp
S11NQT_OBJECTS =
for(CPP, S11NQT_SOURCES){
  BASENAME = $$basename(CPP)
  S11NQT_OBJECTS += $$OBJECTS_DIR/$$replace(BASENAME,.cpp,$$DOT_OBJ)
}



HEADERS = \
 $$S11N_HEADERS_QT
# $$S11N_HEADERS
# argv_parser.hpp

SOURCES = \
 $$S11N_SOURCES_CORE \
 $$S11N_SOURCES_SERIALIZERS \
 $$S11N_SOURCES_QT
 # argv_parser.cpp \
 # main.cpp

QMAKE_CXXFLAGS += $$S11N_CXXFLAGS
#QMAKE_INCDIR += $$MAIN_INCLUDES_DIR

