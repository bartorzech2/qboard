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

QT += svg
QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS


########################################################################
# Sources related to the main GUI.
QBOARD_SOURCES_MAINAPP = \
 AboutQBoardImpl.cpp \
 MainWindowImpl.cpp \
 main.cpp \
 SetupQBoard.cpp

QBOARD_HEADERS_MAINAPP = \
 AboutQBoardImpl.h \
 MainWindowImpl.h \
 SetupQBoard.h \
 $$QBOARD_HEADERS_QT44

RESOURCES = $$RESOURCES_DIR/icons.qrc \
	$$RESOURCES_DIR/help.qrc \
	$$RESOURCES_DIR/javascript.qrc \
	$$RESOURCES_DIR/images.qrc


FORMS = \
 $$UI_SRCDIR/AboutQBoard.ui \
 $$UI_SRCDIR/MainWindow.ui \
 $$UI_SRCDIR/SetupQBoard.ui

HEADERS = \
 $$QBOARD_HEADERS_MAINAPP

# $$QBOARD_HEADERS_LIB \
# $$QBOARD_SOURCES_LIB \

SOURCES = \
 $$QBOARD_SOURCES_MAINAPP


unix:{
  # On unix platforms (at least under gcc/ld) we don't have a problem using s11n as a library.
  # On Windows platforms the s11n classloader can't work because of impossible-to-meet
  # symbol export requirements on those platforms.
  LIBS += $$QBOARD_LIBS
  #LIBS += -L$$DESTDIR -lQBoardS11n
  #OBJECTS += $$QBOARD_OBJECTS_LIB
  #OBJECTS += $$S11N_OBJECTS
  #QMAKE_LIBS += $$S11N_OBJECTS # We add these to QMAKE_LIBS instead of OBJECTS to keep this tree from cleaning them!
  #QMAKE_LIBS += $$QBOARD_OBJECTS_LIB
  #LIBS += -L$$(HOME)/lib  -ls11n
}
win32:{
  #OBJECTS += $$QBOARD_OBJECTS_LIB
  #OBJECTS += $$S11N_OBJECTS
  HEADERS = \
   $$QBOARD_HEADERS_MAINAPP \
   $$QBOARD_HEADERS_LIB # without this, mocs aren't linked
  QMAKE_LIBS += $$S11N_OBJECTS # We add these to QMAKE_LIBS instead of OBJECTS to keep this tree from cleaning them!
  QMAKE_LIBS += $$QBOARD_OBJECTS_LIB
}
