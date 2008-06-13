#ifndef S11N_NET_PARSEPP_NUMERIC_HPP_INCLUDED
#define S11N_NET_PARSEPP_NUMERIC_HPP_INCLUDED
#include <cerrno>
#include <cstdlib>
namespace Ps {
/**
   The numeric namespace contains parsers for numbers in the forms:

   Integers:

   - Decimal: [+-][0-9]+

   - Hex: 0[xX][0-9a-fA-F]+

   - Octal: 0[0-7]+

   Doubles: [-+](Decimal.[0-9]+)
   

   It does right-side checking, such that if a number is adjacent to
   an invalid character then it will fail to parse. e.g. (-38a) will
   fail to parse, as will (-1.2.).

   The list of illegal tailing chars is: (alphanum, '_', '.'). Other
   chars are not considered illegal because they might have
   grammar-specific meanings, whereas that list applies to the vast
   majority of grammars.
*/
namespace numeric {
    using namespace Ps;

    /**
       Intended for use with parse_number, it holds the result
       of a parse.
    */
    struct number_state
    {
	enum Types {
	Unknown,
	Double,
	Integer,
	OctalInteger,
	HexInteger,
	};
	typedef long double value_type;
	/**
	   The numeric type (from the Types enum).
	*/
	Types type;
	value_type value;
	std::string valstr;
	number_state() : type(Unknown),value(),valstr() {}
    };

    /**
       An Action to convert a match into a number and set the State's
       value to that number.
    */
    template <number_state::Types NumType>
    struct a_setval
    {
	static void matched( parser_state &ps, const std::string & m, number_state & s )
	{
	    s.type = NumType;
	    s.valstr = m;
	    char const * chp = s.valstr.c_str();
	    char * end = 0;
	    errno = 0;
	    if( NumType == number_state::Double )
	    {
		s.value = std::strtold( chp, &end );
	    }
	    else
	    {
		s.value = std::strtoll( chp, &end, 0 );
	    }
	    // In theory the parsers will only pass us legal, convertible strings,
	    // but i'm feeling especially pedantic today:
	    if( end && *end )
	    {
		throw parse_error( ps,"a_setval<>: strtoXX() cannot parse input ["+s.valstr+"]" );
	    }
	    if( ERANGE == errno )
	    {
		throw parse_error( ps,"a_setval<>: number would over/underflow: ["+s.valstr+"]" );
	    }
	    if( EINVAL == errno )
	    {
		throw parse_error( ps,"a_setval<>: invalid number: ["+s.valstr+"]" );
	    }
	}
    };

    struct a_setint : a_setval<number_state::Integer> {};
    struct a_setdbl : a_setval<number_state::Double> {};
    struct a_sethex : a_setval<number_state::OctalInteger> {};
    struct a_setoct : a_setval<number_state::HexInteger> {};

    typedef r_oneof< char_list<'+','-'> > r_sign;
    typedef r_ch<'.'> r_dot;

    struct r_illegaltail : r_or< rule_list< r_alpha, r_oneof< char_list<'_','.'> > > >
    {};

    struct r_double : r_and< rule_list< r_opt<r_sign>,
			     r_plus<r_digit>, r_dot, r_plus<r_digit>,
			     r_notat< r_illegaltail > > >
    {
	/** Action to convert a match to an actual number. */
	typedef a_setdbl action;
    };

    /**
       Matches a floating-point number, requring a leading 0 if the
       number is less than 1.  It updates the State on a match.
    */
    struct read_double :
	r_action< r_double,
		  r_double::action >
    {};

    struct r_hexprefix : r_and< rule_list< r_ch<'0'>, r_chi<'x'> > > {};
    struct r_hexdigits : r_plus<r_xdigit>
    {};

    /**
       Matches a hex literal in the form (0[xX][a-fA-F0-9]+).
    */
    struct r_hexnum : r_and< rule_list< r_hexprefix, r_hexdigits,
					r_notat<r_illegaltail> > >
    {
	typedef a_sethex action;
    };

    /**
       Updates the State if r_hexnum matches.
    */
    struct read_hexnum :
	r_action< r_hexnum, r_hexnum::action >
    {};

    /**
       Matches an octal literal in the form (0[0-7]+).
    */
    struct r_octnum : r_and< rule_list< r_ch<'0'>, r_plus< r_range<'0','7'> >,
					r_notat< r_or< rule_list< r_oneof< char_list<'8','9'> >,r_illegaltail> > > > >
    {
	/** Action to convert a match to an actual number. */
	typedef a_setoct action;
    };


    /**
       Updates the State on a match of r_octnum.
    */
    struct read_octnum :
	r_action< r_octnum, r_octnum::action >
    {};

    /**
       Matches a literal integer in the form ([-+][0-9]+).
    */
    struct r_int :
	r_and< rule_list< r_opt<r_sign>, r_plus<r_digit>, r_notat<r_illegaltail> > >
    {
	/** Action to convert a match to an actual number. */
	typedef a_setint action;
    };

    /**
       Updates the state if r_int matches.
    */
    struct read_int : r_action<r_int,r_int::action> {};

    /**
       Parses a double or an integer (optionally signed), or
       an unsigned hex code starting with 0x (case-insenstive),
       or an octal literal starting with 0.

       As State it requires a number_state object. On a successful
       match, the State object is modified like so:

       state.value = value of number (as a double)

       state.valstr = string value of number, which might be different
       from state.value (e.g. if this parser handled integer hex codes
       or if the parse succeeds on a very large number).
    */
    struct parse_number :
	r_or< rule_list< read_hexnum, read_octnum, read_double, read_int > >
    {};

}} // namespaces
#endif // S11N_NET_PARSEPP_NUMERIC_HPP_INCLUDED
