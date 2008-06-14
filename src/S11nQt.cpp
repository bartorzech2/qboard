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

#include "S11nQt.h"
#include <QRegExp>
#include <QChar>
#include <s11n.net/s11n/functional.hpp>
#include "S11nQtList.h"

bool QColor_s11n::operator()( S11nNode & dest, QColor const & src ) const
{
	typedef s11nlite::node_traits NT;
	std::ostringstream os;
	os 	<< src.red() << ' '
		<< src.green() << ' '
		<< src.blue();
	NT::set( dest, "rgb", os.str() );
	return true;
}
bool QColor_s11n::operator()( S11nNode const & src, QColor & dest )
{
	typedef s11nlite::node_traits NT;
	QColor nil;
	std::string key("rgb");
	std::string sval = NT::get( src, key, std::string() );
	if( sval.empty() )
	{
	    key = "argb";
	    sval = NT::get( src, key, std::string() );
	}
	if( sval.empty() ) return false;
	std::istringstream is( sval );
	if( ! is.good() ) return false;
	int fl;
	if( "argb" == key )
	{
	    is >> fl;
	    nil.setAlpha( fl );
	    if( ! is.good() ) return false;
	}
	is >> fl;
	nil.setRed( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setGreen( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setBlue( fl );
	dest = nil;
	return true;	
}

bool QFont_s11n::operator()( S11nNode & dest, QFont const & src ) const
{
	typedef s11nlite::node_traits NT;
	NT::set( dest, "B", src.bold() );
	NT::set( dest, "it", src.italic() );
	NT::set( dest, "ptf", src.pointSizeF() );
	NT::set( dest, "wt", src.weight() );
	NT::set( dest, "fam", std::string( src.family().toAscii() ) );
	return true;
}
bool QFont_s11n::operator()( S11nNode const & src, QFont & dest )
{
	typedef s11nlite::node_traits NT;
	QFont nil;
	nil.setBold( NT::get(src,"B",dest.bold()) );
	nil.setItalic( NT::get(src,"it",dest.italic()) );
	nil.setPointSizeF( NT::get(src,"ptf",dest.pointSizeF()) );
	nil.setWeight( NT::get(src,"wt",dest.weight()) );
	std::string fam = NT::get(src,"fam",std::string( dest.family().toAscii()) );
	nil.setFamily( QString(fam.c_str()) );
	dest = nil;
	return true;	
}

bool QLineF_s11n::operator()( S11nNode & dest, QLineF const & src ) const
{
	typedef s11nlite::node_traits NT;
	std::ostringstream os;
	os << src.x1() << ' '
		<< src.y1() << ' '
		<< src.x2() << ' '
		<< src.y2();
	NT::set( dest, "xyXY", os.str() );
	return true;
}
bool QLineF_s11n::operator()( S11nNode const & src, QLineF & dest )
{
	typedef s11nlite::node_traits NT;
	std::istringstream is( NT::get( src, "xyXY", std::string() ) );
	qreal x1, x2, y1, y2; 
	if( ! is.good() ) return false;
	is >> x1;
	if( ! is.good() ) return false;
	is >> y1;
	if( ! is.good() ) return false;
	is >> x2;
	if( ! is.good() ) return false;
	is >> y2;	
	dest = QLineF( x1, y1, x2, y2 );
	return true;	
}

bool QLine_s11n::operator()( S11nNode & dest, QLine const & src ) const
{
	return QLineF_s11n()( dest, QLineF(src) );
}
bool QLine_s11n::operator()( S11nNode const & src, QLine & dest )
{
	QLineF proxy;
	if( QLineF_s11n()( src, proxy ) )
	{
		dest = proxy.toLine();
		return true;
	}
	return false;
}

bool QPointF_s11n::operator()( S11nNode & dest, QPointF const & src ) const
{
	typedef S11nNodeTraits NT;
	std::ostringstream os;
	os << src.x() << ' ';
	os << src.y();
	NT::set( dest, "xy", os.str() );
	return true;
}
bool QPointF_s11n::operator()( S11nNode const & src, QPointF & dest )
{
	typedef S11nNodeTraits NT;
	QPointF nil;
	std::string val(  );
	std::istringstream is(NT::get( src, "xy", std::string() ));
	if( ! is.good() ) return false;
	qreal fl;
	is >> fl;
	nil.setX( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setY( fl );
	dest = nil;
	return true;	
}

bool QPoint_s11n::operator()( S11nNode & dest, QPoint const & src ) const
{
	return QPointF_s11n()( dest, QPointF(src) );
}
bool QPoint_s11n::operator()( S11nNode const & src, QPoint & dest )
{
	QPointF proxy;
	if( QPointF_s11n()( src, proxy ) )
	{
		dest = proxy.toPoint();
		return true;
	}
	return false;
}

bool QRectF_s11n::operator()( S11nNode & dest, QRectF const & src ) const
{
	typedef s11nlite::node_traits NT;
	std::ostringstream os;
	os << src.x() << ' ';
	os << src.y() << ' ';
	os << src.width() << ' ';
	os << src.height();
	NT::set( dest, "xywh", os.str() );
	return true;
}
bool QRectF_s11n::operator()( S11nNode const & src, QRectF & dest )
{
	typedef s11nlite::node_traits NT;
	QRectF nil;
	std::istringstream is( NT::get( src, "xywh", std::string() ) );
	if( ! is.good() ) return false;
	qreal fl;
	is >> fl;
	nil.setX( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setY( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setWidth( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setHeight( fl );	
	dest = nil;
	return true;	
}

bool QRect_s11n::operator()( S11nNode & dest, QRect const & src ) const
{
	return QRectF_s11n()( dest, QRectF(src) );
}
bool QRect_s11n::operator()( S11nNode const & src, QRect & dest )
{
	QRectF proxy;
	if( QRectF_s11n()( src, proxy ) )
	{
		dest = proxy.toRect();
		return true;
	}
	return false;
}

bool QSizeF_s11n::operator()( S11nNode & dest, QSizeF const & src ) const
{
	typedef S11nNodeTraits NT;
	std::ostringstream os;
	os << src.width() << ' ';
	os << src.height();
	NT::set( dest, "wh", os.str() );
	return true;
}
bool QSizeF_s11n::operator()( S11nNode const & src, QSizeF & dest )
{
	typedef S11nNodeTraits NT;
	QSizeF nil;
	std::string val(  );
	std::istringstream is(NT::get( src, "wh", std::string() ));
	if( ! is.good() ) return false;
	qreal fl;
	is >> fl;
	nil.setWidth( fl );
	if( ! is.good() ) return false;
	is >> fl;
	nil.setHeight( fl );
	dest = nil;
	return true;	
}

bool QSize_s11n::operator()( S11nNode & dest, QSize const & src ) const
{
	return QSizeF_s11n()( dest, QSizeF(src) );
}
bool QSize_s11n::operator()( S11nNode const & src, QSize & dest )
{
	QSizeF proxy;
	if( QSizeF_s11n()( src, proxy ) )
	{
		dest = proxy.toSize();
		return true;
	}
	return false;
}

typedef unsigned short unicode_char_t;
bool QString_s11n::tryAscii = true;
bool QString_s11n::operator()( S11nNode & dest, QString const & src ) const
{
	typedef s11nlite::node_traits NT;
	size_t slen = src.length();
	if( 0 == slen )
	{
		NT::set(dest,"ascii",std::string());
		return true;
	}
	if ( QString_s11n::tryAscii )
	{
		QChar const * qch = src.data();
		bool isAscii = true;
		while( qch && ! qch->isNull() )
		{
			if( qch->unicode() & 0xFF80 ) // > 127 )
			{
				isAscii = false;
				break;
			}
			++qch;
		}
		if( isAscii )
		{
			QByteArray const bar( src.toAscii() );
			char const * cp = bar.data();
			NT::set( dest, "ascii", std::string(cp,cp+bar.size()) );
			return true;
		}
	}
#if 0
	if( 1 )
	{
		std::ostringstream os;
		QChar const * qch = src.data();
		unicode_char_t ch;
		for( ; qch && ! qch->isNull(); ++qch )
		{
			ch = qch->unicode();
			if( ch < 128 )
			{
				os << char(ch);
			}
			else
			{
				os << '\\' << 'u';
				if( ch < 0x1fff )
				{
					os << '0';
				}
				if( ch < 0x01ff )
				{
					os << '0';
				}
				os << std::hex << ch;
			}
		}
		NT::set( dest, "unicode", os.str() );
	}
#endif
	if( 1 )
	{
		// Write a unicode-friendly format (not human-readable)
		NT::set( dest, "len", slen );
		std::ostringstream os;
		QChar const * cp = src.data();
		int ival;
		size_t i;
		for( i = 0; (i < slen) && (cp); ++i, ++cp )
		{
			ival = static_cast<unicode_char_t>( cp->unicode() );
			os << std::hex << ival;
			if( i < (slen-1) ) os << ' ';
		}
		if( i != slen )
		{
			throw s11n::s11n_exception("QString_s11n::deserialize(): length mismatch between string data and declared length (%u vs %u).",
					i, slen );
		}
		NT::set( dest, "data", os.str() );
	}
	return true;
}
bool QString_s11n::operator()( S11nNode const & src, QString & dest )
{
	typedef s11nlite::node_traits NT;
	if( NT::is_set( src, "ascii" ) )
	{
		char const * cp = NT::get(src,"ascii",std::string()).c_str(); 
		dest = QString( cp ? cp : "" );
		return true;
	}
	if( ! NT::is_set( src, "len" ) )
	{
		return false;
	}
	size_t len = NT::get( src, "len", 0 );
	QString tmp;
	if( len )
	{
		unicode_char_t uc;
		std::istringstream is( NT::get( src, "data", std::string() ) );
		while( is.good() )
		{
			is >> std::hex;
			is >> uc;
			tmp.append( QChar(uc) );
		}
	}
	dest = tmp;
	return true;	
}

bool QStringList_s11n::operator()( S11nNode & dest, QStringList const & src ) const
{
	std::for_each( src.begin(), src.end(),
			s11n::ser_to_subnode_unary_f( dest, "string" ) );
#if 0
	QStringList::const_iterator it = src.begin();
	QStringList::const_iterator et = src.end();
	for( ; et != it; ++it )
	{
		if( ! s11n::serialize_subnode( dest, "string", *it ) )
		{
			return false;
		}
	}
#endif
	return true;
}
bool QStringList_s11n::operator()( S11nNode const & src, QStringList & dest )
{
	typedef S11nNodeTraits NT;
	std::for_each( NT::children(src).begin(),
		NT::children(src).end(),
		s11n::deser_to_outiter_f<QString>( std::back_inserter(dest) ) ); 
	return true;
}


bool QVariant_s11n::canHandle( QVariant::Type t )
{
	return 
		(t == QVariant::Color)
		|| (t == QVariant::Double)
		|| (t == QVariant::Int)
		|| (t == QVariant::Line)
		|| (t == QVariant::LineF)
		|| (t == QVariant::LongLong)
		|| (t == QVariant::Rect)
		|| (t == QVariant::RectF)
		|| (t == QVariant::Point)
		|| (t == QVariant::PointF)
		|| (t == QVariant::Size)
		|| (t == QVariant::String)
		|| (t == QVariant::StringList)
		|| (t == QVariant::ULongLong)
		|| (t == QVariant::UInt)
		;
}

bool QVariant_s11n::operator()( S11nNode & dest, QVariant const & src ) const
{
	const QVariant::Type vt = src.type();
	typedef S11nNodeTraits NT;
	NT::set( dest, "type", vt );
	bool ret = false;
	if( ! QVariant_s11n::canHandle(vt) )
	{
		//throw s11n::s11n_exception( "QVariant_s11n cannot serialize QVariants type() %d", vt );
		return false;
	}
	switch( vt )
	{
		case QVariant::Int:
			ret = true;
			NT::set( dest, "val", src.toInt() );
			break;
		case QVariant::UInt:
			ret = true;
			NT::set( dest, "val", src.toUInt() );
			break;
		case QVariant::LongLong:
			ret = true;
			NT::set( dest, "val", src.toLongLong() );
			break;
		case QVariant::Color:
			ret = s11n::serialize_subnode( dest, "val", QColor(src.value<QColor>()) );
			break;
		case QVariant::Double:
			ret = true;
			NT::set( dest, "val", src.toDouble() );
			break;
		case QVariant::Line:
			ret = s11n::serialize_subnode( dest, "val", src.toLine() );
			break;
		case QVariant::LineF:
			ret = s11n::serialize_subnode( dest, "val", src.toLineF() );
			break;
		case QVariant::Point:
			ret = s11n::serialize_subnode( dest, "val", src.toPoint() );
			break;
		case QVariant::PointF:
			ret = s11n::serialize_subnode( dest, "val", src.toPointF() );
			break;
		case QVariant::Rect:
			ret = s11n::serialize_subnode( dest, "val", src.toRect() );
			break;
		case QVariant::RectF:
			ret = s11n::serialize_subnode( dest, "val", src.toRectF() );
			break;
		case QVariant::Size:
			ret = s11n::serialize_subnode( dest, "val", src.toSize() );
			break;
		case QVariant::SizeF:
			ret = s11n::serialize_subnode( dest, "val", src.toSizeF() );
			break;
		case QVariant::String:
			ret = s11n::serialize_subnode( dest, "val", src.toString() );
			break;
		case QVariant::StringList:
			ret = s11n::serialize_subnode( dest, "val", src.toStringList() );
			break;
		case QVariant::ULongLong:
			ret = true;
			NT::set( dest, "val", src.toULongLong() );
			break;
		default:
			break;
	};
	return ret;
}
bool QVariant_s11n::operator()( S11nNode const & src, QVariant & dest )
{
	typedef S11nNodeTraits NT;
	int vt = NT::get( src, "type", static_cast<int>(QVariant::Invalid) );
	if( ! QVariant_s11n::canHandle( static_cast<QVariant::Type>(vt) ) )
	{
		return false;
	}
	bool ret = false;
	switch( vt )
	{
		case QVariant::Color:
		{
			QColor tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::Double:
		{
			double i = NT::get( src, "val", 0.0 );
			dest = QVariant( i );
			ret = true;
		}
			break;
		case QVariant::Int:
		{
			int i = NT::get( src, "val", 0 );
			dest = QVariant( i );
			ret = true;
		}
			break;
		case QVariant::Line:
		{
			QLine tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
		case QVariant::LineF:
		{
			QLineF tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
		case QVariant::LongLong:
		{
			qlonglong i = NT::get( src, "val", static_cast<qlonglong>(0) );
			dest = QVariant( i );
			ret = true;
		}
		break;
		case QVariant::Point:
		{
			QPoint tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::PointF:
		{
			QPointF tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::Rect:
		{
			QRect tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::RectF:
		{
			QRectF tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::Size:
		{
			QSize tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::SizeF:
		{
			QSizeF tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::String:
		{
			QString tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::StringList:
		{
			QStringList tmp; 
			if( (ret = s11n::deserialize_subnode( src, "val", tmp ) ) )
			{
				dest = QVariant(tmp);
			}
		}
			break;
		case QVariant::UInt:
		{
			uint i = NT::get( src, "val", static_cast<uint>(0) );
			dest = QVariant( i );
			ret = true;
		}
			break;
		case QVariant::ULongLong:
		{
			qulonglong i = NT::get( src, "val", static_cast<qulonglong>(0) );
			dest = QVariant( i );
			ret = true;
		}
			break;
		default:
			break;
	};
	return ret;
}


bool QObjectProperties_s11n::operator()( S11nNode & dest, QObject const & src ) const
{
	typedef QList<QByteArray> QL;
	QL ql( src.dynamicPropertyNames() );
	QL::const_iterator it( ql.begin() );
	QL::const_iterator et( ql.end() );
	for( ; et != it; ++it )
	{
		char const * key = it->constData();
		if( *key == '_' ) continue; // Qt reserves "_q_" prefix, so we'll elaborate on that.
		if( ! s11n::serialize_subnode( dest, key, src.property( key ) ) )
		{
			return false;
		}
	}
	return true;
}
bool QObjectProperties_s11n::operator()( S11nNode const & src, QObject & dest ) const
{
	typedef S11nNodeTraits::child_list_type CLT;
	CLT::const_iterator it = S11nNodeTraits::children(src).begin();
	CLT::const_iterator et = S11nNodeTraits::children(src).end();
	QString key;
	S11nNode const * node = 0;
	std::string nname;
	for( ; et != it; ++it )
	{
		node = *it;
		QVariant vari;
		if( ! s11n::deserialize( *node, vari ) ) return false;
		nname = S11nNodeTraits::name(*node);
		dest.setProperty( nname.c_str(), vari ); 
	}
	return true;
}
