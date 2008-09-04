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
#include <QBuffer>
#include <QByteArray>

#include <cctype>

#include "parsepp.hpp"
#include "WikiLiteParser.h"

#include <iostream>
#include <vector>
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
	Para = 0x0080,
	Superscript = 0x0100,
	Subscript = 0x0200,
	Strike = 0x0400,
	Header = 0x0800,
	TD = 0x1000,
	Table = 0x2000,
	END = 0xFFFF
	};
	long flags;
	int brCount;
	int listDepth;
	std::vector<int> listTypes;
	QString wikiWord;
	QString wikiLabel;
	int tableCol;
	int tableRow;
	state_t() : out(0),
		    flags(None),
		    brCount(0),
		    listDepth(0),
		    listTypes(10,0),
		    wikiWord(),
		    wikiLabel(),
		    tableCol(0),
		    tableRow(0)
	{
	}

	~state_t()
	{
	}

	void output( QString const & t )
	{
	    this->out->write( t.toAscii() );
	}
	void output_char( int ch )
	{
	    if( std::isspace( ch ) )
	    {
		this->out->putChar( ch );
		return;
	    }
	    if( ( ch < 32) || (ch>127) )
	    {
		QString exp;
		exp.sprintf("&#x%02x;", ch);
		this->output( exp );

	    }
	    else switch( ch )
	    {
	      case '&':
	      case '<':
	      case '\'': // We translate quotes because google code wiki does.
	      case '"':  // Maybe it's a security thing? Or an i18n thing?
		  {
		      QString exp;
		      exp.sprintf("&#x%02x;", ch);
		      this->output( exp );
		  } break;
	      default:
		  this->out->putChar(ch);
		  return;
		  break;
	    }
	}
	void openPara()
	{
	    if( !(this->flags & Para) )
	    {
		this->output( "<p class='WLP'>" );
		this->flags += Para;
	    }
	}
	bool closePara()
	{
	    if( this->flags & Para )
	    {
		this->output( "</p>" );
		this->flags -= Para;
		return true;
	    }
	    return false;
	}
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
	static void matched( parser_state & ps,
			     const std::string &,
			     state_t & state )
	{
	    if( state.flags & state_t::Bullet )
	    {
		state.flags -= state_t::Bullet;
		state.output( "</li>" );
	    }
	    if( state.listDepth )
	    { // close list
		int type = state.listTypes[state.listDepth];
		state.listTypes[state.listDepth--] = 0;
		QString tag;
		if( state_t::UList == type ) tag = "</ul>\n";
		else if( state_t::OList == type ) tag = "</ol>\n";
		else
		{
		    //throw new Ps::parse_error(ps,"Wiki parser: invalid list depth state.");
		    QString msg =
			QString("(WIKI PARSER ERROR: : invalid list depth state for tag '%s'.)").
			arg(tag);
		    throw new Ps::parse_error(ps,msg.toAscii().constData());
		    //state.output(msg);
		    return;
		}
		--state.brCount;
		state.output(tag);
		return;
	    }

	    ++state.brCount;
	    if( (state.brCount > 1) )
 	    {
		state.output("<br/>\n");
		state.brCount = 0;
 	    }
 	    else
 	    {
 		state.output("\n");
 	    }
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
	    --state.brCount;
	    if( ! (state.flags & state_t::Header) )
	    {
		state.flags += state_t::Header;
		tag.sprintf("\n<h%d class='WLP'>",level);
	    }
	    else
	    {
		state.flags -= state_t::Header;
		tag.sprintf("</h%d>",level);
	    }
	    state.output( tag );
	}
    };

    struct a_char
    {
	static void matched( parser_state &,
			     const std::string &m,
			     state_t & state )
	{
	    if( 1 == m.size() )
	    {
		state.output_char( m[0] );
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
	    bool doclose = (state.flags & tagFlag);
	    bool doopen = !doclose;
	    QString style(" class='WLP'" );
	    switch( tagFlag )
	    {
	      case state_t::Bold: tag = "strong"; break;
	      case state_t::Italics: tag = "em"; break;
	      case state_t::FixedFont: tag = "tt"; break;
	      case state_t::Superscript: tag = "sup"; break;
	      case state_t::Subscript: tag = "sub"; break;
	      case state_t::Strike: tag = "strike"; break;
	      case state_t::TD:
		  style = "";
 		  if( doopen )
 		  {
 		      ++state.tableCol;
 		  }
  		  if( doopen && (1 == state.tableCol) )
  		  {
		      ++state.tableRow;
  		      state.output("<tr>");
  		  }
#if 1
 		  tag =( 1 == state.tableRow )
		      ? "th"
		      : "td";
#else
		  tag = "td";
#endif
// 		  if( !doclose ) state.output("<tr>");
  		  doopen = true;
		  break;
	      default:
		  tag = "UNKNOWN";
		  break;
	    };
	    if(1) CERR << "a_tag<"<<std::hex<<tagFlag<<"["<<tag<<"]> state.flags="
		       <<std::hex<<state.flags
		       << ", doopen="<<doopen<<" doclose="<<doclose
		       << ", tableCol"<<state.tableCol
		       <<'\n';
	    if( doclose )
	    {
		state.output( QString("</%1>").arg(tag) );
		state.flags &= ~tagFlag;
	    }
	    if( doopen )
	    {
		state.output( QString("<%1%2>").
			      arg(tag).
			      arg(style) );
		state.flags |= tagFlag;
	    }
#if 0
	    static bool bogo = false;
	    if( !bogo)
	    {
		bogo =true;
#define SHOW(F) CERR << # F << " == " << std::hex << (F) << '\n';
		int fl = 0x0020;
		int mask = 0x0010;
		SHOW(fl);
		SHOW(mask);
		SHOW(fl ^ mask);
		SHOW(fl | mask);
		SHOW(fl & mask);
		SHOW(~mask);
		SHOW(fl & ~mask);
#undef SHOW
	    }
#endif
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
	    if(true)
	    {
		QString tag("<pre class='WLP'");
		tag.append( " style='margin-left: 2em;" );
		tag.append( "padding: 0.5em;" );
		tag.append( "border-left: 3px solid #ccc;" );
		tag.append( "'>" );
		state.output( tag );
	    }
	    else
	    {
		state.output("<pre class='WLP'>");
	    }
	    char const * str = accum.c_str();
	    for( ; str && *str; ++str )
	    {
		state.output_char( *str );
	    }
	    state.output("</pre>");
	    --state.brCount;
	    ps.pos( pit );
	    return true;
	}
    };


    template <bool Numbered>
    struct a_bullet_open
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    int flag = Numbered ? state_t::OList : state_t::UList;
	    int type = state.listTypes[state.listDepth];
	    if( state.flags & state_t::Bullet )
	    {
		state.output( "</li>" );
	    }
	    else
	    {
		state.flags += state_t::Bullet;
	    }
	    if( type != flag )
	    {
		state.listTypes[++state.listDepth] = flag;
		QString tag( QString("<%1 class='WLP'>").
			     arg( Numbered ? "ol" : "ul" ) );
		state.output( tag );
	    }
	    state.output( "<li class='WLP'>" );
	}
    };
#if 0
    template <bool Numbered>
    struct a_list_level
    {
	static void matched( parser_state &,
			     const std::string & m,
			     state_t & state )
	{
	    size_t d =  (m.size() + 1) / 2;
	    state.listDepth = d;
	    state.listTypes[d] = Numbered ? state_t::OList : state_t::UList;
	}
    };
#endif
    struct a_hr
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    state.output("<hr class='WLP'/>");
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

    struct r_superscript :
	r_action< r_ch<'^'>, a_tag<state_t::Superscript> >
    {};
    struct r_subscript
	: r_action< r_repeat<r_ch<','>,2>, a_tag<state_t::Subscript> >
    {};
    struct r_strike
	: r_action< r_repeat<r_ch<'~'>,2>, a_tag<state_t::Strike> >
    {};

    struct r_header
	: r_and< RL<
	r_opt< r_repeat< r_ch<'\n'>, 1, 2 > >,
	r_action< r_plus< r_ch<'='> >, a_header_tag >
	> >
    {};

    struct r_fontmod :
	r_or< RL< r_header,
		  r_bold,
		  r_italics,
		  r_fixed,
		  r_superscript,
		  r_subscript,
		  r_strike > >
    {};

    struct r_hr : r_action< r_repeat< r_ch<'-'>, 4>,
			    a_hr >
    {};

    struct r_unclosed :
	r_or< RL<
	r_hr
	> >
    {};

//     struct r_break : r_action< r_repeat<r_ch<'\n'>,2>,
// 			       a_break >
//     {};

//     struct r_toeol : r_star< r_notch<'\n'> >
//     {};

    template <bool Numbered>
    struct r_bullet_x
	: r_action< r_and< RL< r_opt< r_ch<'\n'> >,
#if 0
			       r_action< r_plus< r_repeat<r_blank,2> >,
					 a_list_level<Numbered> >,
#else
			       r_plus< r_repeat<r_blank,2> >,
#endif
			       r_ch<Numbered ? '#' : '*'>,
			       r_star<r_blank> > >,
		    a_bullet_open<Numbered> >
    {};

    struct r_bullet : r_or< RL< r_bullet_x<true>, r_bullet_x<false> > >
    {};

    template <int Phase> // 0 = wikiword/url, 1=description, 2 = finalize
    struct a_wikilink
    {
	static void matched( parser_state &,
			     const std::string &m,
			     state_t & state )
	{
	    if( 0 == Phase )
	    {
		state.wikiWord = m.c_str();
		return;
	    }
	    else if( 1 == Phase )
	    {
		state.wikiLabel = m.c_str();
		return;
	    }
	    QString link = QString("<a href=\"%1\" class='WLP'>%2</a>").
		arg(state.wikiWord).
		arg( state.wikiLabel.isEmpty()
		     ? state.wikiWord
		     : state.wikiLabel );
	    state.output( link );
	}
    };

    struct r_wikilink
	: r_and< RL<
	    r_ch<'['>,
	    r_action< r_plus<
		r_and< RL< r_notat<r_blank>, r_advance<1> > >
		>,
	      a_wikilink<0> >,
	    r_star< r_blank >,
	    r_action< r_star< r_notch<']'> >, a_wikilink<1> >,
	    r_action< r_ch<']'>, a_wikilink<2> >
	> >
    {};
    template <int CH>
    struct a_unescaped
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    state.output_char( CH );
	}
    };
    struct r_escaped :
	r_or< RL< 
	    r_action< r_repeat<r_ch<'['>,2>, a_unescaped<'['> >
	> >
    {};

    struct r_inlinable
	: r_or< RL< r_escaped,
		    r_wikilink,
		    r_fontmod
		    > >
    {};

    struct a_table_open
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    state.output( "<table border='1' style='border-collapse: separate;' class='WLP'><tbody>\n<tr><th>" );
	    state.tableRow = 1;
	    state.flags |= (state_t::TD | state_t::Table);
	}
    };
    struct a_table_eol
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    if( 1 == state.tableRow )
	    {
		state.output( "</th>" );
	    }
	    else
	    {
		state.output( "</td>" );
	    }
	    state.output( "</tr>\n" );
	    state.tableCol = 0;
	    state.flags &= ~state_t::TD;
	}
    };
    struct a_table_close
    {
	static void matched( parser_state &,
			     const std::string &,
			     state_t & state )
	{
	    state.output( "</td></tr>\n</tbody></table>\n" );
	    state.tableCol = 0;
	    state.tableRow = 0;
	    state.flags &= ~state_t::TD;
	    state.flags &= ~state_t::Table;
	}
    };

    struct r_tbar : r_pad< r_repeat<r_ch<'|'>,2>, r_blank >
    {};

    struct r_crnl : 
	r_and< RL<
	    r_opt< r_ch<'\r'> >,
	    r_ch<'\n'>
	    > >
    {};

    /** \r?\n\s*|| */
    struct r_table_open :
	r_and< RL<
	    r_crnl,
	    r_star<r_blank>,
	    r_tbar
	    > >
    {};


    struct r_table_eol :
	r_and< RL<
	    r_pad<r_tbar, r_blank>,
	    r_crnl
	    > >
    {};

    struct r_table_close :
	r_and< RL<
	    r_table_eol,
	    r_crnl
	    > >
    {};

//     struct r_td :
// 	r_action< r_tbar, a_tag<state_t::TD> >
//     {};

    struct r_table_part : r_or< RL<
	r_action<r_table_close, a_table_close >,
	r_action<r_table_eol, a_table_eol >,
	r_action< r_table_open, a_table_open >,
	r_action< r_tbar, a_tag<state_t::TD> >
    > >
    {};



    struct r_markup
	: r_or< RL< r_table_part,
		    r_inlinable,
		    r_verbatim,
		    //r_td,
		    r_bullet,
		    r_unclosed,
		    r_newline		    
		    > >
    {};

    struct r_step
	: r_or< RL< r_markup,
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
	try
	{
	    return Ps::parse< start >( str, st );
	}
	catch( std::exception const & ex )
	{
	    char const * w = ex.what();
	    QString msg =
		QString("(ERROR: WikiLiteParser::parse() exception: %1)").
		arg(w ? w : "no error info!");
	    out->write( msg.toAscii() );
	}
	return false;
    }
    QString WikiLiteParser::parse( QString const & code )
    {
	QByteArray ba;
	QBuffer buf(&ba);
	buf.open(QIODevice::WriteOnly);
	this->parse( code, &buf );
	buf.close();
	return QString(ba);
    }
#undef RL
#undef CL
}
