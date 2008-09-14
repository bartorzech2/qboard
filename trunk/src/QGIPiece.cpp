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

#include <qboard/QGIPiece.h>
#include <qboard/utility.h>
#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QList.h>
#include <qboard/S11nQt/S11nClipboard.h>
#include <qboard/MenuHandlerGeneric.h>
#include <qboard/PropObj.h>

#include <qboard/S11nQt/QGraphicsItem.h>
#include <qboard/S11nQt/QPointF.h>
#include <qboard/S11nQt/QPoint.h>
#include <qboard/S11nQt/QPen.h>
#include <qboard/S11nQt/QTransform.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>

Q_DECLARE_METATYPE(QGIPiece*)


#define QGIPiece_USE_PIXCACHE 1

struct QGIPiece::Impl
{
    QPixmap pixmap;
#if QGIPiece_USE_PIXCACHE
    QPixmap pixcache;
#endif
    size_t countPaintCache;
    size_t countRepaint;
    qreal alpha;
    QPen pen;
    QPen penB;
    bool blocked;
    enum PropIDs {
    PropUnknown = 0,
    PropAlpha,
    PropAngle,
    PropBorderAlpha,
    PropBorderColor,
    PropBorderSize,
    PropBorderStyle,
    PropColor,
    PropDragDisabled,
    PropPixmap,
    PropPos,
    PropScale,
    PropSize,
    PropZLevel,
    PropEND
    };
    Impl()
    {
	blocked = false;
	countPaintCache = countRepaint = 0;
	alpha = 1;

	penB = QPen(QColor(Qt::black),
		    1,
		    Qt::SolidLine,
		    Qt::FlatCap,
		    Qt::MiterJoin );
	pen = penB;
	pen.setColor(QColor(Qt::white));
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
    while( (!impl->blocked)
	   && (e->type() == QEvent::DynamicPropertyChange) )
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

void QGIPiece::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property
    typedef QMap<QString,int> PMAP;
    static PMAP pmap;
    if( pmap.isEmpty() )
    {
#define MAP(S,V) pmap[S] = Impl::V;
	MAP("color",PropColor);
	MAP("alpha",PropAlpha);
	MAP("colorAlpha",PropAlpha); // older name
	MAP("borderColor",PropBorderColor);
	MAP("borderSize",PropBorderSize);
	MAP("borderAlpha",PropBorderAlpha);
	MAP("borderStyle",PropBorderStyle);
	MAP("zLevel",PropZLevel);
	MAP("pos",PropPos);
	MAP("scale",PropScale);
	MAP("size",PropSize);
	MAP("angle",PropAngle);
	MAP("dragDisabled",PropDragDisabled);
	MAP("pixmap",PropPixmap);
#undef MAP
    }

    const QString key(pname?pname:"");
    int kid = pmap.value( key, Impl::PropUnknown );
    if( Impl::PropUnknown == kid ) return;
    if(0) qDebug() << "QGIPiece::propertySet("<<key<<")] val ="<<var;
    if( Impl::PropZLevel == kid )
    {
	this->setZValue(var.toDouble());
	this->update();
    }
    else if( Impl::PropPos == kid )
    {
	this->setPos( var.value<QPointF>() );
	this->update();
    }
    else if( Impl::PropColor == kid )
    {
 	QColor col = var.value<QColor>();
	qreal alpha = impl->pen.color().alphaF();
 	if( 255 == col.alpha() ) // keep original alpha level
 	{
	    if( alpha > 1 )
	    {
		col.setAlpha( int(alpha) );
	    }
	    else
	    {
		col.setAlphaF( alpha );
	    }
 	}
	impl->pen.setColor( col );
	if(0) qDebug() << "QGIPiece::propertySet(color):"<<impl->pen.color()<<" alpha ="<<impl->alpha;
	impl->clearCache();
	this->update();
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
	impl->pen.setColor( col );
	impl->clearCache();
    	this->update();
    }
    else if( Impl::PropBorderColor == kid )
    {
	QColor col = var.value<QColor>();
 	if( 255 == col.alpha() )
 	{
	    col.setAlphaF( impl->penB.color().alphaF() );
 	}
	impl->penB.setColor( col );
	impl->clearCache();
	this->update();
    }
    else if( Impl::PropBorderAlpha == kid )
    {
	qreal a = var.toDouble();
	QColor col( impl->penB.color() );
	if( a > 1.0 )
	{ // assume it's int-encoded
	    col.setAlpha( int(a) );
	}
	else
	{
	    col.setAlphaF( a );
	}
	impl->penB.setColor( col );
	impl->clearCache();
	this->update();
    }
    else if( Impl::PropBorderSize == kid )
    {
	double bs = var.toDouble();
	impl->penB.setWidth( (bs >= 0) ? bs : 0 );
	impl->clearCache();
	this->update();
    }
    else if( Impl::PropBorderStyle == kid )
    {
	impl->clearCache();
	impl->penB.setStyle( s11n::qt::stringToPenStyle(var.toString()) );
	this->update();
    }
    else if( (Impl::PropScale == kid) || (Impl::PropAngle == kid) )
    {
	this->refreshTransformation();
    }
    else if( Impl::PropDragDisabled == kid )
    {
	if( var.isValid() )
	{
	    this->setFlag( QGraphicsItem::ItemIsMovable, var.toInt() ? false : true );
	}
	else
	{
	    this->setFlag( QGraphicsItem::ItemIsMovable, true );
	}
    }
    else if( Impl::PropPixmap == kid )
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
	    QString fname( qboard::homeRelative(var.toString()) );
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
    } // pixmap property
    if( Impl::PropSize == kid )
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
    }
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
    emit doubleClicked( this );
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
	    QColor col = impl->pen.color();
	    if( col.isValid() )
	    {
		cp->fillRect( bounds, col );
	    }
	}
	const qreal bs = impl->penB.widthF();
	qreal xl = bs / 2.0;

	if( ! impl->pixmap.isNull() ) // Draw pixmap
	{
	    // Weird: if i use impl->pixmap.rect() i get (0.5,0.5,W,H)
	    QRectF pmr( QPointF(0,0), impl->pixmap.size() );
	    //QRectF pmr( impl->pixmap.rect() );
	    AMSG << "drawPixmap("<<pmr<<"...)";
	    cp->drawPixmap(pmr, impl->pixmap, impl->pixmap.rect() );
	}

	if( bs && impl->penB.color().isValid() ) // Draw border
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
	    cp->setPen( impl->penB );
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
    //S11nNodeTraits::set( dest, "QGIFlags", int(this->flags()) );
    QList<QGraphicsItem *> chgi( qboard::childItems(this) );
    if( ! chgi.isEmpty() )
    {
	if( -1 == s11n::qt::serializeQGIList<Serializable>( s11n::create_child(dest,"children"),
							    chgi, false ) )
	{
	    return false;
	}
    }
    const QTransform tra( this->transform() );
    if( ! tra.isIdentity() )
    {
        s11n::serialize_subnode( dest, "transform", tra );
    }

    // EVIL KLUDGE: we must ensure that our 'pos' property is set
    // properly. We must violate constness to do so or use a proxy
    // object :/
    PropObj props;
#if 0
    qboard::copyProperties( this, &props );
#if 1
    QStringList no;
    no << "alpha"
       << "angle"
       << "color"
       << "borderColor"
       << "borderSize"
       << "borderStyle"
       << "borderAlpha"
       << "scale"
	;
    foreach( QString k, no )
    {
	props.setProperty(k.toAscii().constData(), QVariant());
    }
#endif
#endif
#define DO(K){ QVariant v( this->property(# K) ); if(v.isValid()) props.setProperty(# K,v); }
    DO(size);
    DO(pixmap)
    props.setProperty("pos", this->pos() );
    props.setProperty("zLevel", this->zValue() );
    return s11n::serialize_subnode( dest,"props", props )
	&& s11n::serialize_subnode( dest, "pen", impl->pen )
	&& s11n::serialize_subnode( dest, "borderPen", impl->penB );
}

bool QGIPiece::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    qboard::clearProperties(this);
    qboard::destroyQGIList( qboard::childItems(this) );
    S11nNode const * ch = 0;
    ch = s11n::find_child_by_name(src, "children");
    if( ch )
    {
	typedef QList<QGraphicsItem *> QGIL;
	QGIL childs;
	if( -1 == s11n::qt::deserializeQGIList<Serializable>( *ch,
							      childs,
							      this
							      //QGraphicsItem::ItemIsMovable
							      ) )
	{
	    return false;
	}
    }
    {
	/**
	   If "transform" entry is missing (i.e. was the Identity
	   Transformation, which we don't bother serializing), then
	   deser will fail, in which case we get an Identity
	   transform, which is what we want by default.
	*/
	QTransform tr;
	s11n::deserialize_subnode( src, "transform", tr );
	this->setTransform(tr);
    }
    ch = s11n::find_child_by_name(src, "pen");
    if( ch )
    {
	if( ! s11n::deserialize( *ch, impl->pen ) ) return false;
    }
    ch = s11n::find_child_by_name(src, "borderPen");
    if( ch )
    {
	if( ! s11n::deserialize( *ch, impl->penB ) ) return false;
    }
    { // properties must come last, to override some pen settings.
	PropObj props;
	if( ! s11n::deserialize_subnode( src, "props", props ) )
	{
	    qDebug() << "QGIPiece::deserialize() deser of props node failed!";
	    return false;
	}
	qboard::copyProperties( &props, this );
    }
    {
	/**
	   kludge: when i refactored some properties to a QPen,
	   it broke some other objects which relied on certain
	   properties. So now we stuff those back into the props list
	*/
	impl->blocked = true;
	this->setProperty("color",impl->pen.color());
	impl->blocked = false;
    }
    return true;
}

#include <qboard/QGIHider.h>
QGraphicsItem * QGIPiece::hideItems()
{
    QGIHider::hideItems( this );
    return this->parentItem();
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
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "alpha" );
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
    if(1)
    {
	mMisc->addAction(QIcon(":/QBoard/icon/box_wrapped.png"),
			 "Cover",pv,SLOT(hideItems()));
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

