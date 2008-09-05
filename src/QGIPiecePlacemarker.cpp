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
#include <QMoveEvent>

#include <cmath>

// #include "GameState.h"
#include <qboard/QGIPiecePlacemarker.h>
#include <qboard/S11nQt.h>
#include "utility.h"

struct QGIPiecePlacemarker::Impl
{
//     GameState * gs;
    bool active;
    Impl() :
// 	gs(0),
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
    this->setProperty("color1",QColor(Qt::yellow));
    this->setProperty("color2",QColor(Qt::red));
    this->setFlags( QGraphicsItem::ItemIsMovable );
    this->setFlag( QGraphicsItem::ItemIsSelectable, false );
#if QT_VERSION >= 0x040400
    this->setCacheMode(DeviceCoordinateCache);
#endif
}

QGIPiecePlacemarker::~QGIPiecePlacemarker()
{
	QBOARD_VERBOSE_DTOR << "~QGIPiecePlacemarker()";
	delete impl;
}
// void QGIPiecePlacemarker::setGameState( GameState & gs )
// {
//     impl->gs = &gs;
// }

void QGIPiecePlacemarker::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    //QGITypes::handleClickRaise( this, ev );
    this->QGraphicsItem::mousePressEvent(ev);
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
QVariant QGIPiecePlacemarker::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
    {
#if 0
	if( impl->gs )
	{
	    impl->gs->setPlacementPos( value.toPoint() );
	}
#endif
    }
    return QGraphicsItem::itemChange(change, value);
}
bool QGIPiecePlacemarker::event( QEvent * e )
{
    if( e->type() == QEvent::DynamicPropertyChange )
    {
	this->update();
    }
    return QObject::event(e);
}
void QGIPiecePlacemarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // FIXME? cache these paints?
    QVariant var = this->property("color1");
    QColor color = (var.isValid() ? var.value<QColor>() : Qt::yellow);
    var = this->property("color2");
    QColor xline = (var.isValid() ? var.value<QColor>() : Qt::red);
    QRectF brect( this->boundingRect() );
    int fudge = 0; // try to avoid lopped off borders
    int w = int(brect.width()-fudge);
    int x = int(brect.x())+(fudge/2);
    int y = int(brect.y())+(fudge/2);
    int step = int(w / 6);
    int w2 = w;
    int h2 = int(brect.height()-fudge);
    bool which = true;
    painter->setPen( QPen( Qt::NoPen ) );
    while( x < 0 )
    {
	QColor c( which ? xline : color );
	which = !which;
	QRadialGradient grad(0,0,x);
	grad.setFocalPoint(-(w2/3),-(h2/3));
	grad.setColorAt( 0, c );
	grad.setColorAt( 1, c.light(130) );
	painter->setBrush( grad );
	painter->drawEllipse( x, y, w2, h2 );
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
