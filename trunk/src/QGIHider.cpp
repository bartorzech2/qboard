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

struct QGIHider::Impl
{
    QGraphicsItem * item;
    Qt::BrushStyle bstyle;
    Impl()
	: item(0),
	  bstyle(Qt::SolidPattern)
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
    this->setBrush( QBrush(QColor(Qt::lightGray), impl->bstyle) );
}

QGIHider::~QGIHider()
{
    //qDebug() << "QGIHider::~QGIHider()";
    delete impl;
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
    return s11n::serialize_subnode( dest, "item", *s )
	&& s11n::serialize_subnode( dest, "pos", this->pos() );
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
	qDebug() << "QGIHider::deserialize: couldn't deser 'pos'!";
    }
    S11nNode const * ch = s11n::find_child_by_name( src, "item" );
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
	}
	this->setBrush( QBrush(color, impl->bstyle) );
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
    return impl->item
	? impl->item->boundingRect()
	: QRectF(0,0,50,50); // arbitrary
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

bool QGIHider::event( QEvent * e )
{
    return QObject::event(e);
}

void QGIHider::contextMenuEvent( QGraphicsSceneContextMenuEvent * ev )
{
    MenuHandlerCommon proxy;
    ev->accept();
    QMenu * m = proxy.createMenu( this );
    m->addAction(QIcon(":/QBoard/icon/box_unwrapped.png"),
		 "Uncover", this, SLOT(unhideItems()) );
    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( this, m );
    clipper->addDefaultEntries( m, true, this->isSelected() );
    m->exec( ev->screenPos() );
    delete m;
}
