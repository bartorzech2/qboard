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

#include <qboard/QGIDot.h>
#include <qboard/S11nQt.h>
#include <qboard/utility.h>
#include <qboard/MenuHandlerGeneric.h>
#include <qboard/S11nQt/QList.h>
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

#include <cmath> // acos()

#include <qboard/S11nQt/QBrush.h>
#include <qboard/S11nQt/QPen.h>
#include <qboard/S11nQt/QPointF.h>
#include <qboard/S11nQt/QGraphicsItem.h>

struct QGIDot::Impl
{
    bool active;
    qreal radius;
    QColor color;
    QPen pen;
    QBrush brush;
    QGIDot::EdgeList edges;
    QGIDotLine * inLine;
    enum PropIDs {
    PropUnknown = 0,
    PropAlpha,
    PropAngle,
    PropColor,
    PropDragDisabled,
    PropPos,
    PropRadius,
    PropScale,
    PropZLevel,
    PropEND
    };
    Impl() : active(false),
	     radius(12),
	     color(Qt::red),
	     pen(Qt::NoPen),
	     brush(),
	     edges(),
	     inLine(0)
	{
	}
	~Impl()
	{
	}
};

struct QGIDotLine::Impl
{
    QGIDot * src;
    QGIDot * dest;
    QPointF pSrc;
    QPointF pDest;
    qreal arrowSize;
    QBrush brush;
    QPen pen;
    enum PropIDs {
    PropUnknown = 0,
    PropAlpha,
    PropColor,
    PropStyle,
    PropWidth,
    PropEND
    };
    Impl() : src(0),
	     dest(0),
	     pSrc(),
	     pDest(),
	     arrowSize(16),
	     brush(QColor(Qt::red)),
	     pen(brush,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin)
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
	this->setProperty("color",QColor(Qt::red)); // forces brush to get updated
}

QGIDot::~QGIDot()
{
    QBOARD_VERBOSE_DTOR << "~QGIDot()";
#if 1
    EdgeList cp( impl->edges );
    foreach( QGIDotLine * it, cp )
    {
	this->removeEdge( it );
    }
#endif
    emit dotDestructing(this);
    delete impl;
}

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
    //this->setPen(impl->pen);
    this->setRect( QRectF(-r,-r,r*2,r*2) );
}

void QGIDot::refreshTransformation()
{
    QVariant v( this->property("angle") );
    qreal angle = v.canConvert<qreal>() ? v.value<qreal>() : 0.0;
    v = this->property("scale");
    qreal scale = v.canConvert<qreal>() ? v.value<qreal>() : 1.0;
    if( 0 == scale ) scale = 1.0;
    QTransform tr( qboard::rotateAndScale( this->boundingRect(), angle, scale, scale, true ) );
    // TODO:
    // horizontal/vertical flip
    // shearing
    this->setTransform( tr );
}

void QGIDot::propertySet( char const *pname, QVariant const & var )
{
    typedef QMap<QString,int> PMAP;
    static PMAP pmap;
    if( pmap.isEmpty() )
    {
#define MAP(S,V) pmap[S] = Impl::V;
	MAP("color",PropColor);
	MAP("alpha",PropAlpha);
	MAP("colorAlpha",PropAlpha);
	MAP("zLevel",PropZLevel);
	MAP("pos",PropPos);
	MAP("scale",PropScale);
	MAP("angle",PropAngle);
	MAP("radius",PropRadius);
	MAP("dragDisabled",PropDragDisabled);

#undef MAP
    }
    int kid = pmap.value( QString(pname?pname:""), Impl::PropUnknown );
    if( Impl::PropUnknown == kid ) return;

    // FIXME: treat QVariant::Invalid appropriately for each property
    if(0) qDebug() << "QGIDot::propertySet("<<pname<<") val="<<var;
    if( Impl::PropZLevel == kid )
    {
	this->setZValue(var.toDouble());
	this->update();
	return;
    }
    if( Impl::PropPos == kid )
    {
	this->setPos( var.value<QPointF>() );
	this->update();
	return;
    }
    if( Impl::PropColor == kid )
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
    if( Impl::PropAlpha == kid )
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
    if( (Impl::PropScale == kid) || (Impl::PropAngle == kid) )
    {
	this->refreshTransformation();
	return;
    }
    if( Impl::PropRadius == kid )
    {
	impl->radius = var.value<qreal>();
	this->prepareGeometryChange();
	this->updatePainter();
	this->update();
	return;
    }
    if( Impl::PropDragDisabled == kid )
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
		    foreach (QGIDotLine *edge, impl->edges)
			edge->adjust();
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

#include <qboard/QGIHider.h>
QGraphicsItem * QGIDot::hideItems()
{
    QGIHider::hideItems( this );
    return this->parentItem();
}

void QGIDot::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{
    this->QGraphicsEllipseItem::mouseDoubleClickEvent(event);
    emit doubleClicked(this);
}

void QGIDot::contextMenuEvent( QGraphicsSceneContextMenuEvent * ev )
{
    ev->accept();
    impl->active = false;
    typedef QList<QObject *> OL;
    OL selected;
    if(  this->isSelected() )
    {
	selected = qboard::selectedItemsCast<QObject>( this->scene() );
    }
    else
    {
	selected.push_back( static_cast<QObject*>(this) );
    }

    MenuHandlerCommon proxy;
    QMenu * m = proxy.createMenu( this );
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "alpha" );
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
	m->addMenu( QObjectPropertyMenu::makeNumberListMenu("Rotate",
							    selected,
							    "angle",
							    0,360,15) );
    }

    QMenu * mMisc = m->addMenu("Misc.");
    if(1)
    {
	mMisc->addAction(QIcon(":/QBoard/icon/box_wrapped.png"),
			 "Cover",this,SLOT(hideItems()));
	mMisc->addAction("Create line", this, SLOT(split()));
    }

    if( impl->inLine )
    {
	m->addSeparator();
	QMenu * mL = impl->inLine->createLineMenu();
	m->addMenu(mL);
	mL->setTitle("Incoming line");
	QPixmap px(16,16);
	px.fill( QColor(Qt::transparent) );
	QPainter pn(&px);
	pn.setPen( impl->inLine->pen() );
	pn.drawLine( QLineF(0,0,16,16) );
	mL->setIcon( QIcon(px) );
	m->addAction(QIcon(":/QBoard/icon/editcopy.png"),
		     "Clone with line",
		     this, SLOT(cloneWithLine()));
    }


    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( this, m );
    clipper->addDefaultEntries( m, true, this->isSelected() );
#if 1
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
#endif
    m->exec( ev->screenPos() );
    delete m;
    // We REALLY want to set (impl->active=false) here, BUT we might have just
    // been deleted by the menu handler! Setting it before exec() doesn't
    // work because of the event ordering.
}

void QGIDot::cloneWithLine()
{
    if( ! impl->inLine ) return;
    QGIDotLine * line = static_cast<QGIDotLine*>( impl->inLine->clone() );
    QGraphicsItem * lp = impl->inLine->parentItem();
    if( lp && (QGITypes::QGIDot == lp->type()) )
    {
	line->setSourceNode( impl->inLine->impl->src );
    }
    else
    {
	delete line; // otherwise we end up leaking it
    }
    QGIDot * dot = static_cast<QGIDot*>( line->impl->dest );
    QPointF mypos( this->pos() );
    dot->setPos( mypos + QPointF( impl->radius /2, impl->radius /2) );
}

void QGIDot::split()
{
    QGIDot * ch = 0;
#if 0
    // aaarrggg! We can't clone this way b/c that'll get our
    // children as well.
    ch = dynamic_cast<QGIDot*>(this->clone());
    if( ! ch )
    {
	throw std::runtime_error("Serious error: QGIDot::clone() failed! Almost certainly a de/serialization error.");
    }
#else
    ch = new QGIDot;
    *ch->impl = *this->impl;
    ch->impl->edges.clear();
    qboard::copyProperties( this, ch );
    ch->updatePainter();
    QRectF r( this->boundingRect() );
    QPointF p = this->pos() + QPointF(r.width(),r.height());
    ch->setPos( p );
    QGIDotLine * li = new QGIDotLine;
    li->setProperty( "color", impl->color );
    li->setNodes( this, ch );
#endif
}

Serializable * QGIDot::clone() const
{
    Serializable * s = this->Serializable::clone();
//     if( ! s ) return 0;
//     QGIDot * cl = static_cast<QGIDot*>(s);
//     if( ! cl )
//     {
// 	delete s;
// 	s = 0;
//     }
//     while(s)
//     {
// 	QGraphicsItem * parent = this->parentItem();
// 	if( ! parent ) break;
// 	if( parent->type() != QGIType::QGIDotLine ) break;
// 	QGIDotLine
//     }
    return s;
}

bool QGIDot::serialize( S11nNode & dest ) const
{
    // FIXME: serialize the full QPen info.
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    if( this->metaObject()->propertyCount() )
    {
	QObject props;
	qboard::copyProperties( this, &props );
	// i can't get the QColor alpha property to serialize (it's as if... well, no...)
	// so we save the alpha properties separately.
	props.setProperty("alpha", impl->color.alpha());
	props.setProperty("pos", this->pos() );
	props.setProperty("zLevel", this->zValue() );
	S11nNode & pr( s11n::create_child( dest, "properties" ) );
	QObject const & constnessKludge( props );
	if( ! s11n::qt::QObjectProperties_s11n()( pr, constnessKludge ) ) return false;
    }
    QList<QGraphicsItem *> chgi( qboard::childItems(this) );
    if( ! chgi.isEmpty() )
    {
	if( -1 == s11n::qt::serializeQGIList<Serializable>( s11n::create_child(dest,"children"),
							    chgi, false ) )
	{
	    qDebug() << "QGIDot::serialize: serializeQGIList() failed!";
	    return false;
	}
    }
    return true;
}

bool QGIDot::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    delete impl;
    impl = new Impl;
    S11nNode const * ch = s11n::find_child_by_name( src, "pos" );
    if( ch )
    {
	// QBoard < 20080828 used this extra property
	QPointF p;
	if( ! s11n::deserialize( *ch, p ) ) return false;
	this->setPos( p );
    }
#if 1
    qboard::destroyQGIList( qboard::childItems(this) );
    ch = s11n::find_child_by_name(src, "children");
    if( ch )
    {
	typedef QList<QGraphicsItem *> QGIL;
	QGIL childs;
	if( -1 == s11n::qt::deserializeQGIList<Serializable>( *ch,
							      childs ) )
	{
	    return false;
	}
	foreach( QGraphicsItem * it, childs )
	{
	    if( it->type() != QGITypes::QGIDotLine )
	    {
		it->setParentItem(this);
		continue;
	    }
	    QGIDotLine * d = dynamic_cast<QGIDotLine*>(it);
	    if( ! d )
	    {
		it->setParentItem(this);
		continue;
	    }
	    d->setSourceNode( this );
	}
    }
#endif
    ch = s11n::find_child_by_name(src, "properties");
    return ch
	? s11n::qt::QObjectProperties_s11n()( *ch, *this )
	: true;
}

QGIDot::EdgeList QGIDot::edges() const
{
    return impl->edges;
}

void QGIDot::addEdge( QGIDotLine * l )
{
    if( ! l ) return;
    connect( l, SIGNAL(lineDestructing(QGIDotLine*)),
	     this,SLOT(removeEdge(QGIDotLine*)));
    impl->edges << l;
    //l->adjust();
}


void QGIDot::removeEdge( QGIDotLine * li )
{
    impl->edges.removeAll(li);
    if( impl->inLine == li ) impl->inLine = 0;
    disconnect( li, SIGNAL(lineDestructing(QGIDotLine*)),
		this,SLOT(removeEdge(QGIDotLine*)));
}

void MenuHandlerDot::showHelp()
{
    qboard::showHelpResource("Dots", ":/QBoard/help/classes/QGIDot.html");
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

void QGIDot::showHelp()
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
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "alpha" );
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

    QMenu * mMisc = m->addMenu("Misc.");
    if(1)
    {
	mMisc->addAction(QIcon(":/QBoard/icon/box_wrapped.png"),
			 "Cover",gvi,SLOT(hideItems()));
	mMisc->addAction("Create line", gvi, SLOT(split()));
    }

    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( gvi, m );
    clipper->addDefaultEntries( m, true, gvi->isSelected() );
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
    m->exec( ev->screenPos() );
    delete m;
}



QGIDotLine::QGIDotLine() : QObject(),
			   QGraphicsLineItem(),
			   Serializable("QGIDotLine"),
			   impl(new Impl)
{
    this->setPen( impl->pen );
    //this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
}

QGIDotLine::~QGIDotLine()
{
    emit lineDestructing(this);
    //qDebug() << "QGIDotLine::~QGIDotLine()";
    delete impl;
}

void QGIDotLine::destDestroyed( QGIDot * )
{
    impl->dest = 0;
    this->deleteLater();
}


void QGIDotLine::adjust()
{
    if (!impl->src || !impl->dest)
        return;

    QLineF line(mapFromItem(impl->src, 0, 0), mapFromItem(impl->dest, 0, 0));
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
    impl->arrowSize = impl->dest->boundingRect().width() / 2;
    this->prepareGeometryChange();
    impl->pSrc = line.p1() + edgeOffset;
    impl->pDest = line.p2() - edgeOffset;
    this->setLine( QLineF( impl->pSrc, impl->pDest ) );
}

QRectF QGIDotLine::boundingRect() const
{
    if (!impl->src || !impl->dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + impl->arrowSize) / 2.0;

    QSizeF d(impl->pDest.x() - impl->pSrc.x(),
	     impl->pDest.y() - impl->pSrc.y());
    return QRectF(impl->pSrc, d )
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void QGIDotLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *wid)
{
    if (!impl->src || !impl->dest)
    {
 	//this->QGraphicsLineItem::paint( painter, opt, wid );
	return;
    }
    static const double Pi = 3.14159265358979323846264338327950288419717;
    this->QGraphicsLineItem::paint(painter,opt,wid);
    QLineF line( this->line() );
    // Draw the arrows if there's enough room
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
    {
        angle = Pi * 2 - angle;
    }
    
    QPointF start( line.p2() );
    QRectF db( impl->dest->boundingRect() );
    const qreal asz = impl->arrowSize;
    QPointF ap1 = start
	+ QPointF(::sin(angle - Pi / 3) * asz,
		  ::cos(angle - Pi / 3) * asz);
    QPointF ap2 = start
	+ QPointF(::sin(angle - Pi + Pi / 3) * asz,
		  ::cos(angle - Pi + Pi / 3) * asz);
    painter->setBrush(impl->pen.brush());
    painter->setPen(impl->pen);
    painter->drawPolygon(QPolygonF() << start << ap1 << ap2);
    
}


void QGIDotLine::setSourceNode( QGIDot * d )
{
    if( d == impl->src ) return;
    if( impl->src )
    {
	impl->src->removeEdge( this );
    }
    impl->src = d;
    if( ! d ) return;
    this->setPos( QPointF(0,0) );
    this->setParentItem( d );
    d->addEdge( this );
    this->adjust();
}
void QGIDotLine::setDestNode( QGIDot * d )
{
    if( d == impl->dest ) return;
    if( impl->dest )
    {
	impl->dest->impl->inLine = 0;
	impl->dest->removeEdge( this );
	disconnect( impl->dest, SIGNAL(dotDestructing(QGIDot*)),
		    this, SLOT(destDestroyed(QGIDot*)));
    }
    impl->dest = d;
    d->impl->inLine = this;
    if( ! d ) return;
    d->setParentItem(this);
    if( impl->src )
    {
	d->setPos( d->pos() - impl->src->pos() );
    }
    connect( d, SIGNAL(dotDestructing(QGIDot*)),
	     this, SLOT(destDestroyed(QGIDot*)));
    d->addEdge(this);
    this->adjust();
}

void QGIDotLine::setNodes( QGIDot * src, QGIDot * dest )
{
    this->setSourceNode( src );
    this->setDestNode( dest );
}

QGIDot * QGIDotLine::srcNode()
{
    return impl->src;
}
QGIDot * QGIDotLine::destNode()
{
    return impl->dest;
}

void QGIDotLine::propertySet( char const *pname,
			      QVariant const & var )
{
    if( ! pname ) return;
    typedef QMap<QString,int> PMAP;
    static PMAP pmap;
    if( pmap.isEmpty() )
    {
#define MAP(S,V) pmap[S] = Impl::V;
	MAP("alpha",PropAlpha);
	MAP("colorAlpha",PropAlpha);
	MAP("color",PropColor);
	MAP("style",PropStyle);
	MAP("width",PropWidth);
#undef MAP
    }
    int kid = pmap.value( QString(pname?pname:""), Impl::PropUnknown );
    if( Impl::PropUnknown == kid ) return;

    if(0) qDebug() << "QGIDotLine::propertySet("<<pname<<") val="<<var;
    QString key( pname );
    if( Impl::PropWidth == kid )
    {
	impl->pen.setWidth( var.toInt() );
    }
    else if( Impl::PropColor == kid )
    {
	QColor newcol( var.value<QColor>() );
	newcol.setAlphaF( impl->pen.color().alphaF() );
	impl->pen.setColor( newcol );
    }
    else if( Impl::PropAlpha == kid )
    {
	qreal a = var.toDouble();
	QColor col = impl->pen.color();
	if( a > 1.0 )
	{ // assume it's int-encoded
	    col.setAlpha( int(a) );
	}
	else
	{
	    col.setAlphaF( a );
	}
	impl->pen.setColor(col);
    }
    else if( Impl::PropStyle == kid )
    {
	impl->pen.setStyle( Qt::PenStyle( s11n::qt::stringToPenStyle(var.toString()) ) );
    }
    this->setPen(impl->pen);
    this->update();
}

bool QGIDotLine::event( QEvent * e )
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

bool QGIDotLine::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    if( ! impl->dest )
    {
	throw s11n::s11n_exception("QGIDotLine cannot serialize when no destination Dot is set!");
    }
    typedef S11nNodeTraits NT;
    if(0) qDebug() <<"QGIDotLine::serialize() this =="<<(void const *)this
	     << "impl->src =="<<(void const *)impl->src
	     << "impl->dest =="<<(void const *)impl->dest
	;
    if( ! s11nlite::serialize_subnode( dest, "pen", impl->pen ) )
    {
	qDebug() << "QGIDotLine::serialize(): pen serialization failed.";
	return false;
    }
    return s11nlite::serialize_subnode( dest, "dest", *impl->dest ); 
}

bool QGIDotLine::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    qboard::destroyQGIList( qboard::childItems(this) );
    //typedef S11nNodeTraits NT;
    s11nlite::deserialize_subnode( src, "pen", impl->pen );
    this->setPen( impl->pen );
    S11nNode const * ch = 0;
    ch = s11n::find_child_by_name(src, "dest");
    if( ch )
    {
	QGIDot * d = s11nlite::deserialize<QGIDot>( *ch );
	if( ! d ) return false;
	this->setDestNode( d );
    }
    return true;
}

QMenu * QGIDotLine::createLineMenu()
{
    if( ! impl->dest ) return 0;
    QMenu * m = new QMenu("Line");

    typedef QList<QGIDot*> QGIL;
    QGIL selected;
    if( impl->dest->isSelected() && impl->dest->scene() )
    {
	selected = qboard::graphicsItemsCast<QGIDot>( impl->dest->scene()->selectedItems() );
    }
    else
    {
	selected.push_back(impl->dest);
    }
    typedef QList<QObject*> QOL;
    QOL selobj;
    for( QGIL::iterator it = selected.begin();
	 selected.end() != it; ++it )
    {
	QGIDotLine * line = (*it)->impl->inLine;
	if( line ) selobj.push_back( line );
    }

    m->addMenu( QObjectPropertyMenu::makeColorMenu(selobj,
						   "color",
						   "alpha") );
    m->addMenu( QObjectPropertyMenu::makePenStyleMenu(selobj, "style") );
    m->addMenu( 
	       QObjectPropertyMenu::makeNumberListMenu("Width",
						       selobj, "width",
						       0, 11, 1 )
	       );
    return m;
}
