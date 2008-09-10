include(../config.qmake)
TEMPLATE = subdirs
SUBDIRS = QBoard
unix:!$$QBOARD_VERSION: SUBDIRS += S11nQtTests QBoardScript WikiLiteParser
