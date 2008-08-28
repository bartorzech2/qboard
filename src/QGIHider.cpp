#include "QGIHider.h"

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>
#include <QEvent>
#include <QWidget>
#include <QMenu>

#include "S11nQt.h"
#include "utility.h"
#include "MenuHandlerGeneric.h"


struct QGIHider::Impl
{
    QGraphicsItem * item;
    Impl()
	: item(0)
    {
    }
    ~Impl()
    {
	if( item ) delete item;
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
    this->setBrush( QColor(Qt::lightGray) );
}

QGIHider::~QGIHider()
{
    delete impl;
}

bool QGIHider::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    if( ! impl->item ) return true;
    //impl->item->setPos( this->pos() );
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
	this->setParentItem(0);
	this->setTransform( toHide->transform() );
	QGraphicsScene * sc = toHide->scene();
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
	this->setFlags( toHide->flags() );
	this->setZValue( toHide->zValue() );
	this->setPath( this->shape() );
	QObject * obj = dynamic_cast<QObject*>(toHide);
	if( obj )
	{
	    QVariant cv( obj->property("color") );
	    if( cv.canConvert<QColor>() )
	    {
		this->setBrush( cv.value<QColor>() );
	    }
	}
	if( sc )
	{
	    sc->addItem(this);
	}
	this->setParentItem(par);
	this->setVisible(true);
    }
}

QGraphicsItem * QGIHider::unhideItem()
{
    if( ! impl->item ) return 0;
    QGraphicsItem * it = impl->item;
    impl->item = 0;
    this->setVisible(false);
    it->setPos( this->pos() );
    it->setZValue( this->zValue() );
    QGraphicsItem * par = this->parentItem();
    QGraphicsScene * sc = this->scene();
    if( sc )
    {
	//qDebug() << "QGIHider::unhideItem("<<it<<") adding item to scene.";
	sc->addItem(it);
    }
    //qDebug() << "QGIHider::unhideItem("<<it<<") parenting item to"<<par;
    it->setParentItem(par);
    sc->removeItem(this);
    it->setVisible(true);
    return it;
}

QGraphicsItem * QGIHider::replaceItem()
{
    QGraphicsItem * it = impl->item;
    this->unhideItem();
    // Is this really legal?
    this->deleteLater();
    return it;
}

void QGIHider::mousePressEvent( QGraphicsSceneMouseEvent * ev )
{
    QGITypes::handleClickRaise( this, ev );
    this->QGraphicsPathItem::mousePressEvent(ev);
}


QGIHider * QGIHider::createHider( QGraphicsItem * it )
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
	QPainterPath path;
	path.addRect( this->boundingRect() );
	return path;

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
    m->addAction( "Uncover", this, SLOT(unhideItem()) );
    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( this, m );
    clipper->addDefaultEntries( m, true, this->isSelected() );

    m->exec( ev->screenPos() );
    delete m;
    if( ! impl->item )
    {
	this->deleteLater();
    }
}
