include(../../config.qmake)
TEMPLATE = app
unix:contains(QMAKE_CXX,g++){ # hope we're working with gcc...
  # the qmake configure setup doesn't do this, and plugins require it!
  # Without this, my plugins can't resolve symbols in the application.
  QMAKE_LFLAGS += -export-dynamic
  warning("activating explicit -export-dynamic kludge")
}
unix:!contains(QMAKE_CXX,g++){
  warning("We're not under g++, which means i don't know the linker args to export your app symbols!")
}

QT += script svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS
RESOURCES = $$RESOURCES_DIR/icons.qrc \
	$$RESOURCES_DIR/help.qrc \
	$$RESOURCES_DIR/javascript.qrc

FORMS = \
 $$UI_SRCDIR/AboutQBoard.ui \
 $$UI_SRCDIR/MainWindow.ui \
 $$UI_SRCDIR/QGIHtmlEditor.ui \
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
