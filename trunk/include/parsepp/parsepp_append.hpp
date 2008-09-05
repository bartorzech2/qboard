#ifndef s11n_net_PARSEPP_APPEND_HPP_INCLUDED
#define s11n_net_PARSEPP_APPEND_HPP_INCLUDED

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <set>

namespace Ps {

    /**
       Helper type for a_append. See that type for details.

       Specializations must have:

       static void matched( parser_state &, std::string const &, ClientState & );
    */
    template <typename ClientState>
    struct a_append_spec;

    /**
       a_append is a generic Action for appending matched
       input to a State object. It requires that a specialized
       a_append_spec implementation be installed, specialized
       on the ClientState type(s) passed to matched().
    */
    struct a_append
    {
	typedef a_append type;
	template <typename ClientState>
	static void matched( parser_state &ps,
			     const std::string &m,
			     ClientState & st )
	{
	    a_append_spec<ClientState>::matched( ps, m, st );
	}
    };


    /**
       Action to append matched string to a std::string ClientState object.
    */
    template <>
    struct a_append_spec<std::string>
    {
	static void matched( parser_state &ps,
			     const std::string &m,
			     std::string & st )
	{
	    st += m;
	}
    };

    /**
       A helper type for a_append_spec implementations. Container must
       support:

       c.insert( c.end(), std::string(...) )

       (e.g. std::vector, std::list, or std::set).
    */
    template <typename Container>
    struct a_append_inserter
    {
	static void matched( parser_state &,
			     const std::string &m,
			     Container & st )
	{
	    st.insert( st.end(), m );
	}
    };


    /**
       Action to append matched string to a vector<string> ClientState object.
    */
    template <>
    struct a_append_spec< std::vector<std::string> > :
	a_append_inserter< std::vector<std::string> >
    {};

    /**
       Action to append matched string to a set<string> ClientState object.
    */
    template <>
    struct a_append_spec< std::set<std::string> > :
	a_append_inserter< std::set<std::string> >
    {};

    /**
       Action to append matched string to a list<string> ClientState object.
    */
    template <>
    struct a_append_spec< std::list<std::string> > :
	a_append_inserter< std::list<std::string> >
    {};


    /**
       Helper to assist in generation of actions for
       lexically casting matches and appending them
       to a container.

       Container must support:

       c.insert( posHint, value )
    */
    template <typename Container>
    struct a_append_lexcast_inserter
    {
	static void matched( parser_state &ps,
			     const std::string &m,
			     Container & st )
	{
	    typedef typename Container::value_type ValT;
	    ValT tgt;
	    std::istringstream in(m);
	    in >> tgt;
	    st.insert( st.end(), tgt );
	}
    };

    /**
       Partially specialized for vector<ValT>, where ValT must be a
       type which we can lexical cast to using i/ostream operators.
    */
    template <typename ValT> 
     struct a_append_spec< std::vector<ValT> > :
 	a_append_lexcast_inserter< std::vector<ValT> >
    {};

    /**
       Partially specialized for set<ValT>, where ValT must be a
       type which we can lexical cast to using i/ostream operators.
    */
    template <typename ValT> 
     struct a_append_spec< std::set<ValT> > :
 	a_append_lexcast_inserter< std::set<ValT> >
    {};

    /**
       Partially specialized for list<ValT>, where ValT must be a
       type which we can lexical cast to using i/ostream operators.
    */
    template <typename ValT> 
     struct a_append_spec< std::list<ValT> > :
 	a_append_lexcast_inserter< std::list<ValT> >
    {};


} // namespace

#endif // s11n_net_PARSEPP_APPEND_HPP_INCLUDED
