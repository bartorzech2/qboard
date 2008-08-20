#ifndef PROPOBJ_H
#define PROPOBJ_H
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

#include <QRect>
#include <QObject>
#include <QVariant>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QString>

#include "Serializable.h"

/**
 */
class PropObj : public QObject, public Serializable
{
private:
    Q_OBJECT
    PropObj( PropObj const & ); // not yet implemented!
    PropObj & operator=( PropObj const & ); // not yet implemented.
public:
    /** Constructs an empty piece. */
    PropObj();
    virtual ~PropObj();
    virtual bool event( QEvent * e );
public Q_SLOTS:
    /** Serializes this object to dest. */
    virtual bool serialize( S11nNode & dest ) const;

    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );
	
    /** Returns true if this object has the given dynamic property, else false. */
    bool hasProperty( char const * );

    void copyPropertiesTo( QObject * dest ) const;
    void copyPropertiesFrom( QObject const * src );

Q_SIGNALS:
    /**
       Signal fired when properties are changed via
       setProperty().
    */
    void propertySet( char const *, QVariant const & );

};

// Register PropObj with s11n:
#define S11N_TYPE PropObj
#define S11N_TYPE_NAME "PropObj"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
// The s11nmeisters out there will know why we add a second registration here:
#define S11N_TYPE PropObj
#define S11N_TYPE_NAME "PropObj"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // PROPOBJ_H
