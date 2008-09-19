#ifndef s11n_net_PARSEPP_CISH_HPP_INCLUDED
#define s11n_net_PARSEPP_CISH_HPP_INCLUDED

#include "parsepp.hpp"
#include "parsepp_typelist.hpp"
#include "parsepp_err.hpp"
namespace Ps {


    /** Parser for C++-style comments. */
    struct r_comment_cpp :
	r_and< rule_list<
	    r_chseq< char_list<'/','/'> >,
	    r_star< r_notch<'\n'> >,
	    r_eol
	    > >
    {};

    namespace Detail
    {
	/** Inner part of a C++ comment. Consumes until '*' followed by '/'. */
	struct r_comment_c_inner : r_and< rule_list<
	    r_star< r_and< rule_list<
		r_notat< r_chseq< char_list<'*','/'> > >,
		r_advance<1>
		> > >,
	    r_error_if< r_eof, Errors::UnclosedComment >
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
    
} // namespace

#endif // s11n_net_PARSEPP_CISH_HPP_INCLUDED
