PLUGIN_NAME=com_trolltech_qt_xml
QT += xml
QMAKE_CXXFLAGS += -I$${QMAKE_INCDIR_QT}/QtXmlPatterns
include(../plugin_template.qmake)
