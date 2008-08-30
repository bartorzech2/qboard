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

#include <QSharedData>
#include <QSharedDataPointer>
#include <QMetaType>

#include "S11n.h"
#include "S11nQt/Stream.h"

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
       Given one of the following strings, it returns the appropriate Qt::PenStyle
       value, or NoPen if it finds no match:
		
       "SolidLine", "DashLine", "DotLine", "DashDotLine", "DashDotDotLine"
    */
    Qt::PenStyle stringToPenStyle( QString const & );
    /**
       The opposite of stringToPenStyle(). Returns "NoPen" if pen
       is not a known Qt::PenStyle. 
    */
    QString penStyleToString( int pen );

    /**
       Like stringToPenStyle(), but works on Qt::BrushStyle names.
    */
    Qt::BrushStyle stringToBrushStyle( QString const & );
    /**
       The opposite of stringToBrushStyle(). Returns "NoBrush" if st
       is not a known Qt::BrushStyle. 
    */
    QString brushStyleToString( int st );


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

    /**
       SharedS11nNode is a QSharedData type implementing
       a reference-counted S11nNode.
    */
    struct SharedS11nNode : public QSharedData
    {
    public:
	S11nNode node;
	SharedS11nNode() : QSharedData(), node()
	{}
	SharedS11nNode( const SharedS11nNode & rhs )
	    : QSharedData(rhs), node( rhs.node )
	{
	}
#if 0
	SharedS11nNode & operator=( SharedS11nNode const & rhs )
	{
	    if( this != rhs )
	    {
		this->node = rhs.node;
	    }
	    return *this;
	}
#endif
    };

    /**
       VariantS11n is type to allow QVariant to hold a *copy* of any
       Serializable.
    */
    class VariantS11n
    {
    public:
	/**
	   Returns the preferred type name for use with QVariant.
	*/
	static char const * variantTypeName()
	{
	    return "VariantS11n";
	}
	/**
	   Returns the id for VariantS11n which is registered via
	   qRegisterMetaType<VariantS11n>().
	*/
	static int variantType()
	{
	    static int bob = qRegisterMetaType<VariantS11n>(variantTypeName());
	    return bob;
	}

	VariantS11n()
	    : proxy( new SharedS11nNode )
	{
	}
	/**
	   Makes a deep copy of n.
	 */
	VariantS11n( S11nNode const & n)
	    : proxy( new SharedS11nNode )
	{
	    proxy->node = n;
	}
	/**
	   Makes a ref-counted copy of src.
	*/
	VariantS11n & operator=( VariantS11n const & src )
	{
	    this->proxy = src.proxy;
	    return *this;
	}
	/**
	   Makes a ref-counted copy of src.
	*/
	VariantS11n( VariantS11n const & src )
	    : proxy(src.proxy)
	{
	}
	/**
	   Calls this->serialize(src). If serialization fails an exception
	   is thrown.
	*/
	template <typename SerT>
	VariantS11n( SerT const & src )
	    : proxy(new SharedS11nNode)
	{
	    if( ! this->serialize( src ) )
	    {
		QString msg = QString("VariantS11n(<Serializable[%1]> const &): serialization failed").
		    arg(s11n::s11n_traits<SerT>::class_name( &src ).c_str());
		throw std::runtime_error( msg.toAscii().constData() );
	    }
	}
	~VariantS11n()
	{}

	/**
	   Returns true if this object contains no serialized data.
	*/
	bool empty() const
	{
	    return S11nNodeTraits::empty(proxy->node);
	}

	/**
	   Equivalent to s11n::serialize(this->node(), src).
	*/
	template <typename SerT>
	bool serialize( SerT const & src )
	{
	    S11nNodeTraits::clear(proxy->node);
	    return s11n::serialize(proxy->node, src );
	}

	/**
	   Equivalent to s11n::deserialize(this->node(), dest).
	*/
	template <typename SerT>
	bool deserialize( SerT & dest ) const
	{
	    return s11n::deserialize( proxy->node, dest );
	}

	/**
	   Equivalent to s11n::deserialize<SerT>(this->node()). The
	   caller owns the returned pointer, which may be 0 on error.
	*/
	template <typename SerT>
	SerT * deserialize() const
	{
	    return s11n::deserialize<SerT>( proxy->node );
	}

	/**
	   Non-const accesor for this object's S11nNode.  Calling this
	   will force a deep copy if other VariantS11n objects are sharing
	   the underlying node.
	*/
	S11nNode & node() { return this->proxy->node; }
	/**
	   Const accesor for this object's S11nNode.
	*/
	S11nNode const & node() const { return this->proxy->node; }

	/**
	   Clears the memory used by serialized data, making
	   this->node() usable again as a target for serialization.
	*/
	void clear()
	{
	    S11nNodeTraits::clear( proxy->node );
	}
    private:
	static const int registration;
	QSharedDataPointer<SharedS11nNode> proxy;
    };

    /**
       A "partial" s11n proxy which de/serializes QObject "dynamic"
       properties (those set with QObject::setProperty()). See
       QVariant_s11n for the supported property types. Other
       properties will cause de/serialization to fail.
    */
    struct QObjectProperties_s11n
    {
	/**
	   Serializes all "dynamic" Properties from the src object to
	   dest.  Each propery is saved in a separate subnode, named
	   after the property.  Properties which begin with an
	   underscore are skipped.
	*/
	bool operator()( S11nNode & dest, QObject const & src ) const;
	/**
	   Deserializes subnodes of src and inserts them as properties
	   of dest from src. If this routine returns false, dest may
	   be partially deserialized - its state is effectively
	   undefined.

	   As per s11n convention, this routine clears out any
	   existing properties of dest before populating it.
	*/
	bool operator()( S11nNode const & src, QObject & dest ) const;

    };

}} // namespaces
using s11n::qt::VariantS11n; // kludge: we don't want the namespace part stored by QMetaType
Q_DECLARE_METATYPE(VariantS11n);

#endif
