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

#include "QGILine.h"
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
#include <cmath>

// If QGILineBinder_do_arrow is true then code is added for QGILineBinder to draw
// arrows at its endpoints.
#define QGILineBinder_do_arrow 0
struct QGILineBinder::Impl
{
	static const int defaultArrowSize = 12;
	QPair<QGILineNode *,QGILineNode *> ends;
	typedef QPair<QPointF,QPointF> PointPair;
	PointPair pts;
	bool blockUpdates;
	bool destructing;
	Impl() :
		ends(0,0),
		pts(),
		blockUpdates(false),
		destructing(false)
	{
		
	}
	~Impl()
	{
		
	}
};

void QGILineBinder::setup()
{
	this->setProperty("color",QColor(Qt::black));
	this->setProperty("alpha",255);
	this->setProperty("style",Qt::SolidLine);
	this->setProperty("width",2);
	this->setProperty("arrowSize",Impl::defaultArrowSize);
	this->setProperty("drawArrows",0);
#if QT_VERSION >= 0x040400
	this->setCacheMode(DeviceCoordinateCache);
#endif
	this->setFlags( QGraphicsItem::ItemIsSelectable );
	//this->setFlags( QGraphicsItem::ItemIsMovable ); // | QGraphicsItem::ItemIsSelectable );
	/** ^^^ IsMovable has weird interactions with the children once one of the children
	has been dragged independently of this object. There's certainly
	a fix for that somewhere...
	
	QGILineBinder::shape() can be re-implemented (by someone smarter than i) to allow this object
	to be reasonably selectable with the mouse. The current impl just sets the bounds to
	a single-pixel line, effectively making it unselectable.
	*/
}
QGILineBinder::QGILineBinder() :

	QObject(),
	QGraphicsItem(),
	Serializable("QGILineBinder"),
	impl(new Impl)
{
	this->setup();
}
QGILineBinder::QGILineBinder( QGILineNode * l, QGILineNode * r ) :
	QObject(),
	QGraphicsItem(),
	Serializable("QGILineBinder"),
	impl(new Impl)
{
	this->first(l);
	this->second(r);
	this->setup();
}
QGILineBinder::~QGILineBinder()
{
	delete impl;
	if(0) qDebug() << "~QGILineBinder()";
}

void QGILineBinder::toggleArrows()
{
	QVariant var( this->property("drawArrows") );
	int i = var.isValid() ? var.toInt() : 1;
	this->setProperty("drawArrows", i ? 0 : 1 );
	this->update();
}
void QGILineBinder::mousePressEvent(QGraphicsSceneMouseEvent * ev)
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
	impl->blockUpdates = true;
}
void QGILineBinder::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

	this->QGraphicsItem::mouseReleaseEvent(event);
	event->accept();
	impl->blockUpdates = false;
	this->adjust();
}
bool QGILineBinder::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	if( ! impl->ends.first || ! impl->ends.second ) return false;
	// We must ensure that the "pos" property is synced, but we have
	// to violate constness to do it :/
	QGILineBinder * ncthis = const_cast<QGILineBinder*>( this );
	ncthis->setProperty( "pos", this->pos() );
	if( this->metaObject()->propertyCount() )
	{
		S11nNode & pr( s11n::create_child( dest, "properties" ) );
		if( ! QObjectProperties_s11n()( pr, *this ) ) return false;
	}
	QObjectProperties_s11n proxy;
	QObject const * qobj = impl->ends.first; // kludge to avoid an ambiguity
	if( ! proxy(s11n::create_child( dest, "first" ), *qobj) ) return false;
	qobj = impl->ends.second;
	if( ! proxy(s11n::create_child( dest, "second" ), *qobj) ) return false;
	/**
	To avoid offset calculations, we save the absolute LineNode
	positions instead of our m_pts member. This still has the problem
	that if the hard-coded size of the LineNodes changes between
	serializations, we'll place the line slightly differently
	than before when we load.
	*/
	Impl::PointPair pts( impl->ends.first->pos(), impl->ends.second->pos() );
	return s11n::serialize_subnode( dest, "pos", pts );
}

bool QGILineBinder::deserialize( S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	typedef S11nNodeTraits NT;
	if( impl->ends.first ) delete impl->ends.first;
	if( impl->ends.second ) delete impl->ends.second;
	Impl::PointPair pts;
	if( ! s11n::deserialize_subnode( src, "pos", pts ) )
	{
		qDebug() << "QGILineBinder::deserialize() deser of pos failed.";
		return false;
	}
	QGILineNode * n1 = new QGILineNode;
	QGILineNode * n2 = new QGILineNode;
	n1->setPos( pts.first );
	n2->setPos( pts.second );
	this->first(n1);
	this->second(n2);
	S11nNode const * ch = s11n::find_child_by_name(src, "properties");
	if( ch ) QObjectProperties_s11n()( *ch, *this );
	ch = s11n::find_child_by_name(src, "first");
	if( ch ) QObjectProperties_s11n()( *ch, *n1 );
	ch = s11n::find_child_by_name(src, "second");
	if( ch ) QObjectProperties_s11n()( *ch, *n2 );
	return true;
}
QGILineNode * QGILineBinder::first()
{
	return impl->ends.first;
}
QGILineNode * QGILineBinder::second()
{
	return impl->ends.second;
}
void QGILineBinder::first( QGILineNode * l )
{
	impl->ends.first = l;
	l->setLine(this);
}
void QGILineBinder::second( QGILineNode * r )
{
	impl->ends.second = r;
	r->setLine(this);
}

bool QGILineBinder::isValid()
{
	if( impl->ends.first && impl->ends.second ) return true;
	this->deleteLater();
	return false;
}

void QGILineBinder::adjust()
{
	if( impl->blockUpdates
		|| ! impl->ends.first
		|| !impl->ends.second )
	{
		 return;
	}
	QLineF line(mapFromItem(impl->ends.first, 0, 0), mapFromItem(impl->ends.second, 0, 0));
	qreal length = line.length();
	QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

	prepareGeometryChange();
	impl->pts.first = line.p1() + edgeOffset;
	impl->pts.second = line.p2() - edgeOffset;
}

QPainterPath QGILineBinder::shape() const
{
	// Make sure the bounding rectangle can't keep us from clicking
	// on otherwise visible objects...
	QVector<QPointF> vec;
	vec.push_back( impl->pts.first );
	vec.push_back( impl->pts.second );
	QPainterPath path;
	path.addPolygon( QPolygonF(vec) );
	// someone with a bigger brain could make that a line of, say, 4 or 6 pixels wide.
	return path;
}

bool QGILineBinder::event( QEvent * e )
{
	if( QEvent::DynamicPropertyChange == e->type() )
	{
		e->accept();
		this->update();
		return true;
	}
	return QObject::event(e);
}

void QGILineBinder::destroyLine()
{
	qDebug() <<"QGILineBinder::destroyLine()";
	impl->destructing = true;
	QGILineNode * n = impl->ends.first;
	if( n )
	{
		delete n;
		impl->ends.first = 0;
	}
	n = impl->ends.second;
	if( n )
	{
		delete n;
		impl->ends.second = 0;
	}
	this->deleteLater();
}

QRectF QGILineBinder::boundingRect() const
{
	if(!impl->ends.first || !impl->ends.second) return QRectF();
	QVariant var;
	qreal arrowSize(0.0);
	if( this->property("drawArrows").toInt() )
	{
		var = this->property("arrowSize");
		arrowSize = (var.isValid() ? var.toDouble() : Impl::defaultArrowSize);
	}
	var = this->property("width");
	qreal penWidth = (var.isValid() ? var.toDouble() : 1.0);
	qreal extra = (penWidth + arrowSize) / 2.0;

	return QRectF(impl->pts.first, QSizeF(impl->pts.second.x() - impl->pts.first.x(),
			impl->pts.second.y() - impl->pts.first.y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}
void QGILineBinder::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if( ! this->isValid() ) return;// schedule our own destruction here.
	QVariant var;
	static const double Pi = 3.14159265358979323846264338327950288419717;
	static double TwoPi = 2.0 * Pi;
	var = this->property("width");
	qreal lineWidth = (var.isValid() ? var.toDouble() : 2.0);
	var = this->property("color");
	QColor lineColor = (var.isValid() ? var.value<QColor>() : Qt::black);
	var = this->property("style");
	int lineStyle = (var.isValid() ? var.toInt() : Qt::SolidLine);
	if( Qt::NoPen == lineStyle ) lineStyle = Qt::SolidLine;


	Qt::PenCapStyle capS(Qt::RoundCap);
	Qt::PenJoinStyle joinS(Qt::RoundJoin);

	// Draw the line itself
	QLineF line(impl->pts.first, impl->pts.second);
	var = this->property("alpha");
	if( var.isValid() ) lineColor.setAlpha( var.toInt() ); 
	painter->save();
	painter->setPen(QPen(lineColor, lineWidth, Qt::PenStyle(lineStyle), capS, joinS));
	painter->drawLine(line);
	painter->restore();

	if( this->property("drawArrows").toInt() )
	{
		var = this->property("arrowSize");
		qreal arrowSize = var.isValid() ? var.toDouble() : Impl::defaultArrowSize;
		// Draw the arrows if there's enough room
		double angle = std::acos(line.dx() / line.length());
		if (line.dy() >= 0)
		{
			angle = TwoPi - angle;
		}		
		QPointF sourceArrowP1 = impl->pts.first + QPointF(std::sin(angle + Pi / 3) * arrowSize,
			std::cos(angle + Pi / 3) * arrowSize);
		QPointF sourceArrowP2 = impl->pts.first + QPointF(std::sin(angle + Pi - Pi / 3) * arrowSize,
			std::cos(angle + Pi - Pi / 3) * arrowSize);   
		QPointF destArrowP1 = impl->pts.second + QPointF(std::sin(angle - Pi / 3) * arrowSize,
			std::cos(angle - Pi / 3) * arrowSize);
		QPointF destArrowP2 = impl->pts.second + QPointF(std::sin(angle - Pi + Pi / 3) * arrowSize,
			std::cos(angle - Pi + Pi / 3) * arrowSize);
		painter->setPen(QPen(lineColor, 2, Qt::SolidLine, capS, joinS ));
		painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2, Qt::WindingFill);
		painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2, Qt::WindingFill);
	}
	if( this->isSelected() )
	{ // doesn't seem to do what i want?
		painter->setPen( QPen(lineColor.light(), 1, Qt::DotLine, capS, joinS));
		painter->drawLine( line );
	}
}

QGILineNode::QGILineNode() :
	QGraphicsItem(),
#if QGILineNode_IS_SERIALIZABLE
	Serializable("QGILineNode"),
#endif
	m_active(false),
	m_line(0)
{
	this->setProperty("color",QColor(Qt::yellow));
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
	// QGraphicsItem::ItemIsSelectable is the root of timing problems in the deletion of the
	// related widgets.
#if QT_VERSION >= 0x040400
	this->setCacheMode(DeviceCoordinateCache);
#endif
}

QGILineNode::~QGILineNode()
{
    if(0) qDebug() << "~QGILineNode()";
}

QGILineBinder * QGILineNode::edge()
{
	return m_line;
}
void QGILineNode::setLine( QGILineBinder * ed )
{
	if( ! ed ) return;
	this->setParentItem( (this->m_line = ed) );
	ed->adjust();
}

#include "utility.h"
void QGILineNode::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	this->m_active = true;
	if( (ev->buttons() & Qt::LeftButton) )
	{
		QGraphicsItem * p = this->parentItem();
		if( p )
		{
			ev->accept();
			qreal zV = qboard::nextZLevel(p);
			if( zV > this->zValue() )
			{
				p->setZValue(zV);
			}
		}
	}
	this->QGraphicsItem::mousePressEvent(ev);
	this->update();
}

void QGILineNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	this->m_active = false;
	this->QGraphicsItem::mouseReleaseEvent(event);
	if( ! m_line ) return;
	event->accept();
	//foreach (QGILineBinder *edge, m_ed) edge->adjust();
	this->m_line->adjust();
	this->update();
}

QRectF QGILineNode::boundingRect() const
{
	qreal adjust = 2;
	return QRectF(-10 - adjust, -10 - adjust,23 + adjust, 23 + adjust);
}

QPainterPath QGILineNode::shape() const
{
	QPainterPath path;
	path.addEllipse(-10, -10, 20, 20);
	return path;
}
bool QGILineNode::event( QEvent * e )
{
	if( e->type() == QEvent::DynamicPropertyChange )
	{
		e->accept();
		this->update();
		return true;
	}
	return QObject::event(e);
}
QVariant QGILineNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
		case ItemPositionHasChanged:
			//foreach (QGILineBinder *edge, m_ed)	edge->adjust();
			if( m_line )
			{
				m_line->adjust();
				this->update();
			}
		break;
	default:
		break;
	};
	return QGraphicsItem::itemChange(change, value);
}
void QGILineNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawEllipse(-7, -7, 20, 20);
	QRadialGradient gradient(-3, -3, 10);
	QVariant var = this->property("color");
	QColor color = (var.isValid() ? var.value<QColor>() : Qt::yellow);
	QColor darker = color.darker();
	//if (option->state & QStyle::State_Sunken) {
	if( this->m_active ){
		gradient.setCenter(3, 3);
		gradient.setFocalPoint(3, 3);
		gradient.setColorAt(1, color.light(150));
		gradient.setColorAt(0, darker.light(150));
	} else {
		gradient.setColorAt(0, color);
		gradient.setColorAt(1, darker);
	}
	painter->setBrush(gradient);
	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(-10, -10, 20, 20);
	// FIXME: render some notification when selected.
}

void QGILineNode::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	event->accept();
	MenuHandlerLineNode mh;
	this->m_active = false;
	mh.doMenu( this, event );
#if 0 // don't do anything here - we might have been deleted!
	this->edge()->update();
	this->update();
#endif
}


#if QGILineNode_IS_SERIALIZABLE
bool QGILineNode::serialize( S11nNode & dest ) const
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
bool QGILineNode::deserialize(  S11nNode const & src )
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


struct MenuHandlerLineNode::Impl
{
	QGILineBinder * line;
	QGILineNode * node;
	bool delLine;
	Impl() : line(0),node(0),delLine(false) {}
	~Impl()
	{
		if( delLine )
		{
			line->destroyLine();
		}
	}
};
MenuHandlerLineNode::MenuHandlerLineNode() : QObject(), impl(new Impl)
{
	
}
MenuHandlerLineNode::~MenuHandlerLineNode()
{
	delete impl;
}

void MenuHandlerLineNode::scheduleLineDestruction()
{
	this->impl->line->setParent(this);
	//this->impl->delLine = true;
}

void MenuHandlerLineNode::showHelp()
{
    qboard::showHelpResource("Lines", ":/QBoard/help/classes/QGILine.html");
}

void MenuHandlerLineNode::doMenu( QGILineNode *gvi, QGraphicsSceneContextMenuEvent * ev )
{
	ev->accept();
	QString label("Line...");
	QMenu * menu = new QMenu(label);
	menu->addAction(label)->setEnabled(false);
	menu->addAction(QIcon(":/QBoard/icon/button_cancel.png"), QString("Destroy Line"),
		this,SLOT(scheduleLineDestruction()) );
	menu->addSeparator();
	//QObject::connect( this, SIGNAL(aboutToShow()), this, SLOT(updateView()) );
	QGILineBinder * line = dynamic_cast<QGILineBinder*>( gvi->parentItem() );
	if( ! line )
	{ // fixme: report an error here.
		delete menu;
		return;
	}
	impl->line = line;
	impl->node = gvi;

	QMenu * menuLine = menu->addMenu("Line");
	menuLine->addMenu( QObjectPropertyMenu::makeColorMenu(line, "color", "alpha" ) );
	menuLine->addMenu( QObjectPropertyMenu::makePenStyleMenu(line, "style" ) );
	menuLine->addMenu( QObjectPropertyMenu::makeNumberListMenu("Width", line, "width",1,10,1) );

	QMenu * mdot = QObjectPropertyMenu::makeColorMenu( gvi,  "color" );
	mdot->setTitle("Dot");
	menu->addMenu( mdot );

	QMenu * menuArrows = menu->addMenu("Arrows");
	QAction * act = menuArrows->addAction("Toggle arrows",line, SLOT(toggleArrows()) );
	QVariant var( line->property("drawArrows") );
	int doArrow = var.isValid() ? var.toInt() : 0;
	if( doArrow )
	{
		act->setCheckable(true);
		act->setChecked(true);
	}
	menuArrows->addMenu( QObjectPropertyMenu::makeNumberListMenu("Size", line, "arrowSize",8,24,2) );

	menu->addSeparator();
	menu->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );

	menu->exec( ev->screenPos() );
	delete menu;
	// Reminder: do not act on gvi or line after menu->exec(), as the exec might have
	// destroyed them! This means we can't do obj->update() to update their appearance
	// from here. We use QObject::event() in gvi/line to catch the updates instead.
}
