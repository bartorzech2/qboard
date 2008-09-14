This is a minimalist QtScript plugin for a GNU Readline-like
interface.  In actuallity it supports GNU Readline, BSD Editline, and
stdin as input sources, but for purposes of this plugin it's
configured to use GNU Readline.

========================================================================
BUILDING AND INSTALLING

      ~> qmake
      ~> make

Copy the generated library file to QT_DIR/plugins/script (or another
dir in your plugins path).

========================================================================
USING THE PLUGIN

Load it into your JS engine:

     myEngine->importExtension("readline");

To use it from JS code, call Readline.read(), passing it a prompt
string:

    var s = Readline.read("prompt: ");
    if( undefined === s ) {
       // User tapped Ctrl-D or otherwise triggered end of input
    }
    else {
        print("User entered:",s);
    }


If the user taps the EOF sequence (normally Ctrl-D) then undefined is
returned, otherwise the entered string (possibly empty) is returned.

History support:

Readline can remember what you've typed before. To load a history
file, creating it if needed, use:

	bool Readline.history.load(filename)

To save history file (or it is automatically saved when the Readline
object is destroyed):

	bool Readline.history.save(filename)

To clear the input history:

	Readline.history.clear()

There is not (and won't be) any support for more complex features of
GNU Readline, like custom tab expansions. The reason is simple: the
underlying read handler isn't bound specifically to GNU Readline - it
can use lesser APIs if GNU Readline isn't available (see Readline.hpp
and Readline.cpp for the details).

