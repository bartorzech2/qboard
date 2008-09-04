include(../../config.qmake)
TEMPLATE = app
unix:contains(QMAKE_CXX,g++){
  # Let's hope we're working with the ld linker...
  #
  # The qmake configure setup doesn't set up apps to export their
  # symbols, yet plugins which use the app's symbols require it.
  # Without this, plugins can't resolve symbols in the application:
  #
  # QMAKE_LFLAGS += -export-dynamic
  #
  # Suggestion from Qt tech support:
  QMAKE_LFLAGS += -Wl,-E
  # but they also admit that qmake doesn't have a built-in way to do this
  # because (and i quote):
  #
  # "The situation where the library needs to access symbols from the
  # application is not that common in our experience so there's no option
  # in qmake to do this."
  #
  # Which means that, per their model, plugins are only supposed to be
  # providers, and not users of the application's API. Bummer. One-way
  # plugins.
  warning("activating explicit -export-dynamic kludge")
}
!contains(QMAKE_CXX,g++){
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
