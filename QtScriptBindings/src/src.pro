TEMPLATE = subdirs
SUBDIRS =

########################################################################
# Comment out or remove any particular bindings you don't want:
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

########################################################################
# If you don't have GNU Readline dev files installed, comment this out:
unix {
  SUBDIRS += readline
  SUBDIRS += qs_eval
}


for( MODULE, BINDINGS_TO_BUILD ){
  SUBDIRS += com_trolltech_qt_$${MODULE}
}
