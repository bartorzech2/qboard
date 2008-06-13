#include "QBBatch.h"

#include "S11nQt.h"
#include "QBoard.h"

#include <sstream>
#include <memory>
#include <algorithm>
#include <functional>
#include <fstream>
#include <QDebug>

namespace QBBatch {

    struct a_set_toeocmd
    {
	typedef a_set_toeocmd type;
	static void matched( parser_state &, std::string const & m, State & st )
	{
	    QString tmp( QString(m.c_str()).simplified() );
	    st.toeocmd = tmp.isEmpty() ? "" : tmp.toAscii().constData();
	}
    };

    struct a_board
    {
	typedef a_board type;
	static void matched( parser_state &, std::string const &, State & st)
	{
		QBB_VERBOSE << "Setting current item to [board].\n";
		st.current = &st.gstate.board();
		//CERR << "pos: L" << ps.pos().line() << " C"<<ps.pos().col() << '\n';
	}
    };

    struct a_piece
    {
	typedef a_piece type;
	static void matched( parser_state &ps, std::string const &, State & st)
	{
	    QBB_VERBOSE << "Setting current item to [piece].\n";
	    if( ! st.piece )
	    {
		std::string msg("The 'piece' command cannot be used before a piece is created with 'new piece'");
		QBB_PERR << msg<<'\n';
		throw parse_error(ps,msg);
	    }
	    st.current = st.piece;
	}
    };

    struct a_new
    {
	typedef a_new type;
	static void matched( parser_state & ps, std::string const & m, State & st )
	{
	    QBB_VERBOSE << "trying to create new object of type [" << m << "]\n";
	    if( "piece" == m )
	    {
		QBB_VERBOSE << "Creating new piece.\n";
		st.gstate.pieces().addPiece( (st.piece = new GamePiece) );
		st.current = st.piece;
		//COUT << "Created new game piece.\n";
		
	    }
	    else if( "game" == m )
	    {
		QBB_VERBOSE << "Clearing game state and starting anew.\n";
		st.gstate.clear();
	    }
	    else
	    {
		std::string msg("Invalid type name for a_new_obj: "+m);
		QBB_PERR << msg<<'\n';
		throw parse_error(ps,msg);
	    }
	}
    };

    struct a_set_type_error
    {
	typedef a_set_type_error type;
	static void matched( parser_state & ps, std::string const & m, State & )
	{
	    std::ostringstream os;
	    os << "Invalid flag for 'set -TYPE': ["<<m<<"]";
	    std::string msg(os.str());
	    QBB_PERR << msg << '\n';
	    throw parse_error(ps,msg);
	}
    };

    struct a_save
    {
	typedef a_save type;
	static void matched( parser_state &ps, std::string const &, State & st )
	{
	    std::string rol( st.toeocmd );
	    //COUT << "Saving to ["<<rol<<"]\n";
	    bool worked = (rol.empty() || (rol=="-"))
		? st.gstate.save( std::cout )
		: st.gstate.save( QString(rol.c_str()) );
	    if( worked )
	    {
		QBB_VERBOSE << "Saved game state to ["
			<<(rol.empty()?("cout"):rol)
			<< "].\n";
	    }
	    else
	    {
		std::ostringstream os;
		os << "SAVE FAILED to ["
		   <<(rol.empty()?("cout"):rol)
		   << "].\n";
		QBB_PERR << os.str();
		throw parse_error(ps,os.str());
	    }
	}
    };


    Name2VTMap & namesMap()
    {
	static Name2VTMap m;
	if( m.empty() )
	{
	    // General properties:
	    m["alpha"] = QVariant::Int;
	    m["angle"] = QVariant::Int;
	    m["color"] = QVariant::Color;
	    m["pixmap"] = QVariant::String;
	    m["pos"] = QVariant::Point;
	    m["size"] = QVariant::Size;
	    m["style"] = QVariant::Int;
	    m["width"] = QVariant::Int;
	    m["zLevel"] = QVariant::Double;

	    // Line-specific:
	    m["arrowSize"] = QVariant::Int;
	    m["drawArrows"] = QVariant::Int;

	    // Piece-specific:
	    m["borderAlpha"] = QVariant::Int;
	    m["borderColor"] = QVariant::Color;
	    m["borderSize"] = QVariant::Int;
	    m["colorAlpha"] = QVariant::Int;

	    // Other?
	    m["text"] = QVariant::String;
	}
	return m;
    }

    int vtype( std::string const & n )
    {
	typedef Name2VTMap::const_iterator IT;
	IT it( namesMap().find(n) );
	return (namesMap().end() == it)
	    ? int(QVariant::Invalid)
	    : (*it).second;
    }

    QVariant propToVariant( std::string const & key,
			    std::string const & val,
			    int typeHint )
    {
	const int type = (typeHint == QVariant::Invalid)
	    ? vtype(key)
	    : typeHint;
	QVariant var;
	QString vs(val.c_str());
	S11nNode node;
	typedef S11nNodeTraits NTR;
	/**
	   Note that here we shamelessly abuse internal knowledge of
	   various Qt-related serializable types here. If the S11nQt
	   algos change, that may break this code.
	 */
#define TRY_DESER if( !s11nlite::deserialize(node,lav) ) {\
	    throw s11n::s11n_exception("Could not convert property [%s=[%s]] to type QVariant::Types[%d] via s11n", \
				       key.c_str(),val.c_str(),type );\
	} else {  var = lav; }

	switch( type )
	{
	  case QVariant::Color:{
	      if( ! val.empty() && (val[0]=='#') )
	      {
		  var = QColor(vs);
	      }
	      else if( 0 == QRegExp("^[a-z]+").indexIn(vs) )
	      {
		  var = QColor(vs);
	      }
	      else
	      {
		  NTR::set(node,"rgb",val);
		  QColor lav;
		  TRY_DESER;
	      }
	  }
	      break;
	  case QVariant::Double:
	      var = QVariant( QVariant(vs).toDouble() );
	      break;
	  case QVariant::Int:
	      var = QVariant( QVariant(vs).toInt() );
	      break;
	  case QVariant::LongLong:
	      var = QVariant( QVariant(vs).toLongLong() );
	      break;
	  case QVariant::Point: {
	      NTR::set(node,"xy",val);
	      QPoint lav;
	      TRY_DESER;
	  }
	      break;
	  case QVariant::PointF: {
	      NTR::set(node,"xy",val);
	      QPointF lav;
	      TRY_DESER;
	  }
	      break;
	  case QVariant::Size: {
	      NTR::set(node,"wh",val);
	      QSize lav;
	      TRY_DESER;
	  }
	      break;
	  case QVariant::SizeF: {
	      NTR::set(node,"wh",val);
	      QSizeF lav;
	      TRY_DESER;
	  }
	      break;
	  case QVariant::String:
	      var = QVariant(vs);
	      break;
	  case QVariant::UInt:
	      var = QVariant( QVariant(vs).toUInt() );
	      break;
	  case QVariant::ULongLong:
	      var = QVariant( QVariant(vs).toULongLong() );
	      break;
	  default:
	      QBB_WARNING << "using default string-to-variant conversion for ["
		   << QString(key.c_str())<<"]=["<<vs<<"]";
	      var = QVariant(vs);
	      break;
	}
	return var;
#undef TRY_DESER
    }

    void process_scripts( int argc, char ** argv )
    {

	QBBatch::State state;
	for( int i = 0; i < argc; ++i )
	{
	    std::string fn(argv[i]);
	    if( fn.empty() ) continue;
	    if( "-v" == fn )
	    {
		state.verbose = true;
		continue;
	    }
	    std::ostringstream buf;
	    {
		std::ifstream is( fn.c_str() );
		if( ! is.good() )
		{
		    throw std::runtime_error("Could not open file '"+fn+"'.");
		}
		std::copy( std::istreambuf_iterator<char>(is),
			   std::istreambuf_iterator<char>(),
			   std::ostreambuf_iterator<char>( buf ) );
	    }
	    if( ! Ps::parse< QBBatch::start >( buf.str(), state ) )
	    {
		throw std::runtime_error("script parse failed");
	    }
	}
    }


} // namespace
