#ifndef s11n_net_PARSEPP_TYPELIST_HPP_INCLUDED
#define s11n_net_PARSEPP_TYPELIST_HPP_INCLUDED

namespace Ps {

    struct nil_t
    {
	/** This typedef is used by the Rules API. */
	typedef nil_t type;
    };

    /**
       A base type for implementing a list of Rule types, implemented
       as a Typelist, as described by Alexandrescu in "Modern C++
       Design". H may be any type. T must be either nil_t (to mark the
       end of the typelist) or a rule_list_base<...> (or strictly
       compatible).

       Client code will normally use the rule_list<> types instead
       of using this one directly.
    */
    template <typename H, typename T>
    struct rule_list_base
    {
	/**
	   Subtypes of this type should not override the 'type'
	   typedef, as it is used by the core lib to trick some
	   overloads into working, such that subclasses of
	   rule_list_base will be picked up by specializations for
	   certain rules, as if they actually were a rule_list_base.

	   There might be exceptions to the no-override rule, but none
	   come to mind.
	*/
	typedef rule_list_base type;
	/** This typedef is used by the Rules API. */
	/* First Type in the list. */
	typedef H head;
	/* Second type in the list (must be either nil_t or
	   rule_list_base<>).
	*/
	typedef T tail;
    };

    typedef char char_type;
    /**
       A type for implementing lists of integers. It works like
       a typelist, in that Val must be an integer and Tail must
       be either nil_t or char_list_base<AnInt,X>, where X has
       the same requirements as Tail.

       By convention, a Val of -1 is used to mark the end of a list.
    */
    template <char_type Val,typename Tail>
    struct char_list_base
    {
	/** See description for rule_list_base::type. */
	typedef char_list_base type;
	/**
	   The Val template parameter.
	*/
	static const char_type value = Val;
	/**
	   The Tail of this list. Must be either nil_t or
	   a char_list_base<...>.
	*/
	typedef Tail tail;
    };

} // namespace

#ifndef PARSEPP_RULE_LIST_MAX_ARGS
/**
   Define PARSEPP_RULE_LIST_MAX_ARGS before including this file to set
   the maximum number of arguments which rule_list<> and char_list<>
   can accept. It defaults to "some reasonable number", and
   pre-generated versions are available for 0-30 arguments. For
   generating variants with higher arg counts, see the maketypelist.pl
   script in this code's source repo.

   ACHTUNG:

   Changing this number may require rebuilding any code compiled using
   a different number. The main template instantiations will be
   different for code generated with a different maximum arg count.
   So pick a number and stick with it.
*/
#define PARSEPP_RULE_LIST_MAX_ARGS 15
#endif
namespace Ps {
#if PARSEPP_RULE_LIST_MAX_ARGS < 6
#  include "parsepp_typelist_5.hpp"
#elif PARSEPP_RULE_LIST_MAX_ARGS < 11
#    include "parsepp_typelist_10.hpp"
#elif PARSEPP_RULE_LIST_MAX_ARGS < 16
#    include "parsepp_typelist_15.hpp"
#elif PARSEPP_RULE_LIST_MAX_ARGS < 21
#    include "parsepp_typelist_20.hpp"
#elif PARSEPP_RULE_LIST_MAX_ARGS < 26
#    include "parsepp_typelist_25.hpp"
#elif PARSEPP_RULE_LIST_MAX_ARGS < 31
#    include "parsepp_typelist_30.hpp"
#else
#    error PARSEPP_RULE_LIST_MAX_ARGS is too high. See the docs above this code for details.
#endif
} // namespace
#undef PARSEPP_RULE_LIST_MAX_ARGS

#endif // s11n_net_PARSEPP_TYPELIST_HPP_INCLUDED
