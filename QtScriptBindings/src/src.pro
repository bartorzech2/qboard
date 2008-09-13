TEMPLATE = subdirs
SUBDIRS =
BINDINGS_TO_BUILD = core
BINDINGS_TO_BUILD += gui
BINDINGS_TO_BUILD += uitools
BINDINGS_TO_BUILD += network
BINDINGS_TO_BUILD += opengl
BINDINGS_TO_BUILD += sql
BINDINGS_TO_BUILD += svg
BINDINGS_TO_BUILD += webkit
BINDINGS_TO_BUILD += xml
BINDINGS_TO_BUILD += xmlpatterns

for( MODULE, BINDINGS_TO_BUILD ){
  SUBDIRS += com_trolltech_qt_$${MODULE}
}

unix{
  # Only build this if you have libreadline dev files installed:
  SUBDIRS += qs_eval
}
