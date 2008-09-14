include(../config.qmake)
TEMPLATE = subdirs
SUBDIRS = QBoard
unix:contains(QBOARD_VERSION,^0$){
# only build for a dev tree...
  SUBDIRS += S11nQtTests QBoardScript WikiLiteParser
}
