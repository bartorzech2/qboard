#include <qboard/QGIHider.h>

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>
#include <QEvent>
#include <QWidget>
#include <QMenu>
#include <QGraphicsPixmapItem>

#include <qboard/S11nQt.h>
#include <qboard/utility.h>
#include <qboard/MenuHandlerGeneric.h>
#include <qboard/S11nQt/QPointF.h>
#include <qboard/S11nQt/QBrush.h>
#include <qboard/S11nQt/QTransform.h>

struct QGIHider::Impl
{
    QGraphicsItem * item;
    QPen pen;
    //QBrush brush;
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
    Impl()
	: item(0),
	  pen()
    {
    }
    ~Impl()
    {
	// reminder: we don't own this->item
    }
};

QGIHider::QGIHider() : QObject(),
		       QGraphicsPathItem(),
		       Serializable("QGIHider"),
		       impl(new Impl)
{
    this->setup();
}

void QGIHider::setup()
{
    this->setBrush( QBrush(QColor(Qt::lightGray),Qt::SolidPattern) );

}

QGIHider::~QGIHider()
{
    //qDebug() << "QGIHider::~QGIHider()";
    delete impl;
}
void QGIHider::refreshTransformation()
{
    QVariant v( this->property("angle") );
    qreal angle = v.canConvert<qreal>() ? v.value<qreal>() : 0.0;
    v = this->property("scale");
    qreal scale = v.canConvert<qreal>() ? v.value<qreal>() : 1.0;
    if( 0 == scale ) scale = 1.0;
    QTransform tr( qboard::rotateAndScale( this->boundingRect(), angle, scale, scale, true ) );
    this->setTransform( tr );
}


void QGIHider::propertySet( char const *pname, QVariant const & var )
{
    typedef QMap<QString,int> PMAP;
    static PMAP pmap;
    if( pmap.isEmpty() )
    {
#define MAP(S,V) pmap[S] = Impl::V;
	MAP("color",PropColor);
	MAP("alpha",PropAlpha);
	MAP("zLevel",PropZLevel);
	MAP("pos",PropPos);
	//MAP("scale",PropScale);
	//MAP("angle",PropAngle);
	//MAP("dragDisabled",PropDragDisabled);
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
    if( (Impl::PropScale == kid) || (Impl::PropAngle == kid) )
    {
	this->refreshTransformation();
	return;
    }
    if( Impl::PropColor == kid )
    {
	QBrush br( this->brush() );
	QColor old( br.color() );
	QColor col = var.value<QColor>();
	if( 255 == col.alpha() )
	{
	    col.setAlpha(old.alpha());
	}
	br.setColor(col);
	this->setBrush( br );
	this->update();
	return;
    }
    if( Impl::PropAlpha == kid )
    {
	qreal a = var.toDouble();
	QBrush br = this->brush();
	QColor col = br.color();
	if( a > 1 )
	{ // assume it's int-encoded
	    col.setAlpha( int(a) );
	}
	else
	{
	    col.setAlphaF(a);
	}
	br.setColor(col);
	this->setBrush( br );
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

bool QGIHider::event( QEvent * e )
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

bool QGIHider::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    if( ! impl->item ) return true;
    Serializable const * s = dynamic_cast<Serializable const *>( impl->item );
    if( ! s )
    {
	throw s11n::s11n_exception( "QGIHider cannot serialize hidden non-Serializable QGI." );
    }

    if( this->metaObject()->propertyCount() )
    {
	QObject props;
	props.setProperty("zLevel", this->zValue() );
// 	props.setProperty("angle", this->property("angle") );
// 	props.setProperty("scale", this->property("scale") );
	S11nNode & pr( s11n::create_child( dest, "properties" ) );
	QObject const & constnessKludge( props );
	if( ! s11n::qt::QObjectProperties_s11n()( pr, constnessKludge ) ) return false;
    }

    return s11n::serialize_subnode( dest, "item", *s )
	&& s11n::serialize_subnode( dest, "pos", this->pos() )
	&& s11n::serialize_subnode( dest, "brush", this->brush() )
        && s11n::serialize_subnode( dest, "transform", this->transform() )
	;
}

bool QGIHider::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    QGraphicsItem * old = this->unhideItem();
    if( old && ((!old->parentItem()) && (!old->scene()) ) )
    {
	delete old;
    }
    typedef S11nNodeTraits NT;
    QPointF pos;
    if( s11n::deserialize_subnode( src, "pos", pos ) )
    {
	this->setPos( pos );
    }
    else
    {
	qWarning("QGIHider::deserialize: couldn't deser 'pos'");
    }
    S11nNode const * ch = 0;
    ch = s11n::find_child_by_name( src, "item" );
    if( ! ch ) return true;
    Serializable * s = s11nlite::deserialize<Serializable>( *ch );
    if( ! s )
    {
	throw s11n::s11n_exception("QGIHider::deserialization of child item failed!");
    }
    QGraphicsItem * it = dynamic_cast<QGraphicsItem*>( s );
    if( ! it )
    {
	s11n::cleanup_serializable( s );
	throw s11n::s11n_exception("QGIHider::deserialized child is not a QGI!");
    }
    pos = this->pos();
    this->hideItem( it );
    this->setPos(pos);

    ch = s11n::find_child_by_name( src, "brush" );
    if( ch )
    {
	QBrush brush;
	s11n::deserialize( *ch, brush );
	this->setBrush(brush);
    }
    ch = s11n::find_child_by_name( src, "transform" );
    if( ch )
    {
	QTransform tr;
	s11n::deserialize( *ch, tr );
	this->setTransform(tr);
    }

    ch = s11n::find_child_by_name(src, "properties");
    if( ch )
    {
	s11n::qt::QObjectProperties_s11n()( *ch, *this );
    }
    return true;
}


void QGIHider::hideItem( QGraphicsItem * toHide )
{
    if( impl->item == toHide ) return;
    else if( impl->item )
    {
	this->unhideItem();
    }
    if( (impl->item = toHide) )
    {
	bool sel = toHide->isSelected();
	toHide->setSelected(false);
	this->setParentItem(0);
	this->setTransform( toHide->transform() );
	this->setFlags( toHide->flags() );
	this->setZValue( toHide->zValue() );
	this->setPath( this->shape() );
	QGraphicsScene * sc = toHide->scene();
#if 0
	// During deserialization, neither of us have a scene...
 	if( ! sc )
 	{
 	    sc = this->scene();
// 	    if( sc ) sc->removeItem(this);
 	}
#endif
	QGraphicsItem * par = toHide->parentItem();
	if( par )
	{
	    //qDebug() << "QGIHider::hideItem("<<toHide<<") removing toHide from parent.";
	    toHide->setParentItem(0);
	}
	if( sc )
	{
	    //qDebug() << "QGIHider::hideItem("<<toHide<<") removing toHide from scene.";
	    sc->removeItem(toHide);
	}
	toHide->setParentItem(this);
	toHide->setVisible(false);
	if( sc || par )
	{
	    this->setPos( toHide->pos() );
	}
	//toHide->setPos(QPointF(0,0));
	QObject * obj = dynamic_cast<QObject*>(toHide);
	QColor color;
	if( obj )
	{
	    QVariant cv( obj->property("color") );
	    if( cv.canConvert<QColor>() )
	    {
		color = cv.value<QColor>();
	    }
//  	    this->setProperty("angle", obj->property("angle") );
//  	    this->setProperty("scale", obj->property("scale") );
	}
	QBrush br( this->brush() );
	br.setColor(color);
	this->setBrush( br );
	if( sc )
	{
	    sc->addItem(this);
	}
	this->setParentItem(par);
	this->setVisible(true);
	this->setSelected( sel );
    }
}

void QGIHider::hideItems( QGraphicsItem * toHide )
{
    if( ! toHide ) return;
    typedef QList<QGraphicsItem*> QGIL;
    QGIL li;
    if( toHide->isSelected() && toHide->scene() )
    {
	li = toHide->scene()->selectedItems();
    }
    else
    {
	li.push_back( toHide );
    }
    for( QGIL::iterator it = li.begin();
	 it != li.end(); ++it )
    {
	QGraphicsItem * i = *it;
	if( QGITypes::QGIHider == i->type() ) continue;
	QGIHider::createHider( i );
    }
}

QGraphicsItem * QGIHider::unhideItem()
{
    if( ! impl->item ) return 0;
    QGraphicsItem * it = impl->item;
    impl->item = 0;
    bool sel = this->isSelected();
    this->setVisible(false);
    this->setSelected(false);
    it->setPos( this->pos() );
    it->setZValue( this->zValue() );
    QGraphicsItem * par = this->parentItem();
    QGraphicsScene * sc = this->scene();
    if( sc && !par )
    {
	//qDebug() << "QGIHider::unhideItem("<<it<<") adding item to scene.";
	sc->removeItem(it); // avoids "QGraphicsScene::addItem: item has already been added to this scene"
	sc->addItem(it);
    }
    //qDebug() << "QGIHider::unhideItem("<<it<<") parenting item to"<<par;
    it->setParentItem(par);
    sc->removeItem(this);
    it->setVisible(true);
    it->setSelected( sel );
    return it;
}


/* static */ void QGIHider::unhideItem( QGIHider * h )
{
    if( h )
    {
	h->unhideItem();
	h->deleteLater();
    }
}
/* static */ void QGIHider::unhideItems( QGIHider * h )
{
    if( ! h ) return;
    typedef QList<QGIHider*> QGIL;
    QGIL li;
    if( h->isSelected() && h->scene() )
    {
	li = qboard::graphicsItemsCast<QGIHider>( h->scene()->selectedItems() );
    }
    else
    {
	li.push_back( h );
    }
    for( QGIL::iterator it = li.begin();
	 it != li.end(); ++it )
    {
	QGIHider::unhideItem( *it );
    }
}

void QGIHider::unhideItems()
{
    QGIHider::unhideItems( this );
}

void QGIHider::mousePressEvent( QGraphicsSceneMouseEvent * ev )
{
    QGITypes::handleClickRaise( this, ev );
    this->QGraphicsPathItem::mousePressEvent(ev);
}


/* static */ QGIHider * QGIHider::createHider( QGraphicsItem * it )
{
    if( ! it ) return 0;
    QGIHider * h = new QGIHider;
    h->hideItem( it );
    return h;
}

QRectF QGIHider::boundingRect() const
{
#if 0
    QRectF r = impl->item
	? impl->item->boundingRect()
	: QRectF(0,0,50,50); // arbitrary
    return this->transform().mapRect(r);
#else
    // This value isn't correct when we're scaled independently of the item.
    return impl->item
	? impl->item->boundingRect()
	: QRectF(0,0,50,50);
#endif
}
QPainterPath QGIHider::shape() const
{
#if 0
	QPainterPath path;
	path.addRect( this->boundingRect() );
	return path;
#else
 	return impl->item
 	    ? impl->item->shape() // Qt4.4 = always rectangles
 	    : this->QGraphicsPathItem::path();
	/**
	   Curious:

	return this->QGraphicsPathItem::shape();
	
	On Qt4.3 that leaves us with an empty object. On Qt4.4
	it gives us rectangles.
	*/
#endif
}

QPainterPath QGIHider::opaqueArea() const
{
    return this->shape();
}

void QGIHider::paint(QPainter *painter,
		     const QStyleOptionGraphicsItem *opt,
		     QWidget *wid)
{
    this->QGraphicsPathItem::paint(painter,opt,wid);
}


void QGIHider::contextMenuEvent( QGraphicsSceneContextMenuEvent * ev )
{
    MenuHandlerCommon proxy;
    ev->accept();
    QMenu * m = proxy.createMenu( this );
    m->addAction(QIcon(":/QBoard/icon/box_unwrapped.png"),
		 "Uncover", this, SLOT(unhideItems()) );

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

    if(1)
    {
	m->addSeparator();
	QObjectPropertyMenu * pm =
	    QObjectPropertyMenu::makeColorMenu(selected, "color","alpha");
	pm->setIcon(QIcon(":/QBoard/icon/color_fill.png"));
	m->addMenu(pm);

#if 0
	// Scaling/rotating independently of the item causes graphics artefacts
	pm =
	    QObjectPropertyMenu::makeNumberListMenu("Scale",
						    selected, "scale",
						    0.5, 3.01, 0.5);
	pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	m->addMenu(pm);
	m->addMenu( QObjectPropertyMenu::makeNumberListMenu("Rotate",
							    selected,
							    "angle",
							    0,360,15) );
#endif
    }


    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( this, m );
    clipper->addDefaultEntries( m, true, this->isSelected() );
    m->exec( ev->screenPos() );
    delete m;
}
