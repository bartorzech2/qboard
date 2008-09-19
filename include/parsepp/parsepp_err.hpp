#ifndef s11n_net_PARSEPP_ERRORS_HPP_INCLUDED
#define s11n_net_PARSEPP_ERRORS_HPP_INCLUDED
/************************************************************************
This file contains supplemental error-handling-related code for the
parsepp toolkit.

Author: Stephan Beal (http://wanderinghorse.net/home/stephan)
License: Public Domain
************************************************************************/
#include <string>
#include <sstream>
#include <stdexcept>

#include "parsepp.hpp"
#include "parsepp_typelist.hpp"

namespace Ps {
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
       A list of "standard" error IDs, for use with the error_msg<>
       template. Client-side parsers which use r_error, a_error,
       r_error_if, r_error_unless and related functions may specialize
       error_msg<int> with their own error number values. Clients
       should not use values less than UserErrorBegin. The values need
       to be unique within the context of an application (including
       its libraries). Thus two parsers may use the same unique IDs if
       there is no chance that they will end up in the same library.
    */
    struct Errors
    {
	enum IDs {
	Unknown = 0,
	UnexpectedCharacter,
	IllegalCharacter,
	UnexpectedEOF,
	UnclosedComment,
	UserErrorBegin = 1000 /* client-side IDs should start here. */
	};
    };

    /**
       A template for mapping parser error codes to strings.

       Specializations may provide custom handling. There is no need
       to calculate an error's position because the r_error handlers
       do that. Specializations may want, e.g., to collect a range
       of characters around the error point (the current position
       of the parser_state object).

       Design note: another alternative to solve the
       error-number-to-string problem without requiring template
       specializations would be to use a static map<int,string>, but
       then we'd need to provide .cpp files along with the .hpp files
       for this lib, and i don't wanna do that.
    */
    template <int ErrorNumber = Errors::Unknown>
    struct error_msg
    {
	template <typename State>
	static std::string message( parser_state &, State & )
	{
	    std::ostringstream os;
	    os << "Unknown (or unspecified) parsing error #" << ErrorNumber;
	    return os.str();
	}
    };

    /**
       Specialization for UnexpectedCharacter errors.
    */
    template <>
    struct error_msg<Errors::UnexpectedCharacter>
    {
	template <typename State>
	static std::string message( parser_state & ps, State & )
	{
	    std::string msg("Unexpected character '");
	    if( ps.eof() ) msg.append("EOF");
	    else msg.push_back(*ps.pos());
	    msg.push_back('\'');
	    return msg;
	}
    };

    /**
       Specialization for UnexpectedEOF errors.
    */
    template <>
    struct error_msg<Errors::UnexpectedEOF>
    {
	template <typename State>
	static std::string message( parser_state & ps, State & )
	{
	    return "Unexpected end of input";
	}
    };

    /**
       Specialization for UnclosedComment errors.
    */
    template <>
    struct error_msg<Errors::UnclosedComment>
    {
	template <typename State>
	static std::string message( parser_state & ps, State & )
	{
	    return "Reached EOF inside of a multi-line comment";
	}
    };

    /**
       Specialization for IllegalCharacter errors.
    */
    template <>
    struct error_msg<Errors::IllegalCharacter>
    {
	template <typename State>
	static std::string message( parser_state & ps, State & )
	{
	    std::string msg("Illegal character '");
	    msg.push_back(*ps.pos());
	    msg.push_back('\'');
	    return msg;
	}
    };

    /**
       Similar to r_throw, this rule throws a parse_error
       exception. The what() text of the exception is the text of
       error_msg<ErrorNumber>, allowing one to specialize error_msg to
       create custom error messages. The where() part of the exception
    */
    template <int ErrorNumber = Errors::Unknown>
    struct r_error
    {
	typedef r_error type;
	template <typename State>
	static bool matches( parser_state & in, State & st )
	{
	    std::ostringstream os;
	    os << "Parse error #" << ErrorNumber
	       << ": " << error_msg<ErrorNumber>::message(in,st);
	    throw parse_error( in, os.str() );
	    return false;
	}
    };

    /**
       If Rule matches then the effect is the same as calling
       r_error<ErrorNumber>::matches(), otherwise it returns
       true. If Rule matches then the input interator is moved
       back to the point it was at before the match, to allow
       more accurate error reporting.
    */
    template <typename Rule, int ErrorNumber = Errors::Unknown>
    struct r_error_if
    {
	typedef r_error_if type;
	template <typename State>
	static bool matches( parser_state & in, State & st )
	{
	    parse_iterator pos(in.pos());
	    if( Rule::matches(in,st) )
	    {
		in.pos(pos);
		r_error<ErrorNumber>::matches(in,st);
	    }
	    return true;
	}
    };

    template <typename R>
    struct r_throw_if : r_and< rule_list<
	r_and< rule_list<R,r_throw> >,
	r_success > >
    {};



    /**
       The evil twin of r_error_if, this Rule throws an error
       if Rule does NOT match.
    */
    template <typename Rule, int ErrorNumber = Errors::Unknown>
    struct r_error_unless
    {
	typedef r_error_unless type;
	template <typename State>
	static bool matches( parser_state & in, State & st )
	{
	    parse_iterator pos(in.pos());
	    if( ! Rule::matches(in,st) )
	    {
		in.pos(pos);
		r_error<ErrorNumber>::matches(in,st);
	    }
	    return true;
	}
    };

    /**
       This is an Action form of the r_error Rule, mainly for use with
       r_ifelse and similar Rules.
    */
    template <int ErrorNumber>
    struct a_error
    {
	typedef a_error type;
	template <typename State>
	static void matched( parser_state & in, std::string const &, State & st )
	{
	    throw parse_error( in, error_msg<ErrorNumber>::message(in,st) );
	}
    };
    
} // namespace

#endif // s11n_net_PARSEPP_ERRORS_HPP_INCLUDED
