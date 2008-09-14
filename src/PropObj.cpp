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

#include <QList>
#include <QByteArray>
#include <QVariant>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>
#include <QMap>
#include <QEvent>

#include <s11n.net/s11n/s11n.hpp>
#include <s11n.net/s11n/functional.hpp>

#include <iterator>
#include <algorithm>

#include <qboard/PropObj.h>
#include <qboard/utility.h>
#include <qboard/S11nQt.h>

// Qt bug? Q_EMIT is never getting defined for me
#ifndef Q_EMIT
#define Q_EMIT
#endif

PropObj::PropObj() : QObject(),
		     Serializable("PropObj")
{
}

PropObj::~PropObj() 
{
    QBOARD_VERBOSE_DTOR << "~PropObj()";
}



bool PropObj::hasProperty( char const * key )
{
    return this->property(key).isValid();
}

bool PropObj::event( QEvent * e )
{
    if( e->type() == QEvent::DynamicPropertyChange )
    {
	QDynamicPropertyChangeEvent *chev = dynamic_cast<QDynamicPropertyChangeEvent *>(e);
	if( ! chev ) return false;
	e->accept();
	char const * key = chev->propertyName().constData();
	Q_EMIT propertySet( key, this->property(key) );
	return true;
    }
    return QObject::event(e);
}


void PropObj::copyPropertiesTo( QObject * dest ) const
{
    qboard::copyProperties( this, dest );
}

void PropObj::copyPropertiesFrom( QObject const * src )
{
    qboard::copyProperties( src, this );
}

bool PropObj::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    return s11n::qt::QObjectProperties_s11n()( dest , *this );
}

bool PropObj::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    return s11n::qt::QObjectProperties_s11n()( src, *this );
}
