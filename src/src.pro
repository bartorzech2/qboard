########################################################################
# This file exists only so i can run make from the "proper" directory
# using emacs's make commands.
# The generated library is bogus - we don't use it. It exists only to
# trigger the build of the source files.


include(../config.qmake)
TEMPLATE = lib
TARGET = QBoard
VERSION = $$QBOARD_VERSION

QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
# RESOURCES = $$RESOURCES_DIR/icons.qrc $$RESOURCES_DIR/help.qrc
FORMS = \
 $$UI_SRCDIR/QGIHtmlEditor.ui


HEADERS = \
 $$QBOARD_HEADERS_LIB

SOURCES = \
 $$QBOARD_SOURCES_LIB

# OBJECTS += $$S11N_OBJECTS
