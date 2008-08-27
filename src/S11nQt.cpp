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

#include <QChar>
#include <QBuffer>
#include <QDebug>


#include "S11nQt.h"
#include <s11n.net/s11n/functional.hpp>

#include "S11nQtList.h"
#include "S11nQtMap.h"

typedef unsigned long long hashval_t; // FIXME: some platforms don't have long long
static const hashval_t hashval_t_err = static_cast<hashval_t>(-1);
static hashval_t hash_cstring_djb2( void const * vstr)
{ /* "djb2" algo code taken from: http://www.cse.yorku.ca/~oz/hash.html */
    if( ! vstr ) return hashval_t_err;
    hashval_t hash = 5381;
    int c = 0;
    char const * str = (char const *)vstr;
    if( ! str ) return hashval_t_err;
    while( (c = *str++) )
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


unsigned short s11n::qt::QByteArray_s11n::compressionThreshold = 100;
bool s11n::qt::QByteArray_s11n::operator()( S11nNode & dest, QByteArray const & src ) const
{
    if( src.isEmpty() ) return true;
    typedef s11nlite::node_traits NT;
    bool zIt = QByteArray_s11n::compressionThreshold
	? ((src.size() > QByteArray_s11n::compressionThreshold) ? true : false)
	: false;
    QByteArray b64( QByteArray( zIt
				? qCompress(src)
				: src ).toBase64() );
    b64.append('\0');
    NT::set( dest, "djb2", hash_cstring_djb2( b64.constData() ) );
    NT::set( dest, "bin64", std::string( b64.constData(), b64.size()-1 ) );
    return true;
}
bool s11n::qt::QByteArray_s11n::operator()( S11nNode const & src, QByteArray & dest ) const
{
	typedef s11nlite::node_traits NT;
	QByteArray tmp;
	{
	    std::string data( NT::get( src, "bin64", std::string() ) );
	    if( data.empty() ) return true;
	    tmp = QByteArray( data.c_str(), int(data.size()) );
	}
	hashval_t ck1 = NT::get( src, "djb2", hashval_t_err );
	if( hashval_t_err != ck1 )
	{
	    hashval_t ck2 = hash_cstring_djb2( tmp.constData() );
	    if( ck2 != ck1 )
	    {
		throw s11n::s11n_exception("QByteArray deserialize failed: checksums don't match. Expected [%ull] got [%ull].",
					   ck1, ck2 );
	    }
	}
	tmp = QByteArray::fromBase64( tmp );
	dest = qUncompress( tmp );
	return true;
}

bool QColor_s11n::operator()( S11nNode & dest, QColor const & src ) const
{
	typedef s11nlite::node_traits NT;
	std::ostringstream os;
	os << src.red() << ' '
	   << src.green() << ' '
	   << src.blue() << ' '
	   << src.alpha();
	NT::set( dest, "rgba", os.str() );
	return true;
}
bool QColor_s11n::operator()( S11nNode const & src, QColor & dest ) const
{
	typedef s11nlite::node_traits NT;
	std::string key("rgba");
	bool alphaKey = true;
	std::string sval = NT::get( src, key, std::string() );
	if( sval.empty() )
	{
	    alphaKey = false;
	    key = "rgb";
	    sval = NT::get( src, key, std::string() );
	}
	if( sval.empty() ) return false;
	std::istringstream is( sval );
	QColor nil;
	int fl;
#define DO(F) is >> fl; if( is.good() ) nil.F(fl);
	DO(setRed);
	DO(setGreen);
	DO(setBlue);
#undef DO
	if( alphaKey )
	{
	    int a;
	    is >> a;
	    nil.setAlpha( a );
	}
#if 0
	qreal a = NT::get( src, "alpha", -1.0 );
	if( a >= 0 )
	{
	    if( a > 1 ) nil.setAlpha(int(a));
	    else nil.setAlphaF(a);
	}
#endif
	dest = nil;
	return true;	
}


bool QDate_s11n::operator()( S11nNode & dest, QDate const & src ) const
{
	typedef S11nNodeTraits NT;
	std::ostringstream os;
	os << ' ' <<  src.year()
	   << ' ' <<  src.month()
	   << ' ' <<  src.day();
	NT::set( dest, "ymd", os.str() );
	return true;
}
bool QDate_s11n::operator()( S11nNode const & src, QDate & dest ) const
{
	typedef S11nNodeTraits NT;
	std::istringstream is(NT::get( src, "ymd", std::string() ));
	if( ! is.good() ) return false;
	int y;
	is >> y;
	if( ! is.good() ) return false;
	int m;
	is >> m;
	if( ! is.good() ) return false;
	int d;
	is >> d;
	dest.setDate( y, m, d );
	return true;	
}

bool QDateTime_s11n::operator()( S11nNode & dest, QDateTime const & src ) const
{
    return s11n::serialize_subnode( dest, "date", src.date() )
	&& s11n::serialize_subnode( dest, "time", src.time() );
}

bool QDateTime_s11n::operator()( S11nNode const & src, QDateTime & dest ) const
{
    QDate d;
    QTime t;
    if( ! ( s11n::deserialize_subnode( src, "date", d )
	    && s11n::deserialize_subnode( src, "time", t ) ) )
    {
	return false;
    }
    dest.setDate(d);
    dest.setTime(t);
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
bool QFont_s11n::operator()( S11nNode const & src, QFont & dest ) const
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
#if 0
    return s11n::serialize_subnode( dest, "p1", src.p1() )
	&& s11n::serialize_subnode( dest, "p2", src.p2() );
#else // older way (pre-20080824)
    typedef s11nlite::node_traits NT;
    std::ostringstream os;
    os << src.x1() << ' '
       << src.y1() << ' '
       << src.x2() << ' '
       << src.y2();
    NT::set( dest, "xyXY", os.str() );
#endif
    return true;
}
bool QLineF_s11n::operator()( S11nNode const & src, QLineF & dest ) const
{
    typedef s11nlite::node_traits NT;
    std::string xystr( NT::get( src, "xyXY", std::string() ) );
    if( ! xystr.empty() )
    { // older format
	std::istringstream is( xystr );
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
#if 0
    QPointF p1;
    QPointF p2;
    if(  s11n::deserialize_subnode( src, "p1", p1 )
	 && s11n::deserialize_subnode( src, "p2", p2 ) )
    {
	dest.setPoints( p1, p2 );
	return true;
    }
#endif
    return false;
}

bool QLine_s11n::operator()( S11nNode & dest, QLine const & src ) const
{
	return QLineF_s11n()( dest, QLineF(src) );
}
bool QLine_s11n::operator()( S11nNode const & src, QLine & dest ) const
{
	QLineF proxy;
	if( QLineF_s11n()( src, proxy ) )
	{
		dest = proxy.toLine();
		return true;
	}
	return false;
}

bool QMatrix_s11n::operator()( S11nNode & dest, QMatrix const & src ) const
{
    typedef s11nlite::node_traits NT;
    std::ostringstream os;
    os << src.m11() << ' '
       << src.m12() << ' '
       << src.m21() << ' '
       << src.m22() << ' '
       << src.dx() << ' '
       << src.dy();
    NT::set( dest, "matrix", os.str() );
    return true;
}

bool QMatrix_s11n::operator()( S11nNode const & src, QMatrix & dest ) const
{
    typedef s11nlite::node_traits NT;
    std::string str( NT::get( src, "matrix", std::string() ) );
    if( str.empty() ) return false;
    std::istringstream is( str );
    qreal m11, m12, m21, m22, x, y;
#define DO(V) if( ! is.good() ) return false; is >> V;
    DO(m11); DO(m12);
    DO(m21); DO(m22);
    DO(x); DO(y);
#undef DO
    dest.setMatrix( m11, m12, m21, m22, x, y );
    return true;
}


bool QPixmap_s11n::operator()( S11nNode & dest, QPixmap const & src ) const
{
	QByteArray ba;
	{
	    QBuffer buf(&ba);
	    if( ! src.save( &buf, "PNG" ) )
	    {
		return false;
	    }
	}
	return s11n::serialize_subnode( dest, "bytes", ba );
}
bool QPixmap_s11n::operator()( S11nNode const & src, QPixmap & dest ) const
{
    dest = QPixmap();
    QByteArray ba;
    if( ! s11n::deserialize_subnode( src, "bytes", ba ) )
    {
	return false;
    }
    return dest.loadFromData( ba );
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
bool QPointF_s11n::operator()( S11nNode const & src, QPointF & dest ) const
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
bool QPoint_s11n::operator()( S11nNode const & src, QPoint & dest ) const
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
bool QRectF_s11n::operator()( S11nNode const & src, QRectF & dest ) const
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
bool QRect_s11n::operator()( S11nNode const & src, QRect & dest ) const
{
	QRectF proxy;
	if( QRectF_s11n()( src, proxy ) )
	{
		dest = proxy.toRect();
		return true;
	}
	return false;
}

bool QRegExp_s11n::operator()( S11nNode & dest, QRegExp const & src ) const
{
	typedef S11nNodeTraits NT;
	if( ! s11n::serialize_subnode( dest, "pattern", src.pattern() ) ) return false;
	NT::set( dest, "syntax", int(src.patternSyntax()) );
	NT::set( dest, "case", int(src.caseSensitivity()) );
	NT::set( dest, "min", int(src.caseSensitivity()) );
	return true;
}
bool QRegExp_s11n::operator()( S11nNode const & src, QRegExp & dest ) const
{
	typedef S11nNodeTraits NT;
	QString p;
	if( ! s11n::deserialize_subnode( src, "pattern", p ) ) return false;
	dest.setPattern(p);
	int i = int(dest.caseSensitivity());
	dest.setCaseSensitivity( Qt::CaseSensitivity(NT::get( src, "case", i )) );
	i = int(dest.patternSyntax());
	dest.setPatternSyntax( QRegExp::PatternSyntax(NT::get( src, "syntax", i )) );
	dest.setMinimal( NT::get( src, "min", dest.isMinimal() ) );
	return true;	
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
bool QSizeF_s11n::operator()( S11nNode const & src, QSizeF & dest ) const
{
	typedef S11nNodeTraits NT;
	QSizeF nil;
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
bool QSize_s11n::operator()( S11nNode const & src, QSize & dest ) const
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
bool s11n::qt::QString_s11n::tryAscii = true;
bool s11n::qt::QString_s11n::operator()( S11nNode & dest, QString const & src ) const
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
bool s11n::qt::QString_s11n::operator()( S11nNode const & src, QString & dest ) const
{
	typedef s11nlite::node_traits NT;
	if( NT::is_set( src, "ascii" ) )
	{
		char const * cp = NT::get(src,"ascii",std::string()).c_str(); 
		dest = QString( cp ? cp : "" );
		return true;
	}
	else if( NT::is_set( src, "bin64" ) )
	{ // allow QByteArray data
	    QByteArray tmp;
	    if( ! s11n::deserialize( src, tmp ) ) return false;
	    dest = tmp;
	    return true;
	}
	else if( ! NT::is_set( src, "len" ) )
	{
		return false;
	}
	size_t len = NT::get( src, "len", size_t(0) );
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
	long sz = src.size();
	return (sz>=0) && (sz == long(S11nNodeTraits::children(dest).size()));
}
bool QStringList_s11n::operator()( S11nNode const & src, QStringList & dest ) const
{
    dest.clear();
    typedef S11nNodeTraits NT;
    std::for_each( NT::children(src).begin(),
		   NT::children(src).end(),
		   s11n::deser_to_outiter_f<QString>( std::back_inserter(dest) ) ); 
    long sz = dest.size();
    return (sz>=0) && (sz == long(S11nNodeTraits::children(src).size()));
}


bool QTime_s11n::operator()( S11nNode & dest, QTime const & src ) const
{
	typedef S11nNodeTraits NT;
	std::ostringstream os;
	os << src.hour()
	   << ' ' <<  src.minute()
	   << ' ' <<  src.second()
	   << ' ' <<  src.msec();
	NT::set( dest, "hmsm", os.str() );
	return true;
}
bool QTime_s11n::operator()( S11nNode const & src, QTime & dest ) const
{
	typedef S11nNodeTraits NT;
	std::istringstream is(NT::get( src, "hmsm", std::string() ));
	if( ! is.good() ) return false;
	int h;
	is >> h;
	if( ! is.good() ) return false;
	int m;
	is >> m;
	if( ! is.good() ) return false;
	int s;
	is >> s;
	if( ! is.good() ) return false;
	int ms;
	is >> ms;
	dest.setHMS( h, m, s, ms );
	return true;	
}

bool QTransform_s11n::operator()( S11nNode & dest, QTransform const & src ) const
{
    typedef s11nlite::node_traits NT;
    std::ostringstream os;
#define DO(V) os << src.V() << ' ';
    DO(m11); DO(m12); DO(m13);
    DO(m21); DO(m22); DO(m23);
    DO(m31); DO(m32); os << src.m33();
#undef DO
    NT::set( dest, "matrix", os.str() );
    return true;
}

bool QTransform_s11n::operator()( S11nNode const & src, QTransform & dest ) const
{
    typedef s11nlite::node_traits NT;
    std::string str( NT::get( src, "matrix", std::string() ) );
    if( str.empty() ) return false;
    std::istringstream is( str );
    qreal m11, m12, m13, m21, m22, m23, m31, m32, m33;
#define DO(V) if( ! is.good() ) return false; is >> V;
    DO(m11); DO(m12); DO(m13);
    DO(m21); DO(m22); DO(m23);
    DO(m31); DO(m32); DO(m33);
#undef DO
    dest.setMatrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
    return true;
}


bool s11n::qt::QVariant_s11n::canHandle( int t )
{
	return 
	    (t == QVariant::ByteArray)
	    || (t == QVariant::Color)
	    || (t == QVariant::Date)
	    || (t == QVariant::DateTime)
	    || (t == QVariant::Double)
	    || (t == QVariant::Font)
	    || (t == QVariant::Int)
	    || (t == QVariant::Line)
	    || (t == QVariant::LineF)
	    || (t == QVariant::List)
	    || (t == QVariant::LongLong)
	    || (t == QVariant::Map)
	    || (t == QVariant::Matrix)
	    || (t == QVariant::Rect)
	    || (t == QVariant::RectF)
	    || (t == QVariant::RegExp)
	    || (t == QVariant::Pixmap)
	    || (t == QVariant::Point)
	    || (t == QVariant::PointF)
	    || (t == QVariant::Size)
	    || (t == QVariant::SizeF)
	    || (t == QVariant::String)
	    || (t == QVariant::StringList)
	    || (t == QVariant::Time)
	    || (t == QVariant::ULongLong)
	    || (t == QVariant::UInt)
	    || (t == QVariant::UserType) // Only certain ones
	    || (t == VariantS11n::variantType())
	    ;
}


typedef QMap<QString,int> VariantNameTypeIDMap;
#include <s11n.net/s11n/proxy/pod/int.hpp>
static VariantNameTypeIDMap & vNTMap()
{
    static VariantNameTypeIDMap bob;
    if( bob.isEmpty() )
    {
#define MAP(VT) \
	bob[# VT] = QVariant::VT;		\
	bob[QString("%1").arg(int(QVariant::VT))] = QVariant::VT;

	MAP(BitArray);
	MAP(Bitmap);
	MAP(Bool);
	MAP(Brush);
	MAP(ByteArray);
	MAP(Char);
	MAP(Color);
	MAP(Cursor);
	MAP(Date);
	MAP(DateTime);
	MAP(Double);
	MAP(Font);
	MAP(Icon);
	MAP(Image);
	MAP(Int);
	MAP(Invalid);
	MAP(KeySequence);
	MAP(Line);
	MAP(LineF);
	MAP(List);
	MAP(Locale);
	MAP(LongLong);
	MAP(Map);
	MAP(Matrix);
	MAP(Transform);
	MAP(Palette);
	MAP(Pen);
	MAP(Pixmap);
	MAP(Point);
	MAP(PointF);
	MAP(Polygon);
	MAP(Rect);
	MAP(RectF);
	MAP(RegExp);
	MAP(Region);
	MAP(Size);
	MAP(SizeF);
	MAP(SizePolicy);
	MAP(String);
	MAP(StringList);
	MAP(TextFormat);
	MAP(TextLength);
	MAP(Time);
	MAP(UInt);
	MAP(ULongLong);
	MAP(Url);
	MAP(UserType);
#undef MAP
	using s11n::qt::VariantS11n;
	bob[VariantS11n::variantTypeName()] = VariantS11n::variantType();
	bob[QString("%1").arg(VariantS11n::variantType())] = VariantS11n::variantType();

 	//COUT << "vNTMap()==\n"; s11nlite::save(bob,std::cout);
    }
    return bob;
}

static int variantTypeID( QString const & vt )
{
    return vNTMap().value( vt, QVariant::Invalid );
}
typedef QMap<int,QString> VariantTypeIDNameMap;
static VariantTypeIDNameMap & vTNMap()
{
    static VariantTypeIDNameMap bob;
    if( bob.isEmpty() )
    {
#define MAP(VT) \
	bob[QVariant::VT] = # VT;

	MAP(BitArray);
	MAP(Bitmap);
	MAP(Bool);
	MAP(Brush);
	MAP(ByteArray);
	MAP(Char);
	MAP(Color);
	MAP(Cursor);
	MAP(Date);
	MAP(DateTime);
	MAP(Double);
	MAP(Font);
	MAP(Icon);
	MAP(Image);
	MAP(Int);
	MAP(Invalid);
	MAP(KeySequence);
	MAP(Line);
	MAP(LineF);
	MAP(List);
	MAP(Locale);
	MAP(LongLong);
	MAP(Map);
	MAP(Matrix);
	MAP(Transform);
	MAP(Palette);
	MAP(Pen);
	MAP(Pixmap);
	MAP(Point);
	MAP(PointF);
	MAP(Polygon);
	MAP(Rect);
	MAP(RectF);
	MAP(RegExp);
	MAP(Region);
	MAP(Size);
	MAP(SizeF);
	MAP(SizePolicy);
	MAP(String);
	MAP(StringList);
	MAP(TextFormat);
	MAP(TextLength);
	MAP(Time);
	MAP(UInt);
	MAP(ULongLong);
	MAP(Url);
	MAP(UserType);
	bob[s11n::qt::VariantS11n::variantType()] = s11n::qt::VariantS11n::variantTypeName();
#undef MAP
 	//COUT << "vTNMap()==\n"; s11nlite::save(bob,std::cout);
    }
    return bob;
}

QString variantTypeName( int vt )
{
    return vTNMap().value( vt, "Invalid" );
}

#include "S11nQtList.h"
#include "S11nQtMap.h"
bool s11n::qt::QVariant_s11n::operator()( S11nNode & dest, QVariant const & src ) const
{
	const int vt = src.userType();
	if( ! QVariant_s11n::canHandle(vt) )
	{
		//throw s11n::s11n_exception( "QVariant_s11n cannot serialize QVariants type() %d", vt );
		return false;
	}
	typedef S11nNodeTraits NT;
#if 0
	NT::set( dest, "type", vt );
#else
	NT::set( dest, "type", variantTypeName(vt).toAscii().constData() );
#endif

#define CASE_PROP(T,Setter) \
	case QVariant::T: \
	NT::set(dest, "val", src.Setter); \
	return true;
#define CASE_OBJ(T,X) \
	case QVariant::T: \
	return s11n::serialize_subnode( dest, "val", X ); \

	switch( vt )
	{
	    CASE_OBJ(ByteArray, src.toByteArray() );
	    CASE_PROP(Int,toInt());
	    CASE_PROP(UInt,toUInt());
	    CASE_PROP(LongLong,toLongLong());
	    CASE_OBJ(Color, src.value<QColor>() );
	    CASE_OBJ(Date, src.toDate() );
	    CASE_OBJ(DateTime, src.toDateTime() );
	    CASE_PROP(Double,toDouble());
	    CASE_OBJ(Font, src.value<QFont>());
	    CASE_OBJ(Line, src.toLine() );
	    CASE_OBJ(LineF, src.toLineF() );
	    CASE_OBJ(List, src.toList() );
	    CASE_OBJ(Matrix, src.value<QMatrix>() );
	    CASE_OBJ(Map, src.toMap() );
	    CASE_OBJ(Pixmap, src.value<QPixmap>() );
	    CASE_OBJ(Point, src.toPoint() );
	    CASE_OBJ(PointF, src.toPointF() );
	    CASE_OBJ(Rect, src.toRect() );
	    CASE_OBJ(RectF, src.toRectF() );
	    CASE_OBJ(RegExp, src.toRegExp() );
	    CASE_OBJ(Size, src.toSize() );
	    CASE_OBJ(SizeF, src.toSizeF() );
	    CASE_OBJ(String, src.toString() );
	    CASE_OBJ(StringList, src.toStringList() );
	    CASE_OBJ(Time, src.toTime() );
	    CASE_PROP(ULongLong,toULongLong());
	  default:
	      break;
	};
#undef CASE_PROP
#undef CASE_OBJ
	if( src.canConvert<VariantS11n>() )
	{
	    VariantS11n vsn( src.value<VariantS11n>() );
	    S11nNode & ch( s11n::create_child( dest, "val" ) );
	    S11nNodeTraits::swap( vsn.node(), ch );
	    return true;
	}
	return false;
}

bool s11n::qt::QVariant_s11n::operator()( S11nNode const & src, QVariant & dest ) const
{
	typedef S11nNodeTraits NT;
	std::string vtstr( NT::get( src, "type", std::string("0") ) );
	int vt = variantTypeID( vtstr.c_str() );
	if( ! canHandle( vt ) )
	{
	    qDebug() << "QVariant_s11n::deserialize: cannot handle type id "
		     << vt << "/" << vtstr.c_str();
	    return false;
	}
#define CASE_OBJ(VT,Type) \
	case QVariant::VT: { \
	    Type tmp;							\
	    if( s11n::deserialize_subnode( src, "val", tmp ) ) \
	    {dest = QVariant(tmp); return true;}	       \
	} break;
#define CASE_PROP(VT,Type,DefaultVal)		\
	case QVariant::VT: \
	{ Type i = NT::get( src, "val", DefaultVal ); \
	    dest = QVariant( i ); return true; \
	} break;

	switch( vt )
	{
	    CASE_OBJ(ByteArray,QByteArray);
	    CASE_OBJ(Color,QColor);
	    CASE_OBJ(Date,QDate);
	    CASE_OBJ(DateTime,QDateTime);
	    CASE_PROP(Double,double,0.0);
	    CASE_PROP(Int,int,0);
	    CASE_OBJ(Font,QFont);
	    CASE_OBJ(Line,QLine);
	    CASE_OBJ(LineF,QLineF);
	    CASE_OBJ(List,QList<QVariant>);
	    CASE_PROP(LongLong,qlonglong,qlonglong(0));
	    CASE_OBJ(Map,QVariantMap);
	    CASE_OBJ(Matrix,QMatrix);
	    CASE_OBJ(Pixmap,QPixmap);
	    CASE_OBJ(Point,QPoint);
	    CASE_OBJ(PointF,QPointF);
	    CASE_OBJ(Rect,QRect);
	    CASE_OBJ(RectF,QRectF);
	    CASE_OBJ(RegExp,QRegExp);
	    CASE_OBJ(Size,QSize);
	    CASE_OBJ(SizeF,QSizeF);
	    CASE_OBJ(String,QString);
	    CASE_OBJ(StringList,QStringList);
	    CASE_OBJ(Time,QTime);
	    CASE_PROP(UInt,uint,uint(0));
	    CASE_PROP(ULongLong,qulonglong,qulonglong(0));
	  default:
	      break;
	};
	using namespace s11n::qt;
	if( vt == VariantS11n::variantType() )
	{
	      S11nNode const * ch = s11n::find_child_by_name( src, "val" );
	      if( ch )
	      {
		  dest.setValue( VariantS11n(*ch) );
		  return true;
	      }
	      else
	      {
		  return false;
	      }
	}
#undef CASE_OBJ
#undef CASE_PROP
	return false;
}


static void QObjectProperties_s11n_clearProperties( QObject & obj )
{
    typedef QList<QByteArray> QL;
    QL ql( obj.dynamicPropertyNames() );
    QL::const_iterator it( ql.begin() );
    QL::const_iterator et( ql.end() );
    for( ; et != it; ++it )
    {
	char const * key = it->constData();
	if( key && (*key == '_') ) continue; // Qt reserves the "_q_" prefix, so we'll elaborate on that.
	obj.setProperty( key, QVariant() );
    }
}

bool s11n::qt::QObjectProperties_s11n::operator()( S11nNode & dest, QObject const & src ) const
{
	typedef QList<QByteArray> QL;
	QL ql( src.dynamicPropertyNames() );
	QL::const_iterator it( ql.begin() );
	QL::const_iterator et( ql.end() );
	for( ; et != it; ++it )
	{
		char const * key = it->constData();
		if( !key || (*key == '_') ) continue; // Qt reserves the "_q_" prefix, so we'll elaborate on that.
		if( ! s11n::serialize_subnode( dest, key, src.property( key ) ) )
		{
		    qDebug() << "QObjectProperties_s11n::serialize failed for key " <<QString(*it);
		    return false;
		}
	}
	return true;
}

bool s11n::qt::QObjectProperties_s11n::operator()( S11nNode const & src, QObject & dest ) const
{
    QObjectProperties_s11n_clearProperties( dest );
    typedef S11nNodeTraits::child_list_type CLT;
    CLT::const_iterator it = S11nNodeTraits::children(src).begin();
    CLT::const_iterator et = S11nNodeTraits::children(src).end();
    S11nNode const * node = 0;
    std::string nname;
    for( ; et != it; ++it )
    {
	node = *it;
	nname = S11nNodeTraits::name(*node);
	QVariant vari;
	if( ! s11n::deserialize( *node, vari ) )
	{
	    qDebug() << "QObjectProperties_s11n::deserialize failed for key:" <<nname.c_str();
	    //return false;
	    continue;
	}
	dest.setProperty( nname.c_str(), vari ); 
    }
    return true;
}
