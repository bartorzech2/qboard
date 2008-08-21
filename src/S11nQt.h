#ifndef S11NQT_H
#define S11NQT_H
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

/************************************************************************
This file declares some bindings for Qt and libs11n, providing
serialization support for many common Qt types.
************************************************************************/

#include "S11n.h"
#include "S11nQtStream.h"
#include <sstream>
#include <memory>
namespace s11n { namespace qt {

    /**
       Serializes src to dest using libs11n.
       SerializableT must be a non-cv-qualified
       Serializable type.
    */
    template <typename SerializableT>
    bool serialize( QIODevice & dest, SerializableT const & src )
    {
	std::ostringstream dummy;
	StdToQtOBuf sentry( dummy, dest );
	return s11nlite::save( src, dummy );
    }

    /**
       Tries to deserialize a new SerializableT
       object from src using libs11n.
       SerializableT must be a non-cv-qualified
       Serializable type and src must contain data
       for that type. On error it returns a 0 pointer
       or propogates an exception. On success it returns
       a new object, which the caller owns.
    */
    template <typename SerializableT>
    SerializableT * deserialize( QIODevice & src )
    {
	std::istringstream dummy;
	StdToQtIBuf sentry( dummy, src );
	return s11nlite::load_serializable<SerializableT>( dummy );
    }

    /**
       Deserializes dest from src using libs11n.
       SerializableT must be a non-cv-qualified
       Serializable type. Note that deserializing
       this way cannot support polymorphic deserialization
       of SerializableT. If that is important to you,
       use the deserialize(QIODevice&), as it can support
       polymorphic SerializableT.
    */
    template <typename SerializableT>
    bool deserialize( QIODevice & src, SerializableT & dest )
    {
	std::istringstream dummy;
	StdToQtIBuf sentry( dummy, src );
	typedef std::auto_ptr<S11nNode> AP;
	AP n( s11nlite::load_node( dummy ) );
	return n.get()
	    ? s11nlite::deserialize( *n, dest )
	    : false;
    }
}} // namespaces
/************************************************************************
At this point in the file, one might ask why "some" of the code is
in the s11n::qt namespace and "some" is not. The answers are:

a) i vaguely remember having problems with Qt and namespaces some
years ago, so i don't normally use namespaces in Qt-based code.

b) Historical reasons. i would ideally like to see all of it in a
namespace, but it's a lot of repetative editing to fix it, so it ain't
gonna get fixed yet.

c) right now i'd rather write silly code comments than go and do the morally
right thing and move all the code into namespaces.

 ************************************************************************/


#include <QByteArray>
/**
   An s11n proxy for QByteArrays. See the compressionThreshold member
   for important notes.

   Note that since you can read/write directly from/to QByteArrays
   using the QIODevice interface, it is in principal possible to save
   arbitrary binary data using libs11n this way. Not necessarily an
   efficient way to save binary data, but maybe useful for some
   cases (e.g. saving QPixmaps as inlined data).
*/
struct QByteArray_s11n
{
    /**
       Serializes src to dest, saving the data in a bin64-encoded
       string. If src.size() is greater than
       QByteArray_s11n::compressionThreshold then the serialized
       copy is compressed using qCompress() before bin64 encoding
       is applied.

       See the compressionThreshold for more info.
    */
    bool operator()( S11nNode & dest, QByteArray const & src ) const;

    /** Deserializes dest from src. */
    bool operator()( S11nNode const & src, QByteArray & dest ) const;

    /**
       The value of compressionThreshold influences how the serialize
       operator works.

       It is intended that an application set this only once, and not
       twiddle it back and forth.

       If set to 0 then compression is disabled. The default value is
       quite small - technically undefined, but "probably" around 100.
    */
    static unsigned short compressionThreshold;
};
/* s11n proxy for QByteArray. */
#define S11N_TYPE QByteArray
#define S11N_TYPE_NAME "QByteArray"
#define S11N_SERIALIZE_FUNCTOR QByteArray_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>


#include <QColor>
/* s11n proxy for QColor. */
#define S11N_TYPE QColor
#define S11N_TYPE_NAME "QColor"
#define S11N_SERIALIZE_FUNCTOR QColor_s11n
#include "reg_qt_s11n.h"

#include <QDate>
/* Register QDate_s11n as a proxy for QDate. */
#define S11N_TYPE QDate
#define S11N_TYPE_NAME "QDate"
#define S11N_SERIALIZE_FUNCTOR QDate_s11n
#include "reg_qt_s11n.h"

#include <QDateTime>
/* Register QDateTime_s11n as a proxy for QDateTime. */
#define S11N_TYPE QDateTime
#define S11N_TYPE_NAME "QDateTime"
#define S11N_SERIALIZE_FUNCTOR QDateTime_s11n
#include "reg_qt_s11n.h"


#include <QFont>
/* s11n proxy for QFont.

Handles only the following properties of QFont:

- weight
- fontFamily
- pointSizeF
- bold
- italic
*/
#define S11N_TYPE QFont
#define S11N_TYPE_NAME "QColor"
#define S11N_SERIALIZE_FUNCTOR QFont_s11n
#include "reg_qt_s11n.h"

#include <QLine>
/* s11n proxy for QLine. */
#define S11N_TYPE QLine
#define S11N_TYPE_NAME "QLine"
#define S11N_SERIALIZE_FUNCTOR QLine_s11n
#include "reg_qt_s11n.h"

#include <QLineF>
/* s11n proxy for QLineF. */
#define S11N_TYPE QLineF
#define S11N_TYPE_NAME "QLineF"
#define S11N_SERIALIZE_FUNCTOR QLineF_s11n
#include "reg_qt_s11n.h"

#include <QRectF>
/* s11n proxy for QRectF. */
#define S11N_TYPE QRectF
#define S11N_TYPE_NAME "QRectF"
#define S11N_SERIALIZE_FUNCTOR QRectF_s11n
#include "reg_qt_s11n.h"

#include <QRect>
/* s11n proxy for QRect. */
#define S11N_TYPE QRect
#define S11N_TYPE_NAME "QRect"
#define S11N_SERIALIZE_FUNCTOR QRect_s11n
#include "reg_qt_s11n.h"

#include <QPair>
#include <s11n.net/s11n/proxy/mapish.hpp>
#define S11N_TEMPLATE_TYPE QPair
#define S11N_TEMPLATE_TYPE_NAME "QPair"
#define S11N_TEMPLATE_TYPE_PROXY ::s11n::map::serialize_pair_f
#define S11N_TEMPLATE_TYPE_DESER_PROXY ::s11n::map::deserialize_pair_f
#include <s11n.net/s11n/proxy/reg_s11n_traits_template2.hpp>

namespace s11n {
	template <typename T1, typename T2>
	struct default_cleanup_functor< QPair<T1,T2> >
	{
		typedef T1 first_type;
		typedef T2 second_type;
		typedef QPair<T1,T2> serializable_type;
		void operator()( serializable_type & p ) const
		{
			using namespace s11n::debug;
			S11N_TRACE(TRACE_CLEANUP) << "default_cleanup_functor<> specialization cleaning up QPair<>...\n";
			typedef typename type_traits<first_type>::type TR1;
			typedef typename type_traits<second_type>::type TR2;
			cleanup_serializable<TR1>( p.first );
			cleanup_serializable<TR2>( p.second );
		}
	};
}

#include <QPixmap>
/*
s11n proxy for QPixmap. It actually uses QByteArray_s11n
to store the data.
*/
#define S11N_TYPE QPixmap
#define S11N_TYPE_NAME "QPixmap"
#define S11N_SERIALIZE_FUNCTOR QPixmap_s11n
#include "reg_qt_s11n.h"


#include <QPoint>
/* s11n proxy for QPoint. */
#define S11N_TYPE QPoint
#define S11N_TYPE_NAME "QPoint"
#define S11N_SERIALIZE_FUNCTOR QPoint_s11n
#include "reg_qt_s11n.h"

#include <QPointF>
/* s11n proxy for QPointF. */
#define S11N_TYPE QPointF
#define S11N_TYPE_NAME "QPointF"
#define S11N_SERIALIZE_FUNCTOR QPointF_s11n
#include "reg_qt_s11n.h"

#include <QSize>
/* s11n proxy for QSize. */
#define S11N_TYPE QSize
#define S11N_TYPE_NAME "QSize"
#define S11N_SERIALIZE_FUNCTOR QSize_s11n
#include "reg_qt_s11n.h"

#include <QSizeF>
/* s11n proxy for QSizeF. */
#define S11N_TYPE QSizeF
#define S11N_TYPE_NAME "QSizeF"
#define S11N_SERIALIZE_FUNCTOR QSizeF_s11n
#include "reg_qt_s11n.h"

#include <QString>
/**
	s11n proxy for QStrings. See the tryAscii member for important
	notes.

	As a special convenience, it can deserialize QByteArray data,
	in the assumption that it's a string. This can be used to take
	advantage of QByteArray_s11n's compression when you know that
	a given property will often contain large amounts of text.
*/
struct QString_s11n
{
	/** Serializes src to dest. See the tryAscii member for details. */
	bool operator()( S11nNode & dest, QString const & src ) const;
	/** Deserializes dest from src. It can read either format of
		serialized string (see tryAscii). */
	bool operator()( S11nNode const & src, QString & dest ) const;
	
	/**
		If tryAscii is true (the default) then serialized QStrings will be checked
		to see if they are made up of only ASCII values [0..127]. If so, then a human-readable
		representation is serialized, otherwise an unreadable (but unicode-friendly) format
		is used.
		
		It is intended that an application set this only once, and not twiddle it back and forth.
	*/
	static bool tryAscii;
};
/* Register QString_s11n as a proxy for QString. */
#define S11N_TYPE QString
#define S11N_TYPE_NAME "QString"
#define S11N_SERIALIZE_FUNCTOR QString_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#include <QStringList>
/* s11n proxy for QStringList. */
#define S11N_TYPE QStringList
#define S11N_TYPE_NAME "QStringList"
#define S11N_SERIALIZE_FUNCTOR QStringList_s11n
#include "reg_qt_s11n.h"

#include <QTime>
/* Register QTime_s11n as a proxy for QTime. */
#define S11N_TYPE QTime
#define S11N_TYPE_NAME "QTime"
#define S11N_SERIALIZE_FUNCTOR QTime_s11n
#include "reg_qt_s11n.h"


#include <QVariant>
/* s11n proxy for QVariant.

It supports the following QVariant types (as defined in the
QVariant::Type enum):

- String, StringList
- Color (only ARGB format)
- Point, PointF
- Size, SizeF
- Rect, RectF
- Int, UInt
- Line, LineF
- List, Map
- LongLong, ULongLong
- Time, Date, DateTime
- ByteArray
- Pixmap

If an attempt is made to serialize a different type, serialization
will fail. If, upon deserialization, the integer values defined by
QVariant::Types have changed since the serialization, results are
undefined, but will most likely result in an invalid QVariant value.
*/
struct QVariant_s11n
{
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QVariant const & src ) const;
	/** Deserializes dest from src. If this routine returns false,
	dest is guaranteed to have not been modified. That is, if
	this routine fails, dest's state is the same as it was
	before this function was called. */
	bool operator()( S11nNode const & src, QVariant & dest ) const;
	/**
	   Returns true only if this type can de/serialize QVariants
	   of the given type.
	*/
	static bool canHandle( QVariant::Type t );
};
#define S11N_TYPE QVariant
#define S11N_TYPE_NAME "QVariant"
#define S11N_SERIALIZE_FUNCTOR QVariant_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

/**
	A "partial" s11n proxy which de/serializes QObject "dynamic"
	properties (those set with QObject::setProperty()).	See QVariant_s11n
	for the supported property types. Other	properties will cause
	de/serialization to fail.
*/
struct QObjectProperties_s11n
{
    /**
       Serializes all "dynamic" Properties from the src object to dest.
       Each propery is saved in a separate subnode, named after the property.
       Properties which begin with an underscore are skipped.
    */
    bool operator()( S11nNode & dest, QObject const & src ) const;
    /**
       Deserializes subnodes of src and inserts them as properties
       of dest from src. If this routine returns false, dest may be partially
       deserialized - its state is effectively undefined.

       As per s11n convention, this routine clears out any existing
       properties of dest before populating it.
    */
    bool operator()( S11nNode const & src, QObject & dest ) const;

};

#endif
