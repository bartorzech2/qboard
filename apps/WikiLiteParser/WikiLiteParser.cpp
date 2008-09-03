/*
 * This file is (or was, at some point) part of the QBoard project
 * (http://code.google.com/p/qboard)
 *
 * Copyright (c) 2008 Stephan Beal (http://wanderinghorse.net/home/stephan/)
 *
 * This file may be used under the terms of the GNU General Public
 * License versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 * included in the packaging of this file.
 *
 */


#include <QString>
#include <QIODevice>

#include <cctype>

#include "parsepp.hpp"
#include "WikiLiteParser.h"

#include <iostream>
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "

namespace qboard {
    using namespace Ps;

#define RL rule_list
#define CL char_list

    struct state_t
    {
	QIODevice * out;
	enum Flags {
	None = 0x0000,
	Bold = 0x0001,
	Italics = 0x0002,
	FixedFont = 0x0008,
	Bullet = 0x0010,
	UList = 0x0020,
	OList = 0x0040,
	Header = 0x0100,
	END = 0xEFFF
	};
	long flags;
	int nlCount;
	state_t() : out(0),
		    flags(None),
		    nlCount(0)
	{}
    };


    struct a_space
    {
	static void matched( parser_state &,
			     const std::string &m,
			     state_t & state )
	{
	    state.out->write( m.c_str(), m.size() );
	}
    };

    struct a_newline
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    ++state.nlCount;
	    if( state.flags & state_t::Bullet )
	    {
		state.flags -= state_t::Bullet;
		state.out->write("</li>\n",6);
		return;
	    }
	    if( state.flags & state_t::UList )
	    {
		state.flags -= state_t::UList;
		state.out->write("</ul>\n",6);
		return;
	    }
	    if( state.nlCount >= 2 )
	    {
		state.out->write("</br>\n",6);
		return;
	    }
	    else
	    {
		state.out->putChar('\n');
	    }
	}
    };

    static void output_char( int ch, state_t & state )
    {
	if( std::isspace( ch ) )
	{
	    state.out->putChar( ch );
	    return;
	}
	if( ( ch < 32) || (ch>127) )
	{
	    QString exp;
	    exp.sprintf("&#x%02x;", ch);
	}
	else switch( ch )
	{
	  case '&':
	  case '<':
	  case '\'':
	  case '"':
	  {
	      QString exp;
	      exp.sprintf("&#x%02x;", ch);
	      state.out->write( exp.toAscii() );
	  } break;
	  default:
	      state.out->putChar(ch);
	      return;
	      break;
	};
    }

    struct a_char
    {
	static void matched( parser_state &,
			     const std::string &m,
			     state_t & state )
	{
	    state.nlCount = 0;
	    if( 1 == m.size() )
	    {
		output_char( m[0], state );
	    }
	}
    };


    template <int tagFlag>
    struct a_tag
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    char const * tag = 0;
	    switch( tagFlag )
	    {
	      case state_t::Bold: tag = "strong"; break;
	      case state_t::Italics: tag = "em"; break;
	      case state_t::FixedFont: tag = "tt"; break;
	      default:
		  tag = "UNKNOWN";
		  break;
	    };
	    state.out->putChar('<');
	    if( state.flags & tagFlag )
	    {
		state.out->putChar('/');
		state.flags -= tagFlag;
	    }
	    else
	    {
		state.flags += tagFlag;
	    }
	    unsigned int tlen = qstrnlen( tag, 20 );
	    state.out->write( tag, tlen );
	    state.out->putChar('>');
	}
    };

    struct r_space : r_action< r_plus<Ps::r_space>, a_space >
    {};

    struct r_newline
	: r_action< r_ch<'\n'>, a_newline >
    {};
    struct r_rest
	: r_action< r_any, a_char >
    {};

    struct r_verbatim_handler
    {
	typedef r_verbatim_handler type;
	template <typename ClientState>
	inline static bool matches( parser_state & ps, ClientState & state )
	{
	    parse_iterator pit( ps.pos() );
	    int closeCount = 0;
	    std::string accum;
	    for( ; (!pit.eof()) && (closeCount<3); ++pit )
	    {
		if( '}' == *pit )
		{
		    ++closeCount;
		}
		else
		{
		    closeCount = 0;
		}
		accum.push_back(*pit);
	    }
	    if( closeCount != 3 )
	    {
		return false;
	    }
	    if( accum.size() >= 3 )
	    {
		accum = accum.substr( 0, accum.size() - 3 );
	    }
	    state.out->write("<pre>",5);
	    char const * str = accum.c_str();
	    for( ; str && *str; ++str )
	    {
		output_char( *str, state );
	    }
	    state.out->write("</pre>",11);
	    ps.pos( pit );
	    return true;
	}
    };

    struct a_header_tag
    {
	static void matched( parser_state &,
			     const std::string &m,
			     state_t & state )
	{
	    int level = int(m.size());
	    QString tag;
	    if( ! (state.flags & state_t::Header) )
	    {
		state.flags += state_t::Header;
		tag.sprintf("<h%d>",level);
	    }
	    else
	    {
		state.flags -= state_t::Header;
		tag.sprintf("</h%d>",level);
	    }
	    state.out->write( tag.toAscii() );
	}
    };

    struct a_bullet_open
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    if( ! (state.flags & state_t::Bullet) )
	    {
		state.flags += state_t::Bullet;
	    }
	    if( ! (state.flags & state_t::UList) )
	    {
		state.flags += state_t::UList;
		state.out->write("<ul>", 4);
	    }
	    state.out->write( "<li>", 4 );
	}
    };

    struct r_verbatim :
	r_and< RL<
	    r_repeat< r_ch<'{'>, 3 >,
	    r_verbatim_handler > >
    {};


    struct r_bold
	: r_action< r_ch<'*'>, a_tag<state_t::Bold> >
    {};
    struct r_italics
	: r_action< r_ch<'_'>, a_tag<state_t::Italics> >
    {};

    struct r_fixed
	: r_action< r_ch<96>, a_tag<state_t::FixedFont> >
    {};

    struct r_header
	: r_action< r_plus< r_ch<'='> >, a_header_tag >
    {};

//     struct r_break : r_action< r_repeat<r_ch<'\n'>,2>,
// 			       a_break >
//     {};

    struct r_toeol : r_star< r_notch<'\n'> >
    {};

    struct r_bullet
	: r_action< r_and< RL< r_opt< r_ch<'\n'> >,
			       r_plus< r_repeat<r_blank,2> >,
			       r_ch<'*'>,
			       r_star<r_blank> > >,
		    a_bullet_open >
    {};

    struct r_marker
	: r_or< RL< r_bullet,
		    r_verbatim,
		    r_italics,
		    r_bold,
		    r_fixed,
		    r_header
		    > >
    {};

    struct r_step
	: r_or< RL< r_newline,
		    r_marker,
		    r_rest > >
    {};

    struct start : r_star< r_step >
    {
    };

    bool WikiLiteParser::parse( QString const & code,
				QIODevice * out )
    {
	state_t st;
	st.out = out;
	std::string str( code.toAscii().constData() );
	parser_state ps( str );
	return Ps::parse< start >( ps, st );
    }

#undef RL
#undef CL
}
