OBJECTS_DIR = obj
TEMPLATE = app
QT += core gui script
win32:CONFIG += console
mac:CONFIG -= app_bundle

SOURCES = \
	main.cpp \
	Readline.cpp

########################################################################
# See ../QBoard/QBoard.pro for why this wickedness exists...
unix:{
  LIBS += -lreadline
}
win32:{
  error("Sorry, we can't build this on Windows.");
}
