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

#include <QDebug>
#include <QFont>
#include <QGraphicsItem>
#include <QStringList>
#include <cmath>

#include "QGIPiece.h"
#include "utility.h"
#include "S11nQt.h"
#include "S11nQtList.h"
#include "S11nClipboard.h"
#include "MenuHandlerGeneric.h"
#include "PropObj.h"

bool QGITypes::handleClickRaise( QGraphicsItem * it,
				 QGraphicsSceneMouseEvent * ev )
{
    if( ev->buttons() & Qt::LeftButton )
    {
	ev->accept();
	qreal zV = qboard::nextZLevel(it);
	if( zV > it->zValue() )
	{
	    //it->setProperty( "zLevel", zV );
	    it->setZValue( zV );
	}
	return true;
    }
    return false;
}

#define QGIPiece_USE_PIXCACHE 1

struct QGIPiece::Impl
{
    QColor bgColor;
    QColor borderColor;
    qreal borderSize;
    QPixmap pixmap;
#if QGIPiece_USE_PIXCACHE
    QPixmap pixcache;
#endif
    int borderLineStyle;
    /**
       When block is true, QGIPiece will reject piece property updates.
       This is a kludge to allow QGIPiece to properly update the piece
       position.
    */
    bool block;
    bool isCovered;
    size_t countPaintCache;
    size_t countRepaint;
    qreal alpha;
    Impl()
    {
	isCovered = false;
	borderSize = 1;
	borderLineStyle = Qt::SolidLine;
	block = false;
	countPaintCache = countRepaint = 0;
	alpha = 1;
    }
    ~Impl()
    {
	if(0) qDebug() << "QGIPiece: repaint count ="<<countRepaint
		       << "cached paint count ="<<countPaintCache;
    }
    void clearCache()
    {
#if QGIPiece_USE_PIXCACHE
	pixcache = QPixmap();
#endif
    }
};

#include <QGraphicsScene>
QGIPiece::QGIPiece()
    : QObject(),
      QGraphicsPixmapItem(),
      Serializable("QGIPiece"),
      impl( new QGIPiece::Impl )
{
    this->setup();
}

void QGIPiece::setup()
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );
}

QGIPiece::~QGIPiece()
{
    QBOARD_VERBOSE_DTOR << "~QGIPiece()";
    delete this->impl;
}

void QGIPiece::refreshTransformation()
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

bool QGIPiece::event( QEvent * e )
{
#if 1
    while( e->type() == QEvent::DynamicPropertyChange )
    {
	QDynamicPropertyChangeEvent *chev = dynamic_cast<QDynamicPropertyChangeEvent *>(e);
	if( ! chev ) break;
	char const * key = chev->propertyName().constData();
	this->propertySet( key, this->property(key) );
	e->accept();
	break;
    }
#endif
    return QObject::event(e);
}

void QGIPiece::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property

    // FIXME: instead of doing string comparisons on pname, use a
    // lookup map with Impl member funcs as implementations. Make Impl
    // a friend class.
    if(0) qDebug() << "QGIPiece::propertySet("<<pname<<")[block="<<impl->block<<"] val="<<var;
    if( impl->block ) return;
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
 	QColor col = var.value<QColor>();
 	if( 255 == col.alpha() )
 	{
	    if( impl->alpha > 1 )
	    {
		col.setAlpha( int(impl->alpha) );
	    }
	    else
	    {
		col.setAlphaF( impl->alpha );
	    }
 	}
	impl->alpha = col.alpha();
	impl->bgColor = col;
	if(0) qDebug() << "QGIPiece::propertySet(color):"<<impl->bgColor<<" alpha ="<<impl->alpha;
	impl->clearCache();
	this->update();
	return;
    }
    if( "colorAlpha" == key )
    {
	qreal a = impl->alpha = var.toDouble();
	if( a > 1.0 )
	{ // assume it's int-encoded
	    impl->bgColor.setAlpha( int(a) );
	}
	else
	{
	    impl->bgColor.setAlphaF( a );
	}
	impl->clearCache();
    	this->update();
	return;
    }
    if( "borderColor" == key )
    {
// 	QColor old = impl->borderColor;
	QColor col = var.value<QColor>();
 	if( 255 == col.alpha() )
 	{
	    col.setAlphaF( impl->borderColor.alphaF() );
 	}
	impl->borderColor = col;
// 	if( 255 ==  )
// 	{
// 	    impl->borderColor.setAlphaF( old.alphaF() );
// 	}
	impl->clearCache();
	this->update();
	return;
    }
    if( "borderAlpha" == key )
    {
	qreal a = var.toDouble();
	if( a > 1.0 )
	{ // assume it's int-encoded
	    impl->borderColor.setAlpha( int(a) );
	}
	else
	{
	    impl->borderColor.setAlphaF( a );
	}
	impl->clearCache();
	this->update();
	return;
    }
    if( "borderSize" == key )
    {
	double bs = var.toDouble();;
	impl->borderSize = (bs >= 0) ? bs : 0;
	impl->clearCache();
	this->update();
	return;
    }
    if( "borderStyle" == key )
    {
	impl->clearCache();
	if( var.type() == QVariant::Int )
	{
	    const int i = var.toInt();
	    switch( i )
	    {
	      case Qt::NoPen:
	      case Qt::SolidLine:
	      case Qt::DashLine:
	      case Qt::DotLine:
	      case Qt::DashDotLine:
	      case Qt::DashDotDotLine:
		  impl->borderLineStyle = i;
		  break;
	      default:
		  impl->borderLineStyle = Qt::NoPen;
		  break;
	    }
	    this->update();
	    return;
	}
	else
	{
	    impl->borderLineStyle = qboard::stringToPenStyle(var.toString());
	    this->update();
	    return;
	}
    }
    if( ("scale" == key) || ("angle" == key) )
    {
	this->refreshTransformation();
	return;
    }
    if( "size" == key )
    {
	impl->clearCache();
	if( impl->pixmap.isNull() )
	{
	    QPixmap bogus( var.toSize() );
	    bogus.fill( QColor(Qt::transparent) );
	    impl->pixmap = bogus;
	    // Kludge to ensure bounding rect is kept intact
	    this->setPixmap(bogus);
	}
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
    if( "isCovered" == key )
    {
	int i = var.toInt();
	impl->isCovered = (0 != i);
	impl->clearCache();
	this->update();
	return;
    }
    if( "pixmap" == key )
    {
	this->prepareGeometryChange();
	impl->clearCache();
	QPixmap pix;
	if( var.canConvert<QPixmap>() )
	{
	    pix = var.value<QPixmap>();
	}
	else if( var.canConvert<QString>() )
	{
	    QString fname( var.toString() );
	    if( pix.load( fname ) )
	    {
		this->setProperty("size",pix.size());
	    }
	    else
	    {
		pix = QPixmap(300,40);
		QPainter pain(&pix);
		pain.fillRect( pix.rect(), Qt::yellow );
		QFont font( "courier", 8 );
		pain.setFont( font ); 
		pain.setPen( QPen(Qt::red) );
		pain.drawText(5,font.pointSize()+6,"Error loading pixmap:");
		pain.drawText(5,pix.height()-6,fname);
	    }
	} // var.canConvert<QString>()
	this->impl->pixmap = pix;
	if(1)
	{
	    // Kludge to ensure that this type's shape is properly set. We don't want
	    // the parent class to have the real pixmap, so that we can control all
	    // painting ourselves.
	    QPixmap bogus( pix.size() );
	    bogus.fill( QColor(Qt::transparent) );
	    this->setPixmap(bogus);
	}
	this->update();
	return;
    } // pixmap property
}

void QGIPiece::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    if( ev->buttons() & Qt::RightButton )
    {
	// Weird: if we don't do this, the QGraphicsView
	// doesn't know how to keep us selected (if we're selected)
	// and also has trouble knowing whether to show a menu
	// or not.
	// qDebug() <<"QGIPiece::mousePressEvent() RMB:"<<ev;
	// ev->ignore(); it doesn't matter if i accept or not!
	// ev->accept();
	return;
    }
    QGITypes::handleClickRaise( this, ev );
    this->QGraphicsPixmapItem::mousePressEvent(ev);
}

void QGIPiece::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev )
{
    this->QGraphicsPixmapItem::mouseDoubleClickEvent(ev);
}
void QGIPiece::mouseMoveEvent( QGraphicsSceneMouseEvent * ev )
{
    this->QGraphicsPixmapItem::mouseMoveEvent(ev);
}
void QGIPiece::mouseReleaseEvent( QGraphicsSceneMouseEvent * ev )
{
    this->QGraphicsPixmapItem::mouseReleaseEvent(ev);
}

#include <QGraphicsSceneContextMenuEvent>

void QGIPiece::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    event->accept();
    QGIPieceMenuHandler mh;
    mh.doMenu( this, event );
}

QRectF boundsOfChildren( QGraphicsItem const * qgi )
{
    typedef QList<QGraphicsItem*> QGIL;
    QGIL ch( qboard::childItems(qgi) );
    QRectF r;
    for( QGIL::const_iterator it = ch.begin();
	 ch.end() != it; ++it )
    {
	QGraphicsItem const * x = *it;
	QRectF r2( x->mapToParent(x->pos()), x->boundingRect().size() );
	r = r.unite( r2 );
    }
    if(1  && ! r.isNull() ) qDebug() << "bounds of children ="<<r;
    return r;
}


QVariant QGIPiece::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsPixmapItem::itemChange(change,value);
}
QRectF QGIPiece::boundingRect() const
{
    QRectF r( impl->pixmap.rect() );
    if( r.isNull() )
    {
	QVariant sz( this->property("size") );
	if( sz.canConvert<QSize>() )
	{
	    r.setSize(sz.value<QSize>());
	}
    }
#if 0
    // seems to mostly do what i want...
    if( 1 && (impl->borderSize > 0) )
    { // QGraphicsItem::boundingRect() docs say we need this:
	qreal pw( impl->borderSize / 2.0 ); // size/2.0 doesn't quite work for me - still get graphics artefacts
	r.setLeft( r.left() - pw );
	r.setRight( r.right() + pw );
	r.setTop( r.top() - pw );
	r.setBottom( r.bottom() + pw );
    }
#else
    if( 0 && (impl->borderSize > 0) )
    { // QGraphicsItem::boundingRect() docs say we need this:
	qreal bs = impl->borderSize;
	r.adjust( 0, 0, bs * 2, bs * 2 );
	//r.adjust( -bs, -bs, bs * 2, bs * 2 );
    }
#if 0
    r = r.normalized();
    r = QRectF( 0, 0,
		std::ceil(r.width()), std::ceil(r.height()) );
#endif
#endif
    return r; // r.normalized();
}



static void paintLinesToChildren( QGraphicsItem * qgi,
				  QPainter * painter,
				  QPen const & pen )
{
    typedef QList<QGraphicsItem*> QGIL;
    QGIL ch( qboard::childItems(qgi) );

    if( ch.isEmpty() ) return;
    QRectF prect( qgi->boundingRect() );
    QPointF mid( prect.left() + (prect.width() / 2),
		 prect.top() + (prect.height() / 2) );
    painter->save();
    for( QGIL::iterator it = ch.begin();
	 ch.end() != it; ++it )
    {
	QGraphicsItem * x = *it;
	QRectF xr( x->boundingRect() );
	QPointF xmid( xr.center() );
	//xmid = x->mapToParent( xmid );
	xmid = qgi->mapFromItem( x, xmid );
	painter->setPen( pen );
	painter->drawLine( QLineF( mid, xmid ) );
    }
    painter->restore();
}

void QGIPiece::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if(0)
    { // for me this only works in GL mode
	QPen linePen(Qt::red, 4, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
	paintLinesToChildren( this, painter, linePen );
    }
    /**
       Using a pixmap cache for the rendering saves a lot of repainting,
       but it causes rounding errors because QPixmap is int-based.
       The borders don't show up properly (off-by-one errors) at certain
       sizes and scales.
    */

    QRectF bounds( this->boundingRect().normalized() );
#define AMSG if(0) qDebug() << "QGIPixmap::paint():"
    if( 
#if QGIPiece_USE_PIXCACHE
       impl->pixcache.isNull()
#else
       1
#endif
       )
    {
	++impl->countRepaint;
	QPainter * cp = 0;
#if QGIPiece_USE_PIXCACHE
// 	QSizeF csize( std::ceil(bounds.width()),
// 		      std::ceil(bounds.height()) );
	QPixmap captcha( bounds.size().toSize() );
	AMSG << "bounds="<<bounds<<", pixcache.size ="<<captcha.size();
	captcha.fill( Qt::transparent );
	QPainter _cp( &captcha );
	cp = &_cp;
#else
	cp = painter;
#endif
	if( 1 ) // Background color
	{
	    if( impl->bgColor.isValid() )
	    {
		cp->fillRect( bounds, impl->bgColor );
	    }
	}
	const qreal bs = impl->borderSize;
	qreal xl = bs / 2.0;

	if( ! impl->isCovered )
	{
	    if( ! impl->pixmap.isNull() )
	    {
		// Weird: if i use impl->pixmap.rect() i get (0.5,0.5,W,H)
		QRectF pmr( QPointF(0,0), impl->pixmap.size() );
		//QRectF pmr( impl->pixmap.rect() );
		AMSG << "drawPixmap("<<pmr<<"...)";
		cp->drawPixmap(pmr, impl->pixmap, impl->pixmap.rect() );
	    }
	}
	else
	{
	    cp->fillRect( bounds.toRect(),
			  QBrush(impl->borderColor.lighter(),
				 Qt::Dense7Pattern)
			  );
	}

	if( bs && impl->borderColor.isValid() )
	{
	    QRectF br( bounds );
	    br.adjust( xl, xl, -xl, -xl );
#if QGIPiece_USE_PIXCACHE
 	    if( (int(bs+0.49) % 2) == 1 )
 	    { // kludge to avoid some off-by-one unsightlyness
		qreal fudge = 0.5;
 		br.adjust( -fudge, -fudge, -fudge, -fudge );
 	    }
#endif
	    cp->save();
	    cp->setPen( QPen(impl->borderColor,
			     bs,
			     Qt::PenStyle( impl->borderLineStyle ),
			     Qt::FlatCap,
			     Qt::MiterJoin ) );
	    AMSG << "drawRect("<<br<<"...) bs ="<<bs<<", xl ="<<xl;;
	    cp->drawRect( br );
	    cp->restore();
	}
#if QGIPiece_USE_PIXCACHE
	impl->pixcache = captcha;
#endif
    }
    else
    {
	++impl->countPaintCache;
	if(0) AMSG << "using cached image.";
    }
#if QGIPiece_USE_PIXCACHE
    painter->drawPixmap( bounds, impl->pixcache, bounds );
#endif
    // Let parent draw selection borders and such:
    this->QGraphicsPixmapItem::paint(painter,option,widget);
#undef AMSG
}

#include <QGraphicsSceneDragDropEvent>
void QGIPiece::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
{
//     this->ensureVisible();
//     qDebug() << "QGIPiece::dragMoveEvent() pos="<<event->pos();
    this->QGraphicsPixmapItem::dragMoveEvent(event);
}


bool QGIPiece::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    S11nNodeTraits::set( dest, "QGIFlags", int(this->flags()) );
    QList<QGraphicsItem *> chgi( qboard::childItems(this) );
    if( ! chgi.isEmpty() )
    {
	if( ! s11n::qt::serializeQGIList<Serializable>( s11n::create_child(dest,"children"),
							chgi, false ) )
	{
	    return false;
	}
    }
    // EVIL KLUDGE: we must ensure that our 'pos' property is set
    // properly. We must violate constness to do so or use a proxy
    // object :/
    PropObj props;
    qboard::copyProperties( this, &props );
#if 0
    QStringList no;
    no << "colorAlpha"
       << "borderAlpha"
	;
    foreach( QString k, no )
    {
	props.setProperty(k.toAscii().constData(), QVariant());
    }
#endif
    // i can't get the QColor alpha property to serialize (it's as if... well, no...)
    // so we save the alpha properties separately.
    props.setProperty("colorAlpha", impl->bgColor.alpha());
    props.setProperty("borderAlpha", impl->borderColor.alpha());
    props.setProperty("pos", this->pos() );
    props.setProperty("zLevel", this->zValue() );
    return s11n::serialize_subnode( dest,"props", props );
}

bool QGIPiece::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    {
	qboard::clearProperties(this);
	PropObj props;
	if( ! s11n::deserialize_subnode( src, "props", props ) )
	{
	    qDebug() << "QGIPiece::deserialize() deser of props node failed!";
	    return false;
	}
	qboard::copyProperties( &props, this );
    }

    {
	S11nNode const * ch = s11n::find_child_by_name(src, "children");
	if( ch )
	{
	    typedef QList<QGraphicsItem *> QGIL;
	    QGIL childs;
	    if( -1 == s11n::qt::deserializeQGIList<Serializable>( *ch,
				  childs,
				  this,
				  QGraphicsItem::ItemIsMovable ) )
	    {
		return false;
	    }
	}
    }
    return true;
}


struct QGIPieceMenuHandler::Impl
{
    QGraphicsScene * scene;
    QGIPiece * piece;
    Impl() :
	scene(0),
	piece(0)
	{
	}
	~Impl()
	{
		
	}
};

QGIPieceMenuHandler::QGIPieceMenuHandler() 
	: impl(new QGIPieceMenuHandler::Impl)
{
	
}

QGIPieceMenuHandler::~QGIPieceMenuHandler()
{
	delete this->impl;
}


void QGIPieceMenuHandler::clipList( QGIPiece * src, bool copy )
{
    if( !src || ! src->scene() ) return;
    typedef QList<QGIPiece*> QSL;
    QSL sel( qboard::selectedItemsCast<QGIPiece>( src->scene() ) );
    try
    {
	S11nClipboard::instance().serialize( sel );
    }
    catch(...)
    {
    }
    if( ! copy )
    {
	s11n::cleanup_serializable( sel );
    }
}

void QGIPieceMenuHandler::copyList()
{
    clipList( impl->piece, true );
}

void QGIPieceMenuHandler::cutList()
{
    clipList( impl->piece, false );
}

void QGIPieceMenuHandler::showHelp()
{
    qboard::showHelpResource("Game Pieces", ":/QBoard/help/classes/QGIPiece.html");
}

bool QGIPieceMenuHandler::copyPiece()
{
    return impl->piece
	? S11nClipboard::instance().serialize<Serializable>( *impl->piece )
	: false;
}

bool QGIPieceMenuHandler::cutPiece()
{
    if( this->copyPiece() )
    {
	impl->piece->deleteLater();
    }
    return false;
}


void QGIPieceMenuHandler::addChild()
{
    QObject * o = this->sender();
    QString name;
    if( o )
    {
	QAction * ac = qobject_cast<QAction*>(o);
	if( ac ) name = ac->text();
    }
    if(1) qDebug() << "QGIPieceMenuHandler::addChild() class ="<<name;
    if( name.isEmpty() ) return;
    Serializable * s = s11n::cl::classload<Serializable>( name.toAscii().constData() );
    if( s )
    {
	QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>(s);
	if( gi )
	{
	    gi->setParentItem( impl->piece );
	    gi->setFlag( QGraphicsItem::ItemIsSelectable, false );
	    gi->setFlag( QGraphicsItem::ItemIsMovable, true );
	}
	else
	{
	    s11n::cleanup_serializable( s );
	}
    }
    else
    {
	if(1) qDebug() << "QGIPieceMenuHandler::addChild() could not load object of type "<<name;
    }
}

#include "QGIHider.h"
QGraphicsItem * QGIPieceMenuHandler::hideItem()
{
    return QGIHider::createHider( impl->piece );
}

void QGIPieceMenuHandler::doMenu( QGIPiece * pv, QGraphicsSceneContextMenuEvent * ev )
{
    if( ! pv ) return;
    impl->piece = pv;
    impl->scene = pv->scene();
    typedef QList<QObject *> OL;
    OL selected;
    if(  pv->isSelected() )
    {
	selected = qboard::selectedItemsCast<QObject>( impl->scene );
    }
    else
    {
	selected.push_back( dynamic_cast<QObject*>(pv) );
    }
    MenuHandlerCommon proxy;
    ev->accept();
    QMenu * m = proxy.createMenu( pv );
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "colorAlpha" );
    mColor->setIcon(QIcon(":/QBoard/icon/color_fill.png"));
    m->addMenu(mColor);
    if(1)
    {
	QObjectPropertyMenu * pm = QObjectPropertyMenu::makeNumberListMenu("Rotate",selected,"angle",0,360,15);
	pm->setIcon(QIcon(":/QBoard/icon/rotate_cw.png"));
	m->addMenu(pm);
    }

    QMenu * mBrd = m->addMenu("Border");
    mBrd->addMenu( QObjectPropertyMenu::makeColorMenu(selected,"borderColor","borderAlpha") );
    mBrd->addMenu( QObjectPropertyMenu::makePenStyleMenu(selected,"borderStyle") );
    if(1)
    {
	mBrd->addMenu( QObjectPropertyMenu::makeNumberListMenu("Size",selected,"borderSize",0,8) );
    }

    QMenu * mMisc = m->addMenu("Misc.");

    if(0)
    {
	QVariant vcov = pv->property("isCovered");
	int icov = vcov.toInt();
	vcov = icov ? QVariant(int(0)) : QVariant(1);
	QObjectPropertyAction * act =
	    new QObjectPropertyAction( selected,
				       "isCovered",
				       vcov );
	act->setText( icov ? "Uncover" : "Cover" );
	act->setCheckable(true);
	if( icov ) act->setChecked(true);
	mMisc->addAction( act );
    }

    if(1)
    {
	mMisc->addAction("Cover",this,SLOT(hideItem()));
    }

    if(1)
    {
	QObjectPropertyMenu * pm =
	    QObjectPropertyMenu::makeNumberListMenu("Scale",
						    selected, "scale",
						    0.25, 3.01, 0.25);
	pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	mMisc->addMenu(pm);
    }
    
    if(1)
    {
	QMenu * mAdd = mMisc->addMenu("Create child...");
	mAdd->addAction( "QGIHtml", this, SLOT(addChild()) );
	mAdd->addAction( "QGIDot", this, SLOT(addChild()) );
#if ! QBOARD_VERSION
	// QGIDie isn't ready yet (20080827)
	mAdd->addAction( "QGIDie", this, SLOT(addChild()) );
#endif
    }
    
    if(1)
    {
	QVariant lock = pv->property("dragDisabled");
	bool locked = lock.isValid()
	    ? (lock.toInt() ? true : false)
	    : false;
	QVariant newVal = locked ? QVariant(int(0)) : QVariant(int(1));
	QObjectPropertyAction * act = new QObjectPropertyAction(selected,"dragDisabled",newVal);
	act->setIcon(QIcon(":/QBoard/icon/unlock.png"));
	act->setCheckable( true );
	act->blockSignals(true);
	act->setChecked( locked );
	act->blockSignals(false);
	act->setText(locked ? "Unlock position" : "Lock position");
	mMisc->addAction(act);
    }

#if 1
    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( pv, m );
    QMenu * copySub = clipper->addDefaultEntries( m, true, pv->isSelected() );
    if( copySub )
    {
	copySub->addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy selected as QList<QGIPiece*>",
			   this,SLOT(copyList()) );
	copySub->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut selected as QList<QGIPiece*>",
			   this,SLOT(cutList()) );
    }
    m->addSeparator();
#endif
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
    m->exec( ev->screenPos() );
    delete m;
}

