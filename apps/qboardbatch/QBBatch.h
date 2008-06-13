#ifndef QBOARD_QBATCH_H_INCLUDED
#define QBOARD_QBATCH_H_INCLUDED

#include "parsepp.hpp"
#include "parsepp_strings.hpp"

#include "GamePiece.h"
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
    typedef std::map<std::string,int> Name2VTMap;

    /**
       Maps some common property names to the QVariant::Types
       equivalent.
     */
    Name2VTMap & namesMap();

    /**
       Given a property name, it returns the QVariant::Types value
       defined in namesMap(). If n is not found, QVariant::Invalid
       is returned.
    */
    int vtype( std::string const & n );

    /**
       Tries to convert the given property to the QVariant type
       expected by the QBoard-related types. It's just a guess,
       really. If typeHint is not QVariant::Invalid then the value is
       converted to the given QVariant type, otherwise vtype() is used
       to try to determine the type. When specifying a custom type, it
       is imperitive that the val string is in the format expected for
       values of that type (as documented elsewhere).
    */
    QVariant propToVariant( std::string const & key,
			    std::string const & val,
			    int typeHint = QVariant::Invalid );

    /**
       State for the qboardbatch parser.
    */
    struct State
    {
	GameState gstate;
	GamePiece * piece;
	QObject * current; // current item: either piece or board
	std::string key; // current property key
	std::string toeocmd; // collected by r_toeocmd
	bool verbose;
	std::string theString; // buffer for quoted string parser
	int vtype; // type hint for property (from QVariant::Types)
	State() :
	    gstate(),
	    piece(0),
	    current(&gstate.board()),
	    key(),
	    toeocmd(),
	    verbose(false),
	    theString(),
	    vtype(QVariant::Invalid)
	{
	}
    };

    /**
       State translator For use with Ps::strings<>.
    */
    struct StringParseXLate
    {
	void start_string( State & st)
	{
	    st.theString.clear();
	}
	void append( State &st, std::string const & m )
	{
	    st.theString += m;
	}
	void end_string( State & )
	{
	}
    };

#define RL rule_list
#define CL char_list
    using namespace Ps;


    /** Skips leading whitespace and runs rule R. */
    template <typename R = r_success>
    struct skipws : r_and< RL<
	r_star<r_blank>,
	R > >
    {};

    /**
       Matches the end of a command.
    */
    struct eocmd :
	r_or< RL<
	    r_eof,
 	    r_comment_cpp,
 	    r_comment_c,
	    r_eol
	    > >
    {};

    /**
       Sets st.toeocmd.
    */
    struct a_set_toeocmd;

    /**
       Matches everything up to the end of the current command.
    */
    struct r_toeocmd : skipws<
	r_action< r_star<
	    r_and< RL< r_notat< eocmd >, r_any > >
	    >, a_set_toeocmd>
	>
    {};


    struct cn_piece : skipws< r_chseqi< CL<'p','i','e','c','e'> > >
    {};

    struct cn_board : skipws< r_chseqi< CL<'b','o','a','r','d'> > >
    {};

    /**
       Sets current object to the board.
    */
    struct a_board;
    /**
       Implements the 'board' command.
    */
    struct c_board : r_action< cn_board, a_board >
    {
    };

    /**
       Sets current item to the most recently created GamePiece.
    */
    struct a_piece;

    struct c_piece : r_action< cn_piece, a_piece >
    {
    };

    /**
       Implements the 'new' command.
    */
    struct a_new;

    struct c_new :
	r_and< RL<
	skipws< r_chseqi< CL<'n','e','w'> > >,
	skipws< r_action< r_identifier, a_new > >,
	r_toeocmd
	> >
    {};

    /**
       Used by a_set to specify its mode of operation.
    */
    enum SetModes {
    SetKey = 0,
    SetValToEocmd = 1,
    SetValString = 2,
    SetValType = 3
    };

    /**
       Implements the 'set' command.

       Modes:

       SetKey = the matched string is a property key

       SetValToEocmd = sets the prop val to st.toeocmd

       SetValString = sets prop val to st.theString

       SetValType = sets the type to be used for the next
       set operation.
    */
    template <SetModes Mode, int Type = QVariant::Invalid>
    struct a_set
    {
	typedef a_set type;
	static void matched( parser_state & ps, std::string const & m, State & st )
	{
	    parse_iterator pit( ps.pos() );
	    if( SetValType == Mode )
	    {
		QBB_VERBOSE << "Setting next property to type "<<Type<<".\n";
		st.vtype = Type;
		return;
	    }
	    else if( SetKey == Mode )
	    {
		st.key = m;
		return;
	    }
	    QVariant var;
	    switch( Mode )
	    {
	      case SetValToEocmd:
		  var = propToVariant( st.key, st.toeocmd, st.vtype );
		  break;
	      case SetValString:
		  var = propToVariant( st.key, st.theString, st.vtype );
		  break;
	      default: {
		  std::ostringstream os;
		  os << "a_set<"<<Mode<<">: should never happen!";
		  std::string msg(os.str());
		  QBB_PERR << msg << '\n';
		  throw parse_error(ps,msg);
	      }
		  break;
	    }
	    st.vtype = QVariant::Invalid;
	    st.current->setProperty( st.key.c_str(), var );
	    QBB_VERBOSE << "set: ["<<st.key<<"] =";
	    if( st.verbose ) qDebug() << var;
	}
    };

    /**
       Triggers a parse error for invalid 'set -TYPE' flags.
    */
    struct a_set_type_error;

    /**
       Parser for the 'set -TYPE' flags.
    */
    struct c_set_type :
	r_or<
	RL<
	    r_action< r_chseq<CL<'p','o','i','n','t'> >, a_set<SetValType,QVariant::Point> >,
	    r_action< r_chseq<CL<'s','i','z','e'> >, a_set<SetValType,QVariant::Size> >,
	    r_action< r_chseq<CL<'s','t','r','i','n','g'> >, a_set<SetValType,QVariant::String> >,
	    r_action< r_chseq<CL<'i','n','t'> >, a_set<SetValType,QVariant::Int> >,
	    r_action< r_chseq<CL<'d','o','u','b','l','e'> >, a_set<SetValType,QVariant::Double> >,
	    r_action< r_chseq<CL<'c','o','l','o','r'> >, a_set<SetValType,QVariant::Color> >,
	    r_action< r_tospace, a_set_type_error >
	> >
    {};

    /**
       Main parser for 'set -TYPE'
    */
    struct c_set_flags :
	r_and<RL<
	r_ch<'-'>, c_set_type
	> >
    {};

    /**
       Parser for 'set' arguments:

       [-TYPE] key
    */
    struct c_set_arglist :
	r_and<RL<
	    skipws<>,
	    r_opt< c_set_flags >,
	    skipws<>,
	    r_action< r_identifier, a_set<SetKey> >
	> >
    {};

    /**
       Parses the VALUE part of:

       set [-TYPE] KEY VALUE
    */
    struct c_set_val : r_or< RL<
	r_action< strings<StringParseXLate>::start, a_set<SetValString> >,
	r_action< r_toeocmd, a_set<SetValToEocmd> >
    > >
    {
    };

    /**
       Main parser for the 'set' command.
    */
    struct c_set :
	r_and< RL<
	    skipws<>,
	    r_chseqi< CL<'s','e','t'> >,
	    r_or< RL< c_set_arglist, r_throw > >,
	    skipws<>,
	    c_set_val
	    > >
    {};

    /**
       Implements the 'save' action.
     */
    struct a_save;

    struct c_save : r_action< r_and< RL<r_chseqi< CL<'s','a','v','e'> >, r_toeocmd> >, a_save >
    {};

   /**
      Parser for various "empty" constructs: end-of-command and
      comments.
    */
    struct c_empty : r_star< r_or< RL<
	eocmd,
	r_comment_c,
	r_comment_cpp
	> > >
    {};

    /**
       Parses a single command or empty construct.
    */
    struct command : r_skipblanks< r_or< RL<
	c_set,
	c_new,
	c_board,
	c_piece,
	c_save,
	c_empty,
	r_throw
	> > >
    {};
    struct start : r_plus<command>
    {};



    /**
       Processes a set of qboardbatch scripts.

       argc must be the number of items in argv,
       which is assumed to be a list of filenames
       or parameters explained below. On error it
       throws a std::exception, hopefully containing
       some useful information about the cause of
       the error.

       Parameters processed specially by this function:

       -v  turns on verbose mode.

       Others may be added at some point.
    */
    void process_scripts( int argc, char ** argv );
}

#undef RL
#undef CL


#endif // QBOARD_QBATCH_H_INCLUDED
