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

#include "QGIPiece.h"
#include "GamePiece.h"
#include "utility.h"
#include <QDebug>
#include <QFont>
#include <QGraphicsItem>
#include "MenuHandlerPiece.h"
#include "utility.h"
#include "S11nQt.h"
#include "S11nQtList.h"
#include "S11nClipboard.h"
#include "MenuHandlerGeneric.h"
#include "PropObj.h"

struct QGIPiece::Impl
{
    QColor backgroundColor;
    QColor borderColor;
    int borderSize;
    QPixmap pixmap;
    int borderLineStyle;
    int alpha;
    int borderAlpha;
    /**
       When block is true, QGIPiece will reject piece property updates.
       This is a kludge to allow QGIPiece to properly update the piece
       position.
    */
    bool block;
    Impl()
    {
	borderSize = 1;
	borderLineStyle = Qt::SolidLine;
	block = false;
	alpha = borderAlpha = 255;
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
	return true;
    }
#endif
    return QObject::event(e);
}

void QGIPiece::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property
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
	impl->backgroundColor = var.value<QColor>();
	impl->backgroundColor.setAlpha(impl->alpha);
	this->update();
	return;
    }
    if( "borderColor" == key )
    {
	impl->borderColor = var.value<QColor>();
	impl->borderColor.setAlpha(impl->alpha);
	this->update();
	return;
    }
    if( "colorAlpha" == key )
    {
	impl->alpha = var.toInt();
	impl->backgroundColor.setAlpha(impl->alpha);
	this->update();
	return;
    }
    if( "borderAlpha" == key )
    {
	impl->borderAlpha = var.toInt();
	impl->borderColor.setAlpha(impl->borderAlpha);
	this->update();
	return;
    }
    if( "borderSize" == key )
    {
	int bs = var.toInt();;
	impl->borderSize = (bs >= 0) ? bs : 1;
	this->update();
	return;
    }
    if( "borderStyle" == key )
    {
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
	QString val = var.toString();
	impl->borderLineStyle = qboard::stringToPenStyle(val);
	this->update();
	return;
    }
    if( "size" == key )
    {
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
    if( "angle" == key )
    {
	qboard::rotateCentered( this, var.toDouble() );
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
    if( "pixmap" == key )
    {
	QPixmap pix;
	if( var.canConvert<QPixmap>() )
	{
	    impl->pixmap = var.value<QPixmap>();
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
    else if( ev->buttons() & Qt::LeftButton )
    {
	ev->accept();
	qreal zV = qboard::nextZLevel(this);
	if( zV > this->zValue() )
	{
	    this->setProperty( "zLevel", zV );
	}
    }
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
QRectF QGIPiece::boundingRect() const
{
    QRectF r( impl->pixmap.rect() );
    if( impl->borderSize > 0 )
    { // QGraphicsItem::boundingRect() docs say we need this:
	qreal pw( impl->borderSize / 1.8 ); // 2.0 doesn't quite work for me - still get graphics artefacts
	r.setLeft( r.left() - pw );
	r.setRight( r.right() + pw );
	r.setTop( r.top() - pw );
	r.setBottom( r.bottom() + pw );
    }
    return r;
}

QVariant QGIPiece::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsPixmapItem::itemChange(change,value);
}
void QGIPiece::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
#if 0
    this->QGraphicsPixmapItem::paint(painter,option,widget);
    return;
#endif
    QRect rect( this->boundingRect().toRect() );
    if( 1 ) // Background color
    {
	if( impl->backgroundColor.isValid() )
	{
	    painter->fillRect( rect, impl->backgroundColor );
	}
    }
    QPixmap const & pix( impl->pixmap );
    if( ! pix.isNull() )
    {
	//this->QGraphicsPixmapItem::paint(painter,option,widget);
	painter->drawPixmap(rect,pix,rect);
    }

    if( 1 ) // Border
    {
	if( impl->borderSize && impl->borderColor.isValid() )
	{
	    //qDebug() << "borderColor: bounding rect: "<<rect<<"borderSize="<<impl->borderSize<<"color: " << impl->borderColor;
	    painter->save();
	    painter->setPen( QPen(impl->borderColor,
				  qreal( impl->borderSize ),
				  Qt::PenStyle( impl->borderLineStyle ) ) );
	    painter->drawRect( rect );
	    painter->restore();
	}
    }
    // Let parent draw selection borders and such:
    this->QGraphicsPixmapItem::paint(painter,option,widget);
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
    // EVIL KLUDGE: we must ensure that our 'pos' property
    // is set properly. We must violate constness to do so.
    PropObj props;
    qboard::copyProperties( this, &props );
    props.setProperty("pos", this->pos() );
    return s11n::serialize_subnode( dest,"props", props );
}

bool QGIPiece::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    qboard::clearProperties(this);
    // Reminder: don't clear our properties here. Normally
    // we would do a full clean up, but in this case that's problematic.
    PropObj props;
    if( ! s11n::deserialize_subnode( src, "props", props ) )
    {
	qDebug() << "QGIPiece::deserialize() deser of props node failed!";
	return false;
    }
    qboard::copyProperties( &props, this );
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


void QGIPieceMenuHandler::copyQGIList()
{
    if( ! impl->scene ) return;
    typedef QList<QGraphicsItem*> QGIL;
    QGIL sel( impl->scene->selectedItems() );
    QList<QGIPiece*> sli;
    for( QGIL::iterator it = sel.begin(); sel.end() != it; ++it )
    {
	QGIPiece * pcv = dynamic_cast<QGIPiece*>( *it );
	if( ! pcv ) continue;
	sli.push_back(pcv);
    }
    try
    {
	S11nClipboard::instance().serialize( sli );
    }
    catch(...)
    {
    }
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
	QObjectPropertyMenu * pm = QObjectPropertyMenu::makeIntListMenu("Rotate",selected,"angle",0,360,15);
	pm->setIcon(QIcon(":/QBoard/icon/rotate_cw.png"));
	m->addMenu(pm);
    }

    QMenu * mBrd = m->addMenu("Border");
    mBrd->addMenu( QObjectPropertyMenu::makeColorMenu(selected,"borderColor","borderAlpha") );
    mBrd->addMenu( QObjectPropertyMenu::makePenStyleMenu(selected,"borderStyle") );
    if(1)
    {
	mBrd->addMenu( QObjectPropertyMenu::makeIntListMenu("Size",selected,"borderSize",0,8) );
    }

    if(1)
    {
	QMenu * mMisc = m->addMenu("Misc.");
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
    if( pv->isSelected() )
    {
	MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( pv, m );
	m->addAction(QIcon(":/QBoard/icon/editcopy.png"),tr("Copy selected"),clipper,SLOT(clipboardCopy()) );
	m->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut selected",clipper,SLOT(clipboardCut()) );
	QMenu * mcopy = m->addMenu(tr("Copy..."));
	mcopy->addAction(QIcon(":/QBoard/icon/editcopy.png"),"Selected as QList<Serializable*>",this,SLOT(copyQGIList()) );
    }
    else
    {
	m->addAction(QIcon(":/QBoard/icon/editcopy.png"),tr("Copy"),this,SLOT(copyPiece()) );
	m->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut",this,SLOT(cutPiece()) );
    }
    m->addSeparator();
#endif
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
    m->exec( ev->screenPos() );
    delete m;
}
