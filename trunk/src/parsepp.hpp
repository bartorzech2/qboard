#ifndef s11n_net_PARSEPP_HPP_INCLUDED
#define s11n_net_PARSEPP_HPP_INCLUDED

#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <set>

#include "parsepp_typelist.hpp"

// PARSEPP_CURRENT_FUNCTION is basically the same as the conventional
// __FUNCTION__ macro, except that it tries to use compiler-specific
// "pretty-formatted" names. This macro is primarily intended for use
// with the source_info type to provide useful information in
// exception strings.  The idea of PARSEPP_CURRENT_FUNCTION and
// PARSEPP_SOURCEINFO is taken from the P::Classes and Pt frameworks.
#ifdef __GNUC__
#  define PARSEPP_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__BORLANDC__)
#  define PARSEPP_CURRENT_FUNCTION __FUNC__
#elif defined(_MSC_VER)
#  if _MSC_VER >= 1300 // .NET 2003
#    define PARSEPP_CURRENT_FUNCTION __FUNCDNAME__
#  else
#    define PARSEPP_CURRENT_FUNCTION __FUNCTION__
#  endif
#else
#  define PARSEPP_CURRENT_FUNCTION __FUNCTION__
#endif

// PARSEPP_SOURCEINFO is intended to simplify the instantiation of
// source_info objects, which are supposed to be passed values
// which are easiest to get via macros.
#define PARSEPP_SOURCEINFO (::Ps::source_info(__FILE__,__LINE__,PARSEPP_CURRENT_FUNCTION))

/**
   The parsepp namespace contains an experimental/alpha-quality parser
   toolkit, conceptually based very much on Dr. Colin Hirsch's PEGTL
   library (http://code.google.com/p/pegtl), but stripped of the error
   reporting facilities that library has. (This makes it much smaller
   but also not terribly useful for tracking errors.) It is similar to
   libraries like Boost.Spirit (http://spirit.sf.net) but is much,
   much smaller in scope.

   This package's home page is:

   http://wanderinghorse.net/cgi-bin/parsepp.cgi

   This source code is released into the Public Domain by its author,
   Stephan Beal (http://wanderinghorse.net/home/stephan/).

   General parsepp Conventions:

   - It only works for std::string input. Thus it requires buffering
   all input before parsing begins.

   - The ClientState template type a single client-supplied non-const
   object. This library ignores the State, but passes it on so that
   Actions (which are always client-side) may use them to accumulate
   information about a parse.

   - A Rule which matches any given token must advance the input
   iterator to one point past what it consumed. (There are a couple
   exceptions to this rule, and they are clearly documented.)

   RULES:

   Rules are represented by types with this structure:

\code
   struct my_rule {
	template <typename ClientState>
	static bool matches( parser_state & in, ClientState & st )
	{
	  ... return true if you can match against (in), else false ...
	}
  };
\endcode

   Most Rules advance the input if they return true, but a few do not.
   Rules which return false are required to roll back the parser_state
   to its previous position. See the numerous uses of the placemarker
   class for how to do this easily.

   Relatively few Rules actually have an implementation for their
   matches() function. Instead they subclass another Rule template which
   is composed of the required Rules. Rules which don't need an explicit
   implementation (most don't) can also be defined as typedefs instead
   of structs.

   The easiest way to parse a set of rules is using parse(), like so:

\code
parse< my_rule >( in, someObject );
\endcode

   The someObject can be any client-determined type. It is passed
   as-is to all Rules and Actions. All core Rules ignore the State
   object - they are strictly for client-side use. A State object is
   typically used to accumulate the results of parsing operations
   (e.g. appending tokens to a container, creating an abstract symbol
   tree, etc.).

   ACTIONS:

   Actions are Rule wrappers with this structure:

\code
struct my_action {
   template <typename ClientState>
   static void matched( parser_state &, std::string const &, ClientState &){...}
};
\endcode

   They are called like:

   <code>r_action< someRule, my_action ></code>

   If someRule matches then my_action::matched() is called, passed the
   parser_state, the matched string and the State object passed to
   parse().

   Notes about Actions and parser_state:

   Actions, as a general rule, do not use the parser_state
   argument. In theory, however, there is no reason to not allow
   Actions to activate other Rules (and that requires a parser_state
   object). Note, however, that the position of the parser_state
   iterator, as passed to the Action, may or may not have any relation
   to the matched string - that depends entirely on the rules which
   lead up to the activation of the Action. Thus Actions should
   normally not directly interact with the parser_state, but may pass
   it on to other code (most notably when an Action calls other
   Rules).
*/
namespace Ps {
    /**
       The Detail namespace holds private, internal-use-only code. It
       is not intended for use by client users.
    */
    namespace Detail {}

    /**
       source_info simplifies the collection of source file
       information for purposes of wrapping the info into
       exception strings.
	   
       This class is normally not instantiated directly,
       but is instead created using the PARSEPP_SOURCEINFO
       macro.
    */
    struct source_info
    {
	char const * file;
	size_t line;
	char const * func;
	/**
	   It is expected that this function be passed
	   __FILE__, __LINE__, and
	   PARSEPP_CURRENT_FUNCTION.
	*/
	source_info( char const * file, unsigned int line, char const * func )
	    :file(file), line(line), func(func)
	{}
	~source_info() {}

	std::string str() const
	{
	    std::ostringstream os;
	    os << this->file<<':'<<this->line<<':'<<this->func;
	    return os.str();
	}

    };

    /**
       This is THE iterator type supported by parsepp. It is
       essentially a std::string::iterator, but it is extended:

       - It knows its begin() and end() point, so it can avoid
       overflows.  Use eof() and bof() to check.

       - It will not decrement/increment past its start/end points.

       - parse_iterators are read-only iterators. They cannot be used
       to change their input.
    */
    class parse_iterator : public std::iterator<std::input_iterator_tag,const std::string::value_type>
    {
    private:
	static const int start_column = 1;
    public:
	typedef std::string::const_iterator iterator;
	/**
	   Constructs an empty iterator, useful only as the target
	   of an assignment.
	*/
	parse_iterator() : m_beg(), m_pos(), m_end(m_pos), m_line(1), m_col(start_column)
	{}
	/**
	   Constructs an iterator for the range [it,end). The string
	   pointed-to by (it,end) must not change (or otherwise
	   invalid the iterators) for the life of this object, or
	   undefined behaviour results.
	*/
	parse_iterator( iterator it, iterator end )
	    : m_beg(it), m_pos(it), m_end(end), m_line(1), m_col(start_column)
	{}
	/**
	   Equivalent to the parse_iterator(in.begin(),in.end()).

	   Note that the input string must outlive this
	   iterator. Also, the string MUST NOT change for the life of
	   this iterator (and any copies of this iterator). Changing
	   the string invalidates all iterators.
	*/
	explicit parse_iterator( std::string const & in )
	    : m_beg(in.begin()),m_pos(m_beg), m_end(in.end()),
	      m_line(1), m_col(start_column)
	{}

	/**
	   Returns true if this object has been advanced
	   to its end iterator or if the current
	   char is a null char, otherwise returns false.
	*/
	bool eof() const
	{
	    return (this->m_end == this->m_pos);
	// do we want this? || (0 == *this->m_pos);
	}

	/**
	   Returns true if at the begining of the input.
	*/
	bool bof() const
	{
	    return (this->m_beg == this->m_pos);
	}

	/**
	   Returns a copy of this object's underlying iterator,
	   pointing to the current position in the input.
	*/
	iterator iter() const
	{
	    return this->m_pos;
	}

	/**
	   Returns a copy of this object's underlying start point
	   iterator.
	*/
	iterator begin() const
	{
	    return this->m_beg;
	}
	/**
	   Returns a copy of this object's underlying end point
	   iterator.
	*/
	iterator end() const
	{
	    return this->m_end;
	}

	/**
	   Prefix increment. Avances this object one char then returns
	   this object.

	   This operator ignores any attempt to go past the end of
	   input (i.e., ++iterator becomes a no-op once iterator.eof()
	   is true).
	*/
 	parse_iterator & operator++()
 	{
	    if( m_pos != m_end )
	    {
		if( ++m_pos != m_end )
		{
		    if('\n' == *m_pos)
		    {
			m_line += 1;
			m_col = 0;
		    }
		    else
		    {
			++m_col;
		    }
		}
 		else
 		{
 		    ++m_col; // for symmetry with op--
 		}
	    }
 	    return *this;
 	}

	/**
	   Postfix increment. Advances this object by one char and
	   returns the pre-incremented value.
	   
	   See the Prefix form for notes about advancing past the end
	   of the input.
	*/
 	parse_iterator operator++(int)
 	{
	    return (this->m_pos != this->m_end )
		? (++(parse_iterator(*this)))
		: *this;
 	}

	/**
	   Prefix decrement. Decrements the iternal iterator.  It will
	   not decrement to a point before begin(). Trying to do so
	   is a no-op.
	*/
 	parse_iterator & operator--()
 	{
	    if( this->m_pos != this->m_beg )
	    {
		if( m_beg != --m_pos )
		{
		    if('\n' == *m_pos)
		    {
			m_line -= 1;
			m_col = 0;
			iterator it = m_pos;
			--it;
			for( ; it != m_beg; --it )
			{
			    ++m_col;
			    if( '\n' == *it ) break;
			}
		    }
		    else
		    {
			--m_col;
		    }
		}
		else
		{
		    m_col = 0;
		}
	    }
 	    return *this;
 	}

	/** Postfix decrement. It will not decrement to a point
	    before begin().
	*/
 	parse_iterator operator--(int)
 	{
	    return ( this->m_pos != this->m_beg )
		? (--(parse_iterator(*this)))
		: *this;
 	}

	/**
	   Returns a copy of the pointed-to char, or 0 if
	   eof().
	*/
	value_type operator*() const
	{
	    return (this->m_pos == this->m_end)
		? 0
		: *(this->m_pos);
	}

	/**
	   Returns true if rhs and this object point to the
	   same place.
	*/
	bool operator==( parse_iterator const & rhs ) const
	{
	    return (this == &rhs)
		|| (this->m_pos == rhs.m_pos);
	}
	/**
	   Returns true if rhs and this object do not point to the
	   same place.
	*/
	bool operator!=( parse_iterator const & rhs ) const
	{
	    return ! this->operator==( rhs );
	}
	/**
	   Returns true if this object's iterator compares less
	   than rhs's.
	*/
	bool operator<( parse_iterator const & rhs ) const
	{
	    return this->m_pos < rhs.m_pos;
	}

	/**
	   Returns the 1-based line number of this iterator. Counting starts
	   only at the begin iterator this object was initialized with, and is
	   updated via the ++/-- operators. If an itertor is used on a partial
	   range of input, the line/col numbers won't reflect those of the
	   whole input.
	*/
	int line() const { return m_line; }
	/**
	   Returns the 1-base column number of the input. When (*this == '\n')
	   then col() actually returns 0, as doing so simplifies the implementation
	   a good deal and seems to coincide with text editors which use row 1/col 1
	   as a starting point (as opposed to emacs, which uses row 1/col 0).
	   See line() for important caveats.
	*/
	int col() const { return m_col; }

    private:
	iterator m_beg;
	iterator m_pos;
	iterator m_end;
	int m_line;
	int m_col;
    };

    /***
	parser_state stores the "significant" information about a
	parsing run, where "significant" means significant to the
	parsepp internals.

	Important usage notes:

	- All std::strings used as input for this class MUST outlive
	the object using the string, as we continue to point to its
	contents for the life of the parser_state object. If the
	contents of the string changes sometime after this object is
	created then results of any operations in this class are
	undefined.

	- Many operations in this class use parse_iterators. For all
	of these operations EXCEPT the constructor and reset() it is
	illegal to use any parse_iterator object which did not
	originate from this object (e.g. by calling pos() or begin()).
    */
    class parser_state
    {
    public:

	/**
	   Initialized an empty object. It is illegal to use
	   any iteration operations on this object until
	   reset(...) is called to set the state.
	*/
	parser_state() : m_head(), m_it(), m_max(), m_errmsg(),m_gotos() {}

	/**
	   Sets this object's starting point. The point can be
	   advanced by calling operator++ and can be set with
	   pos(parse_iterator).

	   This function is one of the few for which it is legal to
	   pass an interator which originated from outside this
	   object.
	*/
	parser_state( parse_iterator const & start )
	    : m_head(start),m_it(start),m_max(start),m_errmsg(),m_gotos()
	{}

	/**
	   Uses in as input. See the class docs for important
	   information about the lifespan on if.
	*/
	explicit parser_state( std::string const & in )
	    : m_head(in),m_it(in),m_max(m_it),m_errmsg(),m_gotos()
	{}

	/**
	   Equivalent to the ctor(parse_iterator).
	*/
	void reset( parse_iterator const & start )
	{
	    m_head = m_it = m_max = start;
	}

 	/**
	   Equivalent to reset( parse_iterator(in) ).
	*/
	void reset( std::string const & in )
	{
	    return this->reset( parse_iterator(in) );
	}

 	/**
	   Equivalent to reset( begin() ).
	*/
	void reset()
	{
	    m_it = m_max = m_head;
	}

	/**
	   Returns the current iterator position.
	*/
	parse_iterator const & pos() const { return m_it; }

	/**
	   Returns the input's starting point. This is not necessarily
	   the start of the original source, only the start of where
	   this iterator was pointed to at construction (or via
	   copying).
	*/
	parse_iterator const & begin() const { return m_head; }

	/**
	   Returns the after-the-end iterator.
	*/
	//parse_iterator const & end() const { return m_head; }

	/**
	   Sets current position and returns
	   a copy of the old position.
	*/
	parse_iterator pos( parse_iterator const & newpos )
	{
	    parse_iterator old = m_it;
	    m_it = newpos;
	    this->checkmax();
	    return old;
	}

	/**
	   Equivalent to this->pos().eof().
	*/
	bool eof() const { return m_it.eof(); }

	/** Prefix increment. Avances the internal iterator by 1
	    then returns that value. */
 	parse_iterator & operator++()
 	{
	    ++m_it;
	    this->checkmax();
	    return m_it;
 	}

	/** Postfix increment. Advances the internal iterator by
	 one and returns the pre-incremented value.*/
 	parse_iterator operator++(int)
 	{
	    parse_iterator tmp = this->m_it++;
	    this->checkmax();
	    return tmp;
 	}

	/** Prefix decrement of the current position. */
 	parse_iterator & operator--()
 	{
	    return --m_it;
 	}

	/** Postfix decrement of the current position. */
 	parse_iterator operator--(int)
 	{
	    return m_it--;
 	}

	/**
	   Returns a copy of the currently pointed-to char, or 0 if
	   eof(). Equivalent to *(this->pos()).
	*/
	char operator*() const
	{
	    return *(this->m_it);
	}

	//! Don't use this.
	void add_error( std::string const & s )
	{
	    this->m_errmsg = s;
	}

	//! Don't use this.
	std::string errmsg() const
	{
	    return this->m_errmsg;
	}

	/**
	   Returns the highest traversed iterator. This is not
	   necessarily the same as pos(), and will always be
	   >= pos.
	*/
	parse_iterator maxpos() const
	{
	    return this->m_max;
	}

	/**
	   Sets a "goto point" in the parser, using the unique
	   id l. Overwrites any existing goto point.
	*/
	void label( int l, parse_iterator const & pos )
	{
	    m_gotos[l] = pos;
	}

	/**
	   Removes a label previously set with label.
	*/
	void remove_label( int l )
	{
	    m_gotos.erase(l);
	}

	/**
	   If l has been set via label(l) then this function
	   will move the iterator to that position. It returns
	   true if it moves the cursor, false if there is no such
	   label.
	*/
	bool goto_label( int l )
	{
	    marker_map::const_iterator it = m_gotos.find(l);
	    if ( it != m_gotos.end() )
	    {
		this->pos( (*it).second );
		return true;
	    }
	    return false;
	}

    private:
	parse_iterator m_head; // starting pos.
	parse_iterator m_it; // current pos
	parse_iterator m_max; // max pos we've ever reached (for use in error reporting)
	std::string m_errmsg;
	typedef std::map<int,parse_iterator> marker_map;
	marker_map m_gotos;
	//parser_state( parser_state const & );
	//parser_state operator=( parser_state const & );
	void checkmax()
	{
	    if( m_max < m_it ) m_max = m_it;
	}
    };


    void calc_line_col( parse_iterator pos,
			int & line, int & col );

    /**
       Base exception class used by this framework.

       Note that if the parser_state to which a parse_error applies
       goes out of scope during throw/catch unwinding, the pos() and
       state() members have undefined resuls.
    */
    class parse_error : public std::exception
    {
    private:
	void calcWhere()
	{
	    // these two options actually provide different column numbers:
#if 0
	    int line = 1, col = 0;
	    calc_line_col( m_pos, line, col );
#else
	    int line = m_pos.line();
	    int col = m_pos.col();
#endif
	    std::ostringstream os;
	    os << "[line "<<line<<" col "<<col<<']';
	    m_where = (os.str());
	}

    public:

	parse_error(parser_state const & st ) :
	    m_st(&st),
	    m_pos(st.pos()),
	    m_what(),
	    m_where()
	{
	    calcWhere();
	}
	parse_error(parser_state const & st, std::string const & w ) :
	    m_st(&st),
	    m_pos(st.pos()),
	    m_what(w),
	    m_where()
	{
	    calcWhere();
	}
	virtual ~parse_error() throw() {}
	virtual char const * what() const throw()
	{
	    return m_what.c_str();
	}
	parser_state const * state() const
	{ return this->m_st; }

	parse_iterator pos() const { return this->m_pos; }

	/**
	   Returns a best guess as to the position of the
	   error.
	*/
	std::string where() const throw()
	{
	    return m_where;
	}

    protected:
	parse_error(std::string const & w ) : m_st(0), m_what(w) {}
	void pos( parse_iterator const & p ) { this->m_pos = p; }

    private:
	parser_state const * m_st;
	parse_iterator m_pos;
	std::string m_what;
	std::string m_where;
    };


    /**
       This type is used to mark an iterator's position at
       construction and reset it at destruction. This is used
       to implement "non-consuming" rules which can jump back
       to a given point upon determining that they don't match
       quite as well as they thought.
    */
    class placemarker
    {
    private:
	parser_state * st;
	parse_iterator pos;
	/** Copy operations are not implemented. */
	placemarker( placemarker const & );
	/** Copy operations are not implemented. */
	placemarker & operator=( placemarker const & );
    public:
	/**
	   Stores a reference to s and a copy of s->pos().
	*/
	placemarker( parser_state & s ) : st(&s), pos(s.pos())
	{
	}
	/**
	   Stores a reference to s and a copy of pos.
	*/
	placemarker( parser_state & s, parse_iterator const & pos )
	    : st(&s), pos(pos)
	{
	}

	/**
	   If (b) then this function causes this object to not reset
	   its iterator at dtor-time, otherwise it does nothing.
	   Returns b.
	*/
	bool operator()( bool b )
	{
	    if( b && st ) { st = 0; }
	    return b;
	}
	/**
	   If operator()(true) has not been called then revert the
	   iterator to its starting position (the one passed to the
	   ctor).
	*/
	~placemarker() { if( st ) st->pos(pos); }
    };


    /**
       This type exists as a demonstration of what Rule types must
       look like, and to document the Rule API.
    */
    struct RuleConcept
    {
	/**
	   This typedef is used by some code to assist in picking
	   up desired template specializations. Top-level rules
	   (those which have no parent Rule type) must set this
	   typedef to their own type. e.g. if your type is called
	   myrule then do: typedef myrule type;

	   Rules which only subclass other rules, and have no
	   matches() implementation of their own need not
	   re-set this.

	   Another theoretical use of this typedef is fetching a
	   Rule's name using typeid(...).name(), to assist in error
	   reporting. Note, however, that typeid::name() officially
	   has unspecified results - it is legal for it to return an
	   empty string for every type.

	   Note that this is not multiple-inheritance friendly, but in
	   my experience parsers of this model don't use
	   multiple inheritance (nor do i think it'd be possible, with
	   this API, given the overload collisions on the matched()
	   members).
	*/
	typedef RuleConcept type;
	/**
	   The first arg is the parser state, which includes the
	   current input position. If this rule matches the input, it
	   should advance the iterator to one past the last-consumed
	   char, then return true.

	   If this Rule cannot match then it must not advance the
	   iterator, or it must restore it to its initial state before
	   returning. See the placemarker class for a simple way to
	   help ensure that policy.

	   Certain Rules may return true without advancing the
	   iterator, and some routines must take care to avoid endless
	   parse loops in such cases. Rules which don't consume should
	   be clearly documented as such.

	   The State object is the object passed by the client to
	   parse().
	*/
	template <typename ClientState>
	static bool matches( parser_state & , ClientState & );
    };

    /**
       This type only exists as a demonstration of
       what Action types must look like, and to document
       the Action API.

       When an Action is triggered by a given matching rule,
       Action::matched() is called, and passed the matching string and
       the client-provided State object. Actions typically update the
       ClientState object depending on the contents of the string.
       Note that some actions do not consume input, and they will
       cause an empty string to be passed to the action.
    */
    struct ActionConcept
    {
	/**
	   See the notes for RuleConcept::type.
	*/
	typedef ActionConcept type;
	/**
	   The second arg is the matching string which triggered this
	   action. The ClientState object is the object passed by the client
	   to parse().
	*/
	template <typename ClientState>
	static void matched( parser_state & , std::string const &, ClientState & );
    };

    /**
       This is a front-end to Rule::matches().

       If this function returns false then the input iterator is moved
       back to its starting position (i.e. no input is consumed).

       The ClientState object is passed on as-is to the various
       sub-rules and actions. It is legal to pass a bogus state
       object, provided that no actions are invoked which need a
       specific ClientState type.

       Note that Rules which explicitly call other rules should NEVER
       call parse() directly - they should call SubRule::matches()
       instead. The reason is that this function may introduce
       instrumentation which might affect the performance (or
       behaviour) of rules. It does not currently do so (other than
       using a placemarker), but its specification permits it to do
       things like catch certain exceptions and such.
    */
    template <typename Rule, typename ClientState>
    bool parse( parser_state & in, ClientState & st )
    {
	placemarker sentry(in);
	return sentry( Rule::matches(in, st ) );
    }

    /**
       Convenience overload. The source input pointed to by in
       iterator must obey the lifespan rules of the
       parse_iterator(string) ctor. See that function for important
       detail regarding the input string's lifespan and immutability.
    */
    template <typename Rule, typename ClientState>
    bool parse( parse_iterator const & in, ClientState & st)
    {
	try
	{
	    parser_state ps(in);
	    return parse<Rule>( ps, st );
	}
	catch( parse_error const & ex )
	{
	    // recreate exception without the state,
	    // which won't be valid once this function returns.
	    std::string msg( ex.what() );
	    msg += "\n Near "+ex.where();
	    throw std::runtime_error( msg );
	}
	return false;
    }

    /**
       Convenience overload. See the parse_iterator(string) ctor for
       important detail regarding the input string's lifespan and
       immutability.
    */
    template <typename Rule, typename ClientState>
    bool parse( std::string const & in, ClientState & st)
    {
	parse_iterator it(in);
	return parse<Rule>( it, st );
    }

    /**
       A non-consuming parser which returns Val.
    */
    template< bool Val >
    struct r_bool
    {
	typedef r_bool type;
	template <typename ClientState>
	inline static bool matches( parser_state &, ClientState & )
	{
	    return Val;
	}
    };

    /** Always matches. Consumes no input. */
    struct r_success : r_bool<true> {};

    /** Always fails to match. Consumes no input. */
    struct r_failure : r_bool<false> {};

    /**
       Rule matching (std::string().end() == in)
       or (!*in). It does not consume any input.
    */
    struct r_eof
    {
	typedef r_eof type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & )
	{
	    return in.eof();
	}
    };

    namespace Detail {
	using namespace Ps;
	/** Internal implementation of r_seq<>. */
	template <bool FailFast, typename Rule>
	struct r_seq_impl
	{
	    typedef r_seq_impl type;
	    template <typename ClientState>
	    inline static bool matches( parser_state & in, ClientState & st )
	    {
		return Rule::matches( in, st );
	    }
	};
	/** Internal implementation of r_seq<>. */
	template <bool FailFast>
	struct r_seq_impl<FailFast,nil_t>
	{
	    typedef r_seq_impl type;
	    template <typename ClientState>
	    inline static bool matches( parser_state &, ClientState & )
	    {
		return FailFast;
	    }
	};
	/** Internal implementation of r_seq<>. */
	template <bool FailFast, typename Head, typename Tail>
	struct r_seq_impl< FailFast, rule_list_base< Head, Tail > >
	{
	    typedef r_seq_impl type;
	    typedef rule_list_base< Head, Tail > list_type;
	    template <typename ClientState>
	    inline static bool matches( parser_state & in, ClientState & st )
	    {
		typedef typename list_type::head::type R1;
		typedef typename list_type::tail::type R2;
		return FailFast
		    ? (r_seq_impl<FailFast,R1>::matches( in, st )
		       && r_seq_impl<FailFast,R2>::matches( in, st ))
		    : (r_seq_impl<FailFast,R1>::matches( in, st )
		       || r_seq_impl<FailFast,R2>::matches( in, st ))
		    ;
	    }
	};
    }

    /**
       RuleList must be of the type rule_list<> (or strictly
       compatible), and it must be templatized on any number of Rule
       types.

       For each Rule in RuleList, Rule::matches()
       is called. If it succeeds, true is returned
       and this function consumes input if the Rule
       consumes. If the Rule fails, then how we proceed
       depends on the FailFast flag:

       - FailFast == true: stop processing and return false.

       - FailFast == false: try the next Rule in the list.

       Thus, FailFast==true implements an AND operation and
       FailFast==false implements an OR operation.

       An empty RuleList always evaluates to false.
    */
    template <bool FailFast, typename RuleList>
    struct r_seq
    {
	typedef r_seq type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    /* This RealList thing allows r_seq_impl<rule_list<>> to
	       overload as if it were a r_seq_impl<rule_list_base<>>, which
	       can simplify grammar implementations. If we didn't do
	       this, we would need to add an r_seq_impl specialization
	       for each rule_list<> type. */
	    typedef typename RuleList::type RealList;
	    placemarker sentry(in);
	    return sentry( Detail::r_seq_impl<FailFast,RealList>::matches( in, st ) );
	}
    };

    /**
       Convenience form of r_seq: OR operation on a list of Rule.
    */
    template <typename RuleList>
    struct r_or : r_seq<false, RuleList> {};

    /**
       Convenience form of r_seq: AND operation on a list of Rule.
    */
    template <typename RuleList>
    struct r_and : r_seq<true, RuleList> {};

    /**
       A rule which, if it matches, calls Action::matched(in,string,State),
       where string is the part(s) matched by Rule::matches().
    */
    template <typename Rule, typename Action>
    struct r_action
    {
	typedef r_action type;
	/**
	   If Rule::matches() then Action::matched() is called
	   with the matching string and the state.

	   Note that when used in conjunction with non-consuming
	   parsers (e.g. r_at<>) then it may pass empty strings
	   to the Action.
	*/
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    parse_iterator start(in.pos());
	    placemarker sentry(in);
	    if( ! sentry(Rule::matches( in, st )) ) return false;
	    parse_iterator pos( in.pos() );
	    Action::matched( in, std::string(start,pos), st );
	    return true;
	}
    };


    namespace Detail {
	using namespace Ps;
	/** Internal implementation detail of a_actions_impl<>. */
	template <typename Action>
	struct a_actions_impl
	{
	    template <typename ClientState>
	    inline static void matched( parser_state & in, std::string const & m, ClientState & st )
	    {
		Action::matched( in, m, st );
	    }
	};

	/** Internal implementation detail of of a_actions_impl<>. */
	template <>
	struct a_actions_impl<nil_t>
	{
	    typedef a_actions_impl type;
	    template <typename ClientState>
	    inline static bool matched( parser_state &, std::string const & m, ClientState & )
	    {
		return;
	    }
	};

	/** Internal implementation of a_actions_impl<>. */
	template <typename Head, typename Tail>
	struct a_actions_impl< rule_list_base< Head, Tail > >
	{
	    typedef a_actions_impl type;
	    typedef rule_list_base< Head, Tail > list_type;
	    template <typename ClientState>
	    inline static void matched( parser_state & in, std::string const & m, ClientState & st )
	    {
		Head::matched( in, m, st );
		//typedef typename list_type::head::type A1;
		typedef typename list_type::tail::type A2;
		a_actions_impl<Head>::matched( in, m, st );
		a_actions_impl<A2>::matched( in, m, st );
	    }
	};
    }

    /**
       a_actions is an Action proxy which forwards a match to an arbitrary number
       of Actions. ActionList must be of type rule_list<Action1,...ActionN>, and
       each contained type must conform to the ActionConcept iterface.

       Example usage:

       <code>r_action< SomeRule, a_actions< rule_list<MyAction, MyOtherAction> > ></code>
    */
    template <typename ActionList>
    struct a_actions
    {
	template <typename ClientState>
	static void matched( parser_state & in, std::string const & m, ClientState & st )
	{
	    typedef typename ActionList::type ListType;
	    Detail::a_actions_impl<ListType>::matched( in, m, st );
	}

    };


    /**
       Works like Rule, but does not consume any input - it jumps back
       to its starting point when complete. Note that actions trigged
       by Rule cannot be undone if Rule fails after triggering some
       embedded action.
    */
    template <typename Rule>
    struct r_at
    {
	typedef r_at type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    placemarker sentry(in);
	    return Rule::matches( in, st );
	}
    };

    /**
       The negated form of r_at<>. Does not consume any input.
    */
    template <typename Rule>
    struct r_notat
    {
	typedef r_notat type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    placemarker sentry(in);
	    return ! Rule::matches( in, st );
	}
    };

    /**
       Matches on eof or if Rule matches. It always returns true, but
       only advances the input if Rule consumes and we're not at
       eof. Note that actions triggered as part of Rule cannot be
       un-done if the rule later fails to match and the input is
       rewound.

       Note that a break_exception thrown from Rule
       will propagate out of this class, which means
       that a Break changes the meaning of "optional"
       (because "break" trumps "optional").
    */
    template <typename Rule>
    struct r_opt
    {
	typedef r_opt type;
	template <typename ClientState>
	inline static bool matches( parser_state &in, ClientState & st )
	{
	    if( in.eof() ) return true;
	    placemarker sentry(in);
	    sentry( Rule::matches(in,st) );
	    return true;
	}
    };

    /**
       Matches (Rule+). If Rule matches but does not consume input,
       this rule will stop to avoid an endless loop.
    */
    template <typename Rule>
    struct r_plus
    {
	typedef r_plus type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    parse_iterator pos(in.pos());
	    placemarker sentry(in);
	    if( ! Rule::matches( in, st ) ) return sentry(false);
	    if( pos == in.pos() ) return sentry(true); // did not consume
	    while( Rule::matches( in, st ) )
	    {
 		if( in.eof() || (pos==in.pos()) )
 		{ // if match did not consume tokens we need to stop to avoid an endless loop.
 		    break;
 		}
 		pos = in.pos();
	    }
	    return sentry(true);
	}
    };

    /**
       Matches (Rule*). If Rule matches but does not consume input,
       this rule will stop to avoid an endless loop.
    */
    template <typename Rule>
    struct r_star
    {
	typedef r_star type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    // Reminder to self: no placemarker here!
	    parse_iterator pos(in.pos());
	    while( Rule::matches( in, st ) )
	    {
 		if( in.eof() || (in.pos() == pos) )
 		{// if match did not consume tokens we need to stop to avoid an endless loop
 		    break;
 		}
 		pos = in.pos();
	    }
	    return true;
	}
    };

    /**
       Matches Rule at least Min times and at most Max times. If it
       does not match it does not consume, but if forward parsing
       caused Actions to be triggered then they are not un-done by
       rewinding.

       Note that once Max is reached, checking stops. That means
       that Max is not a hard-limit - there may be more than Max
       matches in the input.
    */
    template< typename Rule, unsigned int Min, unsigned int Max = Min >
    struct r_repeat
    {
	typedef r_repeat type;
	template <typename ClientState>
	static bool matches( parser_state & in, ClientState & st )
	{
	    assert( (Min<=Max) && "r_repeat<Min,Max>: Min>Max" );
	    assert( (Min!=0) && "r_repeat<Min,Max>: Min==0" );
	    placemarker sentry(in);
	    parse_iterator pos(in.pos());
	    size_t count = 0;
	    while( Rule::matches( in, st ) )
	    {
		if( (++count == Max )
		    || in.eof()
		    || (pos == in.pos()) // did not consume
		    ) break;
	    }
	    return sentry( (count >= Min) && (count <= Max) );
	}
    };



    /**
       "Pads" Rule with the given left/right rules. This is normally
       used to skip extra whitespace.

       Equivalent to ((RuleLeft*) && Rule && (RuleRight*)).
    */
    template <typename Rule, typename RuleLeft, typename RuleRight = RuleLeft>
    struct r_pad : r_and< rule_list< r_star<RuleLeft>, Rule, r_star<RuleRight> > >
    {};

    /**
       A Rule which advances the input iterator by Count steps. It
       fails only if it reaches eof before advancing the full
       amount. Note that it is possible for the input to be advanced
       exactly to eof and this Rule still return true. That would
       happen if, e.g., r_advance<3> is used when the input has
       exactly 3 chars left.

       This type uses an iterator sentry, so it is not as efficient as
       using r_any for short sequences.
    */
    template <int Count>
    struct r_advance
    {
	typedef r_advance type;
	template <typename State>
	static bool matches( parser_state & in, State & )
	{
	    placemarker sentry(in);
	    int i = 0;
	    for( ; i < Count; ++i )
	    {
		if(in.eof()) break;
		++in;
	    }
	    return sentry( i == Count );
	}
    };

    /**
       Consumes no input and throws an ExceptionType.
    */
    template <typename ExceptionType>
    struct r_throw_base
    {
	typedef r_throw_base base;
	template <typename State>
	static bool matches( parser_state &, State & )
	{
            throw ExceptionType();
	}
    };

    /**
       Specialized to ensure the error point is marked.
    */
    template <>
    struct r_throw_base<parse_error>
    {
	typedef r_throw_base type;
	template <typename State>
	static bool matches( parser_state & in, State & )
	{
            throw parse_error(in, "parse error triggered by r_throw" );
	    return false;
	}
    };

    typedef r_throw_base<parse_error> r_throw;



    /**
       Matches any characters in the range [Min..Max]
    */
    template< int Min, int Max >
    struct r_range
    {
	typedef r_range type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & )
	{
	    assert( (Min<=Max) && "r_range<Min,Max>: Min>Max" );
	    if( ! in.eof() )
	    {
		if( (*in >= Min) && (*in <= Max) )
		{
		    ++in;
		    return true;
		}
	    }
	    return false;
	}
    };

    /**
       Matches only the character CH. If CaseSensitive is true then CH
       must match exactly, otherwise the upper- or lower-case form of
       CH will also match. Also, if CaseSensitive is true then CH MUST
       be in the range [a-zA-Z], or results are undefined.
    */
    template< bool CaseSensitive, char_type CH >
    struct r_ch_base
    {
	typedef r_ch_base type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & )
	{
	    if( CaseSensitive )
	    {
		return (*in == CH)
		    ? (++in,true)
		    : false;
	    }
	    else
	    {
		int ch = *in;
		assert( (((CH>='a')&&(CH<='z'))
			 || ((CH>='A')&&(CH<='Z')))
			&& "r_ch_base<false,char> CH value is not alphabetic." );
		return ( (ch==CH) || (ch == ((CH>='a') ? (CH-32) : (CH+32))) )
		    ? (++in,true)
		    : false;
	    }
	}
    };


    /**
       Matches only CH.
    */
    template< char_type CH >
    struct r_ch : r_ch_base<true,CH> {};
    /**
       Matches CH, case-insensitively. CH *must* be in the range
       [a-zA-Z] or a static assertion will fail.
    */
    template< char_type CH >
    struct r_chi : r_ch_base<false,CH> {};

    /**
       Matches any character except CH.
    */
    template< char_type CH >
    struct r_notch
    {
	typedef r_notch type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & )
	{
 	    return (*in != CH)
 		? (++in,true)
 		: false;
	}
    };

    /**
       Matches any character except CH, case-insensitively.
    */
    template< char_type CH >
    struct r_notchi
    {
	typedef r_notchi type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    placemarker sentry(in);
	    return sentry( ! r_chi<CH>::matches(in,st) );
	}
    };

    /**
       Matches any one char except eof.
    */
    struct r_any
    {
	typedef r_any type;
	template <typename ClientState>
	inline static bool matches( parser_state &in, ClientState & )
	{
	    return in.eof()
		? false
		: (++in,true);
	}
    };

    /**
       Matches [A-Z].
    */
    struct r_upper :
	r_range<'A','Z'>
    {};
    /**
       Matches [a-z].
    */
    struct r_lower :
	r_range<'a','z'>
    {};
    /**
       Matches [a-zA-Z].
    */
    struct r_alpha :
	r_or< rule_list< r_lower, r_upper > >
    {};
    /**
       Matches [0-9].
    */
    struct r_digit :
	r_range<'0','9'>
    {};

    /**
       Matches [a-zA-Z0-9].
    */
    struct r_alnum : r_or< rule_list<r_digit, r_alpha> >
    {};

    /**
       Matches [0-9a-fA-F].
    */
    struct r_xdigit : r_or< rule_list< r_digit, r_range<'a','f'>, r_range<'A','F'> > >
    {};

    /**
       Matches common programming-language identifiers (e.g. var and
       function names). That is, starting with alpha or underscore,
       followed by any number of alphanumeric or underscore chars.
    */
    struct r_identifier :
	r_and< rule_list< r_or< rule_list< r_ch<'_'>, r_alpha > >,
			  r_star< r_or< rule_list<r_ch<'_'>, r_alpha, r_digit > > > >
	>
    {};


    namespace Detail {
	using namespace Ps;
	/** Internal implementation detail of r_chseq_base<>. */
	template <bool FailFast, bool CaseSensitive, typename Rule>
	struct r_chseq_impl;

	/** Internal implementation detail of of r_chseq_base<>. */
	template <bool FailFast, bool CaseSensitive>
	struct r_chseq_impl<FailFast,CaseSensitive,nil_t>
	{
	    typedef r_chseq_impl type;
	    template <typename ClientState>
	    inline static bool matches( parser_state &, ClientState & )
	    {
		return FailFast;
	    }
	};

	/** Internal implementation of r_chseq_base<>. */
	template <bool FailFast, bool CaseSensitive, char_type Head, typename Tail>
	struct r_chseq_impl< FailFast, CaseSensitive, char_list_base< Head, Tail > >
	{
	    typedef r_chseq_impl type;
	    typedef char_list_base< Head, Tail > list_type;
	    template <typename ClientState>
	    inline static bool matches( parser_state & in, ClientState & st )
	    {
		typedef typename list_type::tail::type tail;
		return FailFast
		    ? (r_ch_base<CaseSensitive,Head>::matches( in, st )
		       && r_chseq_impl<FailFast,CaseSensitive,tail>::matches( in, st ))
		    : (r_ch_base<CaseSensitive,Head>::matches( in, st )
		       || r_chseq_impl<FailFast,CaseSensitive,tail>::matches( in, st ))
		    ;
	    }
	};
    }

    /**
       Works like r_seq<FailFast,RuleList>, except that it takes a
       CharList, which must conform to the char_list<> interface.

       If FailFast is true, matching stops at the first failure.
       If FailFast is false, matching stops at the first success.
       This (FailFast==true) is an AND operation and (FailFast==false)
       is an OR operation.

       If CaseSensitive is true then this rule matches if all
       characters in CharList match (in sequence). If CaseSensitive is
       false then all chars in CharList must be in the range [a-zA-Z]
       and are matched case-insensitively.
    */
    template <bool FailFast, bool CaseSensitive, typename CharList>
    struct r_chseq_base
    {
	typedef r_chseq_base type;
	template <typename ClientState>
	inline static bool matches( parser_state & in, ClientState & st )
	{
	    /* This RealList thing allows r_chseq_impl<char_list<>> to
	       overload as if it were a
	       r_chseq_impl<char_list_base<>>, which can simplify
	       grammar implementations. If we didn't do this, we would
	       need to add an r_chseq_impl specialization for each
	       char_list<> type. i think. */
	    typedef typename CharList::type RealList;
	    placemarker sentry(in);
	    return sentry( Detail::r_chseq_impl<FailFast,CaseSensitive,RealList>::matches( in, st ) );
	}
    };

    /**
       Matches a character sequence, case-sensitively.
    */
    template <typename CharList>
    struct r_chseq : r_chseq_base<true,true,CharList> {};

    /**
       Matches a character sequence, case-insensitively.
    */
    template <typename CharList>
    struct r_chseqi : r_chseq_base<true,false,CharList> {};

    /**
       Matches any single character in CharList, case-sensitively.
    */
    template <typename CharList>
    struct r_oneof : r_chseq_base<false,true,CharList> {};

    /**
       Matches any single character in CharList, case-insensitively.
    */
    template <typename CharList>
    struct r_oneofi : r_chseq_base<false,false,CharList> {};

    /**
       Matches space or tab.
    */
    struct r_blank :
	r_oneof< char_list< ' ', '\t' > >
    {};

    /**
       Matches space, [vertical] tab, CR, newline, and form feed.
    */
    struct r_space :
	r_oneof< char_list< ' ', '\n', '\t', '\r', '\f', '\v' > >
    {};

    /**
       Matches if Rule matches, skipping any amount of leading or
       trailing whitespace.
    */
    template <typename Rule>
    struct r_skipws : r_pad<Rule,r_space,r_space> {};

    /**
       Matches until the first space character. If it is not at
       a space character, it does not consume but still succeeds.
    */
    struct r_tospace : r_star<
	r_and< rule_list< r_notat< r_space >, r_advance<1> > >
	>
    {};

    /**
       Matches if Rule matches, skipping any amount of leading or
       trailing blanks (space or tab).
    */
    template <typename Rule>
    struct r_skipblanks : r_pad<Rule,r_blank,r_blank> {};

    /**
       Matches until the first blank character.
    */
    struct r_toblank : r_star<
	r_and< rule_list< r_notat< r_blank >, r_advance<1> > >
	>
    {};

    /**
       Matches the combination \r\n or a single \n.
    */
    struct r_eol :
	r_or< rule_list< r_chseq< char_list<'\r','\n'> >, r_ch<'\n'> > >
    {};


    /** Parser for C++-style comments. */
    struct r_comment_cpp :
	r_and< rule_list<
	    r_chseq< char_list<'/','/'> >,
	    r_star< r_notch<'\n'> >,
	    r_eol
	    > >
    {};

    template <typename R>
    struct r_throw_if : r_and< rule_list<
	r_and< rule_list<R,r_throw> >,
	r_success > >
    {};

    namespace Detail
    {
	/** Inner part of a C++ comment. Consumes until '*' followed by '/'. */
	struct r_comment_c_inner : r_and< rule_list<
	    r_star< r_and< rule_list<
		r_notat< r_chseq< char_list<'*','/'> > >,
		r_advance<1>
		> > >,
	    r_throw_if< r_eof >
	> >
	{};
    }
    /** Parser for C-style comments. */
    struct r_comment_c :
	r_and< rule_list<
	    r_chseq< char_list<'/','*'> >,
	    Detail::r_comment_c_inner,
	    r_chseq< char_list<'*','/'> >
	    > >
    {};



    namespace Detail {
	using namespace Ps;
	struct line_col_state
	{
	    int line;
	    int col;
	    line_col_state() : line(1),col(0){}
	};
	struct calc_inc_line
	{
	    static void matched( parser_state &,
				 const std::string &,
				 line_col_state & state )
	    {
		++state.line;
		state.col = 0;
	    }
	};
	struct calc_inc_col
	{
	    static void matched( parser_state &,
				 const std::string &,
				 line_col_state & state )
	    {
		++state.col;
	    }
	};
	struct r_linecol :
	    r_star< r_or< rule_list<
	    r_action< r_eol, calc_inc_line >,
	    r_action< r_any, calc_inc_col >
	    > > >
	{};
    }


    /**
       Walks parse_iterator until its end end. For each newline it
       finds, line is incremented by one and col is set to 1,
       otherwise col is incremented by 1. line and col are initialized
       by this function.

       Column numbers start at 0 and line numbers start at 1 (because
       emacs does it that way).
    */
    inline void calc_line_col( parse_iterator pos,
			       int & line, int & col )
    {
	line = 1; col = 0;
	parse_iterator inp( pos.begin(), pos.iter() );
	Detail::line_col_state st;
	parse<Detail::r_linecol>( inp, st );
	line = st.line;
	col = st.col;
    }
} // namespace

#endif // s11n_net_PARSEPP_HPP_INCLUDED
