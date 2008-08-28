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

#include "QGIDot.h"
#include "S11nQt.h"
#include "utility.h"
#include "MenuHandlerGeneric.h"
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

struct QGIDot::Impl
{
    bool active;
    qreal radius;
    QColor color;
    QPen pen;
    QBrush brush;
    Impl() : active(false),
	     radius(8),
	     color(Qt::yellow),
	     pen(Qt::NoPen),
	     brush()
	{
		
	}
	~Impl()
	{
		
	}
};

QGIDot::QGIDot() :
    QObject(),
    QGraphicsEllipseItem(),
    Serializable("QGIDot"),
    impl(new Impl)
{
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
#if QT_VERSION >= 0x040400
	this->setCacheMode(DeviceCoordinateCache);
#endif
	this->setProperty("color",QColor(Qt::yellow)); // forces brush to get updated
}

QGIDot::~QGIDot()
{
    QBOARD_VERBOSE_DTOR << "~QGIDot()";
    delete impl;
}

#include "utility.h"
void QGIDot::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	impl->active = true;
	this->updatePainter();
	QGITypes::handleClickRaise(this, ev);
	this->QGraphicsEllipseItem::mousePressEvent(ev);
}

void QGIDot::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	impl->active = false;
	this->updatePainter();
	this->QGraphicsEllipseItem::mouseReleaseEvent(event);
	event->accept();
	//foreach (QGILineBinder *edge, m_ed) edge->adjust();
	//this->m_line->adjust();
}

QRectF QGIDot::boundingRect() const
{
#if 1
    return this->QGraphicsEllipseItem::boundingRect();
#else
	const qreal adjust = 2;
	const qreal r = impl->radius;
	return QRectF(-r - adjust, -r - adjust,2*r + adjust, 2*r + adjust);
#endif
}

QPainterPath QGIDot::shape() const
{
	QPainterPath path;
// 	const qreal r = impl->radius;
// 	path.addEllipse(-r, -r, 2*r, 2*r );
	path.addEllipse( this->boundingRect() );
	return path;
}

void QGIDot::updatePainter()
{
    /**
       Update our appearance flags so the next paint() call will get
       them.
    */
    const qreal r( impl->radius );
    QColor color( impl->color );
    QColor darker( color.darker() );
    const int x = 0;
    QRadialGradient gradient(-x, -x, r);
    if( impl->active )
    {
	gradient.setCenter(0,0);
	gradient.setFocalPoint(x,x);
	QColor tmp(color);
	color = darker.light(150);
	darker = tmp.light(150);
    }
    gradient.setColorAt(0, color);
    gradient.setColorAt(1, darker);
    impl->brush = QBrush(gradient);
    this->setBrush(impl->brush);
    this->setPen(impl->pen);
    this->setRect( QRectF(-r,-r,r*2,r*2) );
}

void QGIDot::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property
    if(0) qDebug() << "QGIDot::propertySet("<<pname<<") val="<<var;
    std::string key( pname );
    if( "zLevel" == key )
    {
	this->setZValue(var.toDouble());
	this->update();
	return;
    }
    if( "pos" == key )
    {
	this->setPos( var.value<QPointF>() );
	this->update();
	return;
    }
    if( "color" == key )
    {
	QColor old( impl->color );
	impl->color = var.value<QColor>();
	if( 255 == impl->color.alpha() )
	{
	    impl->color.setAlpha(old.alpha());
	}
	this->updatePainter();
	this->update();
	return;
    }
    if( "colorAlpha" == key )
    {
	qreal a = var.toDouble();
	if( a > 1 )
	{ // assume it's int-encoded
	    impl->color.setAlpha( int(a) );
	}
	else
	{
	    impl->color.setAlphaF(a);
	}
	this->updatePainter();
    	this->update();
	return;
    }
    if( "scale" == key )
    {
	qreal scale = var.canConvert<qreal>() ? var.value<qreal>() : 1.0;
	if( 0 == scale ) scale = 1.0;
	qboard::rotateAndScale( this, 0, scale, scale, true);
	return;
    }
    if( "radius" == key )
    {
	impl->radius = var.value<qreal>();
	this->prepareGeometryChange();
	this->updatePainter();
	this->update();
	return;
    }
    if( "dragDisabled" == key )
    {
	if( var.isValid() )
	{
	    this->setFlag( QGraphicsItem::ItemIsMovable, var.toInt() ? false : true );
	}
	else
	{
	    this->setFlag( QGraphicsItem::ItemIsMovable, true );
	}
	return;
    }
}


bool QGIDot::event( QEvent * e )
{
    while( e->type() == QEvent::DynamicPropertyChange )
    {
	QDynamicPropertyChangeEvent *chev = dynamic_cast<QDynamicPropertyChangeEvent *>(e);
	if( ! chev ) break;
	char const * key = chev->propertyName().constData();
	this->propertySet( key, this->property(key) );
	e->accept();
	break;
    }
    return QObject::event(e);
}
QVariant QGIDot::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
		case ItemPositionHasChanged:
			//foreach (QGILineBinder *edge, m_ed)	edge->adjust();
// 			if( m_line )
// 			{
// 				m_line->adjust();
// 				this->update();
// 			}
		break;
	default:
		break;
	};
	return QGraphicsEllipseItem::itemChange(change, value);
}
void QGIDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *wid)
{
    this->QGraphicsEllipseItem::paint(painter,opt,wid); // draws our selection box
}

void QGIDot::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	event->accept();
	MenuHandlerDot mh;
	impl->active = false;
	mh.doMenu( this, event );
#if 0 // don't do anything here - we might have been deleted!
	this->edge()->update();
	this->update();
#endif
}


bool QGIDot::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	if( this->metaObject()->propertyCount() )
	{
	    QObject props;
	    qboard::copyProperties( this, &props );
	    // i can't get the QColor alpha property to serialize (it's as if... well, no...)
	    // so we save the alpha properties separately.
	    props.setProperty("colorAlpha", impl->color.alpha());
	    props.setProperty("pos", this->pos() );
	    props.setProperty("zLevel", this->zValue() );
	    S11nNode & pr( s11n::create_child( dest, "properties" ) );
	    QObject const & constnessKludge( props );
	    if( ! s11n::qt::QObjectProperties_s11n()( pr, constnessKludge ) ) return false;
	}
	return true;
}

bool QGIDot::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    S11nNode const * ch = s11n::find_child_by_name( src, "pos" );
    if( ch )
    {
	// QBoard < 20080828 used this extra property
	QPointF p;
	if( ! s11n::deserialize( *ch, p ) ) return false;
	this->setPos( p );
    }
    ch = s11n::find_child_by_name(src, "properties");
    return ch
	? s11n::qt::QObjectProperties_s11n()( *ch, *this )
	: true;
}

struct MenuHandlerDot::Impl
{
    QGIDot * dot;
    Impl() : dot(0)
    {}
    ~Impl()
    {
    }
};
MenuHandlerDot::MenuHandlerDot() : QObject(), impl(new Impl)
{
	
}
MenuHandlerDot::~MenuHandlerDot()
{
	delete impl;
}

void MenuHandlerDot::showHelp()
{
    qboard::showHelpResource("Dots", ":/QBoard/help/classes/QGIDot.html");
}

void MenuHandlerDot::doMenu( QGIDot *gvi, QGraphicsSceneContextMenuEvent * ev )
{
    if( ! gvi ) return;
    impl->dot = gvi;
    ev->accept();

    typedef QList<QObject *> OL;
    OL selected;
    if(  gvi->isSelected() )
    {
	selected = qboard::selectedItemsCast<QObject>( gvi->scene() );
    }
    else
    {
	selected.push_back( dynamic_cast<QObject*>(gvi) );
    }

    QString label("Dot...");
    MenuHandlerCommon proxy;
    QMenu * m = proxy.createMenu( gvi );
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "colorAlpha" );
    mColor->setIcon(QIcon(":/QBoard/icon/color_fill.png"));
    m->addMenu(mColor);

    if(1)
    {
	m->addMenu( QObjectPropertyMenu::makeNumberListMenu(
				    "Radius",
				    selected,
				    "radius",
				    8,33,4) );
    }
    if(1)
    {
	QObjectPropertyMenu * pm =
	    QObjectPropertyMenu::makeNumberListMenu("Scale",
						    selected, "scale",
						    0.5, 3.01, 0.5);
	pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	m->addMenu(pm);
    }

    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( gvi, m );
    clipper->addDefaultEntries( m, true, gvi->isSelected() );
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
    m->exec( ev->screenPos() );
    delete m;
}
