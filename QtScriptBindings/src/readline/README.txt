This is a minimalist QtScript plugin for a GNU Readline-like interface.
In actuallity it supports GNU Readline, BSD Editline, and stdin as input
sources, but for purposes of this plugin it's configured to use GNU Readline.

Using this plugin:

Build it with:

      ~> qmake
      ~> make

Copy the generated library file to QT_DIR/plugins/script (or another
dir in your plugins path).

Load it into your JS engine:

     myEngine->importExtension("readline");

Use it from JS code:

    var s = Readline.read("prompt: ");
    if( undefined === s ) {
       // User tapped Ctrl-D or otherwise triggered end of input
    }
    else {
        print("User entered:",s);
    }



History support:

Load a history file, creating it if needed:

	bool Readline.history.load(filename)

Save history file (it is automatically saved when the Readline object
is destroyed):

	bool Readline.history.save(filename)

Clear the input history:

	Readline.history.clear()

