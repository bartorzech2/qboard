include(../../config.qmake)
OBJECTS_DIR = obj
TEMPLATE = app
unix:contains(QMAKE_CXX,g++){
  QMAKE_LFLAGS += -Wl,-E
  warning("activating explicit -export-dynamic kludge")
}
!contains(QMAKE_CXX,g++){
  warning("We're not under g++, which means i don't know the linker args to export your app symbols!")
}

win32:CONFIG += console
mac:CONFIG -= app_bundle

QMAKE_CXXFLAGS = $$QBOARD_CXXFLAGS

SOURCES = \
	main.cpp \
	Readline.cpp

########################################################################
# See ../QBoard/QBoard.pro for why this wickedness exists...
unix:{
  LIBS += -lreadline -L$$DESTDIR -lQBoard -lQBoardS11n
}
win32:{
  error("Sorry, we can't build this on Windows.");
}
