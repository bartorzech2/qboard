########################################################################
# If you do not have the GNU Readline headers/libs installed, set
# HAVE_READLINE to 0:
QMAKE_CXXFLAGS += -DHAVE_READLINE=1

PLUGIN_NAME=readline
QT += core
message(PLUGIN_NAME=$${PLUGIN_NAME})
CONFIG += plugin script
TEMPLATE = lib
OBJECTS_DIR = obj
TARGET = $$qtLibraryTarget(qtscript_$${PLUGIN_NAME})
DESTDIR = ../../build/plugins/script

SOURCES = \
	main.cpp \
	Readline.cpp
