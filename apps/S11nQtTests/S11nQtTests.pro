include(../../config.qmake)
TEMPLATE = app
QT += script svg
QMAKE_CXXFLAGS = -I$$MAIN_INCLUDES_DIR/qboard $$QBOARD_CXXFLAGS

HEADERS = \
 $$S11NQT_HEADERS

SOURCES = \
 S11nQtTests.cpp

LIBS += -L$$DESTDIR -lQBoard -lQBoardS11n
#QMAKE_LIBS += $$S11N_OBJECTS \
#   $$S11NQT_OBJECTS # We add these to QMAKE_LIBS instead of OBJECTS to keep qmake from cleaning them!
