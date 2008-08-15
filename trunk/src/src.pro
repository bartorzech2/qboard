########################################################################
# This file exists only so i can run make from the "proper" directory
# using emacs's make commands.
# The generated library is bogus - we don't use it. It exists only to
# trigger the build of the source files.


include(../config.qmake)
TEMPLATE = lib
TARGET = dummy
VERSION = QBoardDummy


QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
# RESOURCES = $$RESOURCES_DIR/icons.qrc $$RESOURCES_DIR/help.qrc
FORMS = \
 $$UI_SRCDIR/AboutQBoard.ui \
 $$UI_SRCDIR/MainWindow.ui \
 $$UI_SRCDIR/QGIHtmlEditor.ui \
 $$UI_SRCDIR/ScriptWindow.ui \
 $$UI_SRCDIR/SetupQBoard.ui \
 $$QBOARD_FORMS_QT44

HEADERS = \
 $$QBOARD_HEADERS_LIB \
 $$QBOARD_HEADERS_WINDOW

SOURCES = \
 $$QBOARD_SOURCES_LIB \
 $$QBOARD_SOURCES_WINDOW \
 $$QBOARD_SOURCES_MAINAPP

OBJECTS += $$S11N_OBJECTS
