#ifndef QBOARD_QBATCH_H_INCLUDED
#define QBOARD_QBATCH_H_INCLUDED

#include <QString>
#include <QStringList>
#include "GameState.h"
#include <map>

#define QBB_VERBOSE if( st.verbose ) std::cerr << "QBB_VERBOSE: "
#define QBB_WARNING qDebug() << "QBB_WARNING: "
#define QBB_QDEBUG qDebug() << QString(__FILE__) << ":" << __LINE__ << " : "
#define QBB_PERR std::cerr << "PARSE ERROR:\n" << __FILE__ << ":" << std::dec << __LINE__ << " : "

/************************************************************************
qboardbatch implements a very basic batch script language for creating
QBoard games. The intended usage is to use shell scripts to generate
game data based on external information (e.g. a directory of piece
graphics), which is then given to this app.


These are the core commands, listed here [in brackets]:


  [set KEY VALUE] sets property KEY on the current object. More on
  this below.

  [new piece] creates a new piece and makes it the current object.

  [new game] clears the game state and starts a new game.

  [board] sets the current active item to the game board. At startup,
  the default item is the board (which always exists).

  [piece] sets the current active item to the most recently-created
  piece.

  [save [filename]] saves the game state to a file or stdout. Note that
  it uses GameState::save() to do this, which means that saving to
  a file will cause an automatic file extension to be added.
  The saved file should be loadable by QBoard.

  [COMMENTS] are supported using C- or C++-style.

A script runs only linearly. There are no features such as branching,
conditionals, function calls, etc. Here is a sample script:

------------------------------------------------------------------------
set pixmap qboard_home/relative/path/to/map.jpg
new piece
set pixmap mygame/counters/unit1.png
set color #ff0000
set pos 100 200
new piece
// C++-style comments are supported
set pixmap mygame/counters/unit2.png
set pos 20 100
set angle 60
set colorAlpha 100
set borderColor red
save outfile_name
------------------------------------------------------------------------

COMMANDS in more detail:

========================================================================
SET command:

  set [-TYPE] KEY VALUE...

The property named KEY is set to the given value The VALUE is read
until the end-of-line (or start of a comment). It may optionally be
quoted using either double- or single-quotes, and if it is then the
value may span across lines but must backslash-escape any quote
characters within the string.  e.g. "he said, \"hi, there!\""

The optional -TYPE hint can be used to explicitely set the underlying
QVariant type for a property. This is useful when specifying custom
properties which this code doesn't know about.

The -TYPE may be one of:

    -point Treats val as a QPoint. Requires that VALUE be in the form
    "X Y" (2 int values)

    -size Treat val as a QSize. Requires that VALUE be in the form "W
    H" (2 int values)

    -string Treats value as a string. This is the default type hint,
    but relying on the default for unknown string properties will
    cause a warning to be emitted during parsing.

    -int or -double treats value as the appropriate enumeric type.

    -color Treats value as a color string, which must be in the form
    "#ffffff" or "R G B" (3 integer values).

Only the most common properties from the GamePiece, QBoard, etc. classes
are explicitly handled here with their expected type conversions. Other
properties will be set as Strings unless the -TYPE hint is used when
setting them. Client apps may depend on a given property having a certain
type, so using the default to-string conversion is not recommended.

========================================================================

************************************************************************/
namespace QBBatch {

    /**
       Processes a single qboardbatch script, acting on the given
       GameState object (which may be 0, in which case an anonymous
       GameState is used).

       On error it throws a std::exception, hopefully containing some
       useful information about the cause of the error.

    */
    void process_script( GameState * gs, QString const & filename );

    /*
      The plural form of process_script(). Identical in all ways
      except:

      The list is is assumed to be a list of filenames
      or special parameters explained below.

      Parameters processed specially by this function:

      -v turns on verbose mode for all scripts processed after -v.

      Others special flags may be added at some point.
    */
    void process_scripts( GameState * gs, QStringList const & filenames );
}

#endif // QBOARD_QBATCH_H_INCLUDED
