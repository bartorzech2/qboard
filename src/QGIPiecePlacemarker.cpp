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

#include "QGIPiecePlacemarker.h"
#include "S11nQt.h"
#include "utility.h"
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QFocusEvent>
#include <QDebug>
#include <QRectF>
#include <QPainterPath>
#include <QPainter>
#include <QRadialGradient>
#include <QColor>
#include <QPointF>
#include <QMenu>
#include <QEvent>
#include <cmath>

struct QGIPiecePlacemarker::Impl
{
    bool active;
    Impl() :
	active(false)
	{
		
	}
	~Impl()
	{
		
	}
};

QGIPiecePlacemarker::QGIPiecePlacemarker() :
    QObject(),
    QGraphicsItem(),
#if QGIPiecePlacemarker_IS_SERIALIZABLE
    Serializable("QGIPiecePlacemarker"),
#endif
    impl(new Impl)
{
    this->setProperty("color",QColor(Qt::yellow));
    this->setProperty("lineColor",QColor(Qt::red));
    this->setFlags( QGraphicsItem::ItemIsMovable );
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
#if QT_VERSION >= 0x040400
    this->setCacheMode(DeviceCoordinateCache);
#endif
}

QGIPiecePlacemarker::~QGIPiecePlacemarker()
{
	qDebug() << "~QGIPiecePlacemarker()";
	delete impl;
}


#include "utility.h"
void QGIPiecePlacemarker::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	if( (ev->buttons() & Qt::LeftButton) )
	{
	    ev->accept();
	    qreal zV = qboard::nextZLevel(this);
	    if( zV > this->zValue() )
	    {
		this->setZValue(zV);
	    }
	}
	this->QGraphicsItem::mousePressEvent(ev);
	//this->update();
}
void QGIPiecePlacemarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
	this->QGraphicsItem::mouseReleaseEvent(ev);
}

QRectF QGIPiecePlacemarker::boundingRect() const
{
    return QRectF(-12,-12,24,24);
}

// QPainterPath QGIPiecePlacemarker::shape() const
// {
// 	QPainterPath path;
// 	path.addEllipse(-10, -10, 20, 20);
// 	return path;
// }
bool QGIPiecePlacemarker::event( QEvent * e )
{
	if( e->type() == QEvent::DynamicPropertyChange )
	{
		e->accept();
		this->update();
		return true;
	}
	return QObject::event(e);
}
void QGIPiecePlacemarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	QVariant var = this->property("color");
	QColor color = (var.isValid() ? var.value<QColor>() : Qt::yellow);
	var = this->property("lineColor");
	QColor xline = (var.isValid() ? var.value<QColor>() : Qt::red);
	QRectF brect( this->boundingRect() );
	int fudge = 4; // try to avoid lopped off borders
	int w = int(brect.width()-fudge);
	int x = int(brect.x())+(fudge/2);
	int y = int(brect.y())+(fudge/2);
	int step = int(w / 6);
	int w2 = w;
	int h2 = int(brect.height()-fudge);
	bool which = true;
	while( x < 0 )
	{
	    QColor c( which ? xline : color );
	    which = !which;
	    painter->setBrush( c );
	    painter->setPen(QPen( c, Qt::SolidLine));
	    painter->drawEllipse(x,y,w2,h2);
	    x += step;
	    y += step;
	    w2 -= 2*step;
	    h2 -= 2*step;
	}
}


#if QGIPiecePlacemarker_IS_SERIALIZABLE
bool QGIPiecePlacemarker::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	if( this->metaObject()->propertyCount() )
	{
		S11nNode & pr( s11n::create_child( dest, "properties" ) );
		if( ! QObjectProperties_s11n()( pr, *this ) ) return false;
	}
	return s11n::serialize_subnode( dest, "pos", this->pos() );
}
bool QGIPiecePlacemarker::deserialize(  S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	typedef S11nNodeTraits NT;
	QPointF p;
	if( ! s11n::deserialize_subnode( src, "pos", p ) ) return false;
	this->setPos( p );
	S11nNode const * ch = s11n::find_child_by_name(src, "properties");
	return ch
		? QObjectProperties_s11n()( *ch, *this )
		: true;
}
#endif
