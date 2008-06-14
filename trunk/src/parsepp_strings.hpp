#ifndef S11N_NET_PARSEPP_STRINGS_HPP_INCLUDED
#define S11N_NET_PARSEPP_STRINGS_HPP_INCLUDED

#define RL rule_list
#define CL char_list
namespace Ps {

    /**
       StringsForwarder is a Concept type which exists for
       demonstrating the interface required by the strings<> class.
       See StringsForwarderString for a sample implementation.
    */
    template <typename ClientState>
    struct StringsForwarder
    {
	/**
	   Signals the start of a read-quoted-string operation.
	   The client should normally empty any existing string buffer
	   at this point, to prepare for more input.
	*/
	void start_string( ClientState & );
	/**
	   Signals the appending of one or more chars to a parsed string.
	   The client should simply append m to his input buffer.
	*/
	void append( ClientState &st, std::string const & m );
	/**
	   Signals the end of a string parse. The client is not normally
	   required to do anything here, but can use this to trigger
	   actions which are waiting on the end of the string.
	*/
	void end_string( ClientState & );
    };

    /**
       A StringsForwarder type which uses a std::string as its State.
    */
    struct StringsForwarderString
    {
	/**
	   Signals the start of a read-quoted-string operation.
	   The client should normally empty any existing string buffer
	   at this point.
	*/
	inline void start_string( std::string & st )
	{
	    st.clear();
	}
	/**
	   Signals the appending of one or more chars to a parsed string.
	   The client should simply append m to his input buffer.
	*/
	inline void append( std::string &st, std::string const & m )
	{
	    st += m;
	}
	/**
	   Signals the end of a string parse. The client is not normally
	   required to do anything here, but can use this to trigger
	   actions which are waiting on the end of the string.
	*/
	inline void end_string( std::string & )
	{
	}
    };

    /**
       Rules for parsing conventional quoted strings (single- or
       double-quoted). It supports common C-style escape sequences in
       its input. TransFunc must conform to the StringsForwarder
       interface.
    */
    template <typename TransFunc>
    struct strings {
	//! Starts reading of a string.
	struct op_startstring
	{
	    template< typename State >
	    static void matched( parser_state &, const std::string &, State & s)
	    {
		//COUT << "start "<<CH<<"-style string\n";
		TransFunc().start_string(s);
	    }
	};

	//! Appends a char to a string.
	struct op_appendchar
	{
	    template< typename State >
	    static void matched( parser_state &, const std::string & m, State & s )
	    {
		if( m.empty() ) return; // happens on non-consuming actions
		TransFunc().append(s,m);
	    }
	};

	//! Appends an escaped char (unescaped) to a string.
	template <char CH>
	struct op_appendesc
	{
	    template< typename State >
	    static void matched( parser_state &, const std::string &, State & st )
	    {
		std::string ch;
		ch.push_back(CH);
		TransFunc().append(st,ch);
	    }
	};

	//! Ends reading of a string.
	struct op_endstring
	{
	    template< typename State >
	    static void matched( parser_state &, const std::string &, State & st )
	    {
		TransFunc().end_string(st);
	    }
	};



	//! Reads an escape sequence. QC == QuoteChar (either " or ')
	template <char QC>
	struct read_escchar :
	    r_and< RL<
		r_ch<'\\'>,
		r_or< RL< r_action< r_ch<QC>, op_appendesc<QC> >,
			  r_action< r_ch<'\\'>, op_appendesc<'\\'> >,
			  r_action< r_ch<'t'>, op_appendesc<'\t'> >,
			  r_action< r_ch<'b'>, op_appendesc<'\b'> >,
			  r_action< r_ch<'n'>, op_appendesc<'\n'> >,
			  r_action< r_ch<'r'>, op_appendesc<'\r'> >,
			  r_action< r_ch<'v'>, op_appendesc<'\v'> > > >
		> >
	{};



	//! Reads a single char or known escape sequence for a string.
	template <char QC>
	struct read_char :
	    r_or< RL<
		read_escchar<QC>, r_action< r_notch<QC>, op_appendchar >
		> >
	    //r_action< r_action<r_not<r_ch<QC>>,op_dump>, op_appendchar >
	    //r_action< r_action<r_any,op_dump>, op_appendchar >
	    //r_action< r_action<r_not<r_oneof<'x','y','n'>>,op_dump>, op_appendchar >
	    //r_action< r_action<r_notch<QC>,op_dump>, op_appendchar >
	{};

	/**
	   Reads a string enclosed in QC characters. QC should be one of ' or "
	   and if that character is in the string content, it must be escaped
	   with a backslash character.

	   At the opening quote the State is cleared. Characters are
	   appended until an unescaped quote char is found, then the State
	   is "finalized". If the rule does not match, State.str will not
	   be modified, but State.buf might be (containing content up until
	   the point of the parse error).
	*/
	template <char QC>
	struct read_qstring :
	    r_and< RL<
		r_action< r_ch<QC>, op_startstring >,
		r_star< read_char<QC> >,
		r_action< r_ch<QC>, op_endstring >
		> >
	{};

	//! Matches a double-quoted string.
	struct read_dqstring : read_qstring<'"'> {};
	//! Matches a single-quoted string.
	struct read_sqstring : read_qstring<'\''> {};

	/**
	   This is the parser starting point for this parser.

	   Matches single- or double-quoted strings.  It expects a
	   std::string State object, to which it writes the output
	   (overwriting any existing content). This routine unescapes
	   common C-style escape constructs when building the target
	   string.
	*/
	struct start :
	    r_or< RL< read_dqstring, read_sqstring > >
	{};

    }; // strings

    /**
       A convenience function to use the strings<> parser to parse a
       quoted string. It reads from src, which is expected to be
       string data (possibly escaped using C-style escaped) enclosed
       in either single or double quotes. If the parse is successful
       true is returned. If it fails
       to parse then false is returned.

       tgt is overwritten with the unescaped content of the string
       (minus the enclosing quotes). In the case of a parse failure,
       the state of tgt is undefined - it will likely hold the chars
       of the string up until the point where the parse failed, which
       might be useful in debugging the problem.
    */
    inline bool parse_qstring(  std::string const & src, std::string & tgt )
    {
	return parse< strings<StringsForwarderString>::start >( src, tgt );
    }

} // namespaces
#undef RL
#undef CL
#endif // S11N_NET_PARSEPP_STRINGS_HPP_INCLUDED
