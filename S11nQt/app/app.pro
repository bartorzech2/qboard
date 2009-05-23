include(../config.qmake)
TEMPLATE = app
TARGET = S11nQtTests
QMAKE_LFLAGS = -L$$BUILD_DEST_DIR -lS11nQt
DESTDIR = $$BUILD_DEST_DIR
#HEADERS = \
# $$S11NQT_HEADERS

SOURCES = \
 S11nQtTests.cpp

#LIBS += -L$$DESTDIR -lGalaxyBowlS11n
#QMAKE_LIBS += $$S11N_OBJECTS \
#   $$S11NQT_OBJECTS # We add these to QMAKE_LIBS instead of OBJECTS to keep qmake from cleaning them!
