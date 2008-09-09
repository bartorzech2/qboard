include(../config.qmake)
TEMPLATE = subdirs
SUBDIRS = QBoard WikiLiteParser
!$$QBOARD_VERSION: SUBDIRS += S11nQtTests QBoardScript
