This distribution contains pre-generated Qt/JavaScript bindings
for a large set of the Qt API. It was generated using the
Qt Script Generator:

   http://labs.trolltech.com/page/Projects/QtScript/Generator

This code is distributed primarily for use with the QBoard project
(http://code.google.com/p/qboard), but is distributed independently
in the hopes that it might be useful to others.

License: Same as Qt Open Source Edition (GPLv2 or GPLv3)

Requirements: Qt 4.4.2+ (qmake, headers, libs)
It won't work with Qt <= 4.4.1

========================================================================
Configuring it:

By default the build process will try to build all available bindings.
If you want to limit what modules are built (e.g. if your Qt doesn't
have OpenGL or WebKit support), edit src/src.pro and comment out or
remove the appropriate lines.

========================================================================
Building it:

   ~> qmake
   ~> make

Go get some coffee. That'll take a bit.

That will build several .so files to:

     build/plugins/script

IF you have GNU Readline development files installed, edit src/src.pro
and uncomment the line which adds the subdir 'qs_eval'. That will build
a small console application which you can use to run the examples
in examples/*.qs, e.g.:

  ~> cd examples
  ~> ../src/qs_eval/qs_eval AnalogClock.qs

qs_eval automatically imports all extensions (or at least the ones
which will load on my box). Unfortunately, qs_eval hard-codes the
plugin paths and needs to stay in the directory it was built in.


========================================================================
Installing

To install the libraries and use them from your Qt apps you
have a couple options:

1) Copy them under QT_DIR/plugins/script

2) Copy them to DIR_OF_YOUR_CHOICE/plugins/script then add
DIR_OF_YOUR_CHOICE/plugins (without the '/script' suffix) to the Qt
library path from C++ code like so:

    QStringList paths = qApp->libraryPaths();
    paths <<  QString("DIR_OF_YOUR_CHOICE/plugins");
    qApp->setLibraryPaths(paths);


Then call:

     QScriptEngine * js = ...;
     js->importExtension(XXX);

where XXX is one of the following strings:

      qt.core: the core Qt types
      qt.gui: the widgets and QGraphics stuff
      qt.network
      qt.opengl
      qt.sql
      qt.svg
      qt.uitools
      qt.webkit
      qt.xml
      qt.xmlpatterns

Alternately, you can add an importExtension() function to JS with
a little bit of work:


  static QScriptValue
  js_importExtension(QScriptContext *context, QScriptEngine *engine)
  {
      return engine->importExtension(context->argument(0).toString());
  }

  // Now define that function for JS:

  glob.setProperty("qt", js->newObject());
  glob.property("qt").
	setProperty("app", js->newQObject(QApplication::instance()) );
  QScriptValue qscript = js->newObject();
  qscript.setProperty("importExtension", js->newFunction(js_importExtension));
  glob.property("qt").setProperty("script", qscript);


Now call the following JS code to import the extension:


  qt.script.importExtension("qt.core");
  qt.script.importExtension("qt.gui");
  ...

========================================================================
EXTRA STUFF

Aside from the Qt API bindings, there might be some other plugins
i've written, adapted, or downright stolen:

     src/readline contains a GNU Readline-ish plugin. See the README
     in that directory for details.

========================================================================
NOTES:

i distribute this code but did not write most of it (only the "extra
stuff" listed above). i cannot offer support for any parts i didn't
write.

As of this writing (Sept 13, 2008) i have only tried a few of the JS
bindings. The ones i tried seemed to mostly work as expected, but some
of the example code (under examples/*.qs) contains some comments about
workaround for broken bindings. i'm sure any such shortcomings will be
remedied as the Qt Script Generator matures.
