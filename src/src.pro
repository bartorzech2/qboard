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
 $$UI_SRCDIR/QGIHtmlEditor.ui \
 $$QBOARD_FORMS_QT44

HEADERS = \
 $$QBOARD_HEADERS_LIB

SOURCES = \
 $$QBOARD_SOURCES_LIB
# OBJECTS = $$QBOARD_OBJECTS_LIB
OBJECTS += $$S11N_OBJECTS
unix:{
  LIBS += -L$$DESTDIR #  -lQBoardS11n
}
#win32:{
#  QMAKE_LIBS += $$S11N_OBJECTS # We add these to QMAKE_LIBS instead of OBJECTS to keep this tree from cleaning them!
#}
