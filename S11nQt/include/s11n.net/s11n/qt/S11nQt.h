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

#include <s11n.net/s11n/s11nlite.hpp>
//#include <s11n.net/s11n/s11n_debuggering_macros.hpp>

#include <sstream>
#include <memory>
namespace s11n { namespace qt {

    /* Used by the s11n-related algorithms. */
    typedef s11nlite::node_type S11nNode;
    /* Used by the s11n-related algorithms. */
    typedef s11nlite::node_traits S11nNodeTraits;

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

#endif
