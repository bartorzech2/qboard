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

#include <QApplication>
#include <QScrollBar>
#include <QImage>
#include <cmath>
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QGraphicsScene>
#include <QAction>

#include "QBoardView.h"
#include "GameState.h"
#include "GL.h"
#if QBOARD_USE_OPENGL
#include <QGLWidget>
#endif


#include "QBoard.h"
#include "utility.h"
#include "QGIPiecePlacemarker.h"

struct QBoardView::Impl
{
    GameState & gs;
    QBoard & board;
    qreal scale;
    bool glmode;
    QGIPiecePlacemarker * placer;
    QPoint placeAt;
    bool inMoveMode;
    Impl(GameState & s)
	: gs(s),
	  board(s.board()),
	  scale(1.0),
	  glmode(false),
	  placer(0),
	  placeAt(50,50),
	  inMoveMode(false)
    {
    }
    ~Impl()
    {
	delete placer;
    }
};

QBoardView::QBoardView( GameState & gs ) :
    QGraphicsView(gs.scene()),
    impl( new Impl(gs) )
{
    this->setProperty("scale", 1.0);
    this->setProperty("angle", 0.0);
    connect( &impl->board, SIGNAL(loadedBoard()), this, SLOT(updateBoardPixmap()) );
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setInteractive(true);
    this->setTransformationAnchor(QGraphicsView::NoAnchor);
    //this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
#if 1
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
#else
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
#endif
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setBackgroundBrush(QColor("#abb8fb"));
    this->viewport()->setObjectName( "QBoardViewViewport");
    this->setGLMode(false);
    /** ^^^^ gl mode seems to perform better but misses some updates,
	causing some pieces to become invisible until manually
	updating the display.
     */

    //this->setCacheMode(QGraphicsView::CacheBackground);
    //this->setOptimizationFlags( QGraphicsView::DontClipPainter );
#if 0
    // i can't seem to get local hotkeys for these objects. i have to
    // rely on app-global shortcuts. :(
    Qt::ShortcutContext cx( Qt::WindowShortcut );
#if QT_VERSION >= 0x040400
    cx = Qt::WidgetWithChildrenShortcut;
#endif
    QObject * acp = this; // this->viewport();
    QAction * ac = new QAction(acp);
    ac->setObjectName(QString::fromUtf8("actionCopy"));
    ac->setIcon(QIcon(QString::fromUtf8(":/QBoard/icon/editcopy.png")));
    ac->setShortcut(QString("Ctrl+C"));
    ac->setShortcutContext(cx);
    connect(ac,SIGNAL(triggered(bool)), this,SLOT(clipCopy()));

    ac = new QAction(acp);
    ac->setObjectName(QString::fromUtf8("actionCut"));
    ac->setIcon(QIcon(QString::fromUtf8(":/QBoard/icon/editccut.png")));
    ac->setShortcut(QString("Ctrl+X"));
    ac->setShortcutContext(cx);
    connect(ac,SIGNAL(triggered(bool)), this,SLOT(clipCut()));
    
    ac = new QAction(acp);
    ac->setObjectName(QString::fromUtf8("actionPaste"));
    ac->setIcon(QIcon(QString::fromUtf8(":/QBoard/icon/editpaste.png")));
    ac->setShortcut(QString("Ctrl+V"));
    ac->setShortcutContext(cx);
    connect(ac,SIGNAL(triggered(bool)), this,SLOT(clipPaste()));
#endif
    this->updateBoardPixmap();

}

QBoardView::~QBoardView()
{
    QBOARD_VERBOSE_DTOR << "~QBoardView()";
    if( impl->placer )
    {
	QObject::disconnect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
    }
    delete impl;
}

void QBoardView::refreshTransformation()
{
    qboard::rotateAndScale( this,
			    this->property("angle").toDouble(),
			    this->property("scale").toDouble() );
    this->updateGeometry(); // ensure scrollbars get synced
}

void QBoardView::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property
    // FIXME: instead of doing string comparisons on pname, use a
    // lookup map with Impl member funcs as implementations. Make Impl
    // a friend class.
    if(0) qDebug() << "QBoardView::propertySet("<<pname<<","<<var<<")"<<var;
    QString key( pname );
    if( "color" == key )
    {
	// TODO
	return;
    }
    if( "scale" == key )
    {
	impl->scale = var.toDouble();
	this->refreshTransformation();
	return;
    }
    if( "angle" == key )
    {
	this->refreshTransformation();
	return;
    }
    if( "pixmap" == key )
    {
	impl->board.setProperty( "pixmap", var );
	return;
    } // pixmap property
    // we could arguably handle "openglMode", but i don't want that serialized.
    if( "openglMode" == key )
    {
	if( var.isValid() )
	{
	    this->setGLMode( var.toInt() != 0 );
	}
	return;
    } // pixmap property
}



bool QBoardView::event( QEvent * e )
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
    return this->QGraphicsView::event(e);
}


static QGraphicsItem * firstSelectedQGI( QGraphicsScene * sc )
{
    if( ! sc ) return 0;
    typedef QList<QGraphicsItem *> QGIL;
    QGIL li = sc->selectedItems();
    QGraphicsItem * qgi = 0;
    for( QGIL::iterator it = li.begin();
	 li.end() != it; ++it )
    {
	if( (*it)->isSelected() )
	{
	    qgi = *it;
	    break;
	}
    }
    return qgi;
}

static void clipboardQGI( QGraphicsScene * sc, bool copy )
{
    QGraphicsItem * qgi = firstSelectedQGI( sc );
    if( qgi )
    {
	QPoint pos;
#if 0
	// Works, but centering is nicer.
	QRectF r( qboard::calculateBounds( qgi ) );
	if(0) qDebug() << "clipboardQGI() rect ="<<r;
	pos = r.topLeft().toPoint();
#else
	pos = qboard::calculateCenter( qgi ).toPoint();
	if(0) qDebug() << "clipboardQGI() center ="<<pos;
#endif
	if(0) qDebug() << "clipboardQGI() pos ="<<pos;
	qboard::clipboardScene( qgi->scene(), copy, pos );
    }
    return;
}

void QBoardView::clipCopy()
{
    clipboardQGI( impl->gs.scene(), true  );
}

void QBoardView::clipCut()
{
    clipboardQGI( impl->gs.scene(), false  );
}

static QPoint findBoardPastePoint( QGraphicsView * board )
{
    QPoint ret;
    QWidget * vp = board->viewport();
    if(1) qDebug() << "findBoardPastePoint("<<board<<") viewport ="<<vp
		   << "viewport parent ="<<vp->parent();
    QPoint glpos( QCursor::pos() );
    QWidget * wa = QApplication::widgetAt( glpos );
    if(0) qDebug() << "findBoardPastePoint("<<board<<") widgetAt("<<glpos<<") ="<<wa;
    if( wa != vp ) return ret;
    ret = board->mapToScene( board->mapFromGlobal(glpos) ).toPoint();
    if(0) qDebug() << "findBoardPastePoint("<<board<<") ret ="<<ret;
    return ret;
}

void QBoardView::clipPaste()
{
    //bool pasteGraphicsItems( impl->gs, QPoint() );
    //impl->gs.pasteClipboard( QPoint() );
    impl->gs.pasteClipboard( findBoardPastePoint(this) );
    //impl->gs.pasteClipboard( impl->gv->placementPos() ); // too randomish

}


void QBoardView::setHandDragMode(bool handMode )
{
    this->setDragMode( handMode
		       ? QGraphicsView::ScrollHandDrag
		       : QGraphicsView::RubberBandDrag);
}

GameState & QBoardView::state()
{
    return impl->gs;
}

bool QBoardView::isGLMode() const
{
    return QBOARD_USE_OPENGL
	? impl->glmode
	: false;
}

void QBoardView::setGLMode
#if ! QBOARD_USE_OPENGL
(bool) {}
#else
(bool on)
{
    /**
       This code causes a warning from Qt at runtime:

       QObject: Do not delete object, 'QBoardViewViewport', during its
       event handler!

       It's caused by QAbstractScrollArea::setViewport() deleting the
       original viewport, which is ACTUALLY where the event comes from
       which triggers this function in the first place. Thus the
       warning.

       A bug report has been filed with trolltech, but i now can't
       find it in their tracker. :/
    */
    QWidget * w = (impl->glmode=on)
	? new QGLWidget // (QGLFormat(QGL::SampleBuffers))
	: new QWidget; 
    w->setObjectName( "QBoardViewViewport");
    setViewport( w );
}
#endif

QSize QBoardView::sizeHint() const
{
#if 1
    QSizeF sz;
    if( ! this->impl->board.pixmap().isNull() )
    {
	sz = this->impl->board.pixmap().size();
	if( impl->scale != 1.0 )
	{
	    sz.setWidth( sz.width() * impl->scale );
	    sz.setHeight( sz.height() * impl->scale );
	}
    }
    else
    {
	sz = this->QGraphicsView::sizeHint();
    }
    return sz.toSize();
#else
    return this->QGraphicsView::sizeHint();
#endif
}
void QBoardView::updateBoardPixmap()
{
    //this->setBackgroundBrush(impl->board.pixmap());
    //if( impl->board.pixmap().isNull() ) return;
    if( ! impl->board.pixmap().isNull() )
    {
	QSize isz = impl->board.pixmap().size();
	QRectF rect( 0, 0, isz.width(),  isz.height() );
	//qDebug() << "QBoardView::updateBoardPixmap() setting scene rect ="<<rect;
	this->setSceneRect( rect );
#if 0
	/**
	   Kludge to force the scene rect to grow (if needed). My
	   setting of it up there apparently gets lost along the way
	   at times.
	*/
	
	if( impl->placer )
	{
	    QGraphicsItem * it = impl->placer;
	    QPointF p1( it->pos() );
	    QRectF itb( it->boundingRect() );
	    QPointF p2( rect.right() - (itb.width() / 2),
			rect.bottom() - (itb.height() / 2) );
	    impl->placer->setPos( p2 );
	    impl->placer->setPos( p1 );
	}
#endif
    }
    //this->setBackgroundBrush(impl->board.pixmap());
    // Kludge to get boards to keep their scale on a reload:
    impl->scale -= 0.01;
    this->zoom( impl->scale + 0.01 );
    this->viewport()->update(); // without this, viewport won't update until the board is manipulated
    this->updateGeometry();
}

void QBoardView::drawBackground( QPainter *p, const QRectF & rect )
{
    if( this->impl->board.pixmap().isNull() )
    {
	this->QGraphicsView::drawBackground(p,rect);
    }
    else
    {
#if 0 // do we want this?
	if( impl->board.pixmap().hasAlpha() )
	{
	    this->QGraphicsView::drawBackground(p,rect);
	    //p->fillRect( rect, this->backgroundBrush() );
	}
#endif
	QRect bogo = impl->board.pixmap().rect();
	p->drawPixmap(bogo, impl->board.pixmap(), bogo );
    }
   
}

void QBoardView::wheelEvent(QWheelEvent *event)
{
    if( (event->modifiers() & Qt::ControlModifier) )
    {
	if( event->delta() < 0 )
	{
	    this->zoomIn();
	}
	else
	{
	    this->zoomOut();
	}
	event->accept();
	return;
    }
#if 1
    // There's gotta be an easier way to handle wheel-scroll?
    QSize vsz( viewport()->size() );
    bool vertical = ((event->modifiers() & Qt::AltModifier) ? false : true);
    bool down = (event->delta()<0);
    const int factor = 2;
    int stride = 
	(vertical ? (vsz.height() / factor) : (vsz.width() / factor))
	* (down ? 1 : -1);
    if( vertical )
    {
	int y = verticalScrollBar()->value() + stride;
	if( y < 0 ) y = 0;
	verticalScrollBar()->setSliderPosition( y );
    }
    else
    {
	int x = horizontalScrollBar()->value() + stride;
	if( x < 0 ) x = 0;
	horizontalScrollBar()->setSliderPosition( x );
    }
    //horizontalScrollBar()->setRange(0, vsz.width() - mysz.width());
    //verticalScrollBar()->setPageStep(vsz.height()/3);
    //horizontalScrollBar()->setPageStep(vsz.width()/3);
    //verticalScrollBar()->setRange(0, vsz.height() - mysz.height());
    //horizontalScrollBar()->setRange(0, vsz.width() - mysz.width());
#endif
}
static const double BoardZoomScaleFactor = 0.25;
void QBoardView::zoomIn()
{
    this->zoom( impl->scale + BoardZoomScaleFactor );
}


void QBoardView::zoomOut()
{
    this->zoom( impl->scale - BoardZoomScaleFactor );
}

void QBoardView::zoom( qreal z )
{
    if( (z < 0.10) || (z>5.01) ) return; // arbitrary!
    if( z == impl->scale ) return;
#if 1
    this->setProperty("scale", z);
#else
    impl->scale = z;
    //qDebug() << "QBoardView::zoom()"<<impl->scale;
    //resetTransform() undoes rotation, which is annoying. But we need it to get the behaviour i want. :/
    this->resetTransform();
    this->scale( impl->scale, impl->scale );
    this->updateGeometry(); // without this, scrollbars get out of sync.
#endif
}

void QBoardView::zoomReset()
{
    this->zoom(1.0);
}

void QBoardView::placemarkerDestroyed()
{
    /**
       This bit of kludgery is to work around impl->placer being
       destroyed when game state is cleared (destroying all
       QGraphicsItems), as that leads to a dangling impl->placer in
       this class. When that happens, we re-create the placemarker
       if we had one active already.
    */
    if( impl->placer ) // now (or soon) a dangling pointer
    {
	impl->placer = 0;
	this->enablePlacemarker(true);
    }
}

void QBoardView::mouseReleaseEvent( QMouseEvent * event )
{
    if( impl->inMoveMode )
    {
	impl->inMoveMode = false;
	this->setHandDragMode( false );
	this->setInteractive( true );
	event->accept();
	return;
    }
    this->QGraphicsView::mouseReleaseEvent(event);
}

void QBoardView::mouseDoubleClickEvent( QMouseEvent * event )
{
    if( event->button() & Qt::LeftButton )
    {
	impl->placeAt = this->mapToScene(event->pos()).toPoint();
	if( impl->placer )
	{
 	    impl->placer->setPos( impl->placeAt );
	}
	event->accept();
	return;
    }
    this->QGraphicsView::mouseDoubleClickEvent(event);

}

void QBoardView::mousePressEvent( QMouseEvent * event )
{
#if 0
    if (QGraphicsItem *item = itemAt(event->pos())) {
	qDebug() << "QBoardView::mousePressEvent(): You clicked on item" << item;
    } else {
	qDebug() << "QBoardView::mousePressEvent(): You didn't click on an item.";
    }
#endif
#if 0
    /**
       Weird: if i enable this Ctrl-LMB-drag-mode, all items
       become (and stay) immobile once the drag is completed
       (mouseReleaseEvent()).
    */
    if( (event->button() & Qt::LeftButton)
	&&
	(event->modifiers() & Qt::ControlModifier)
	)
// #else
//     if( event->button() & Qt::MidButton ) // not triggering for me?
#else
    if(0)
#endif
    {
	impl->inMoveMode = true;
	this->setInteractive( true );
	this->setHandDragMode( true );
	this->QGraphicsView::mousePressEvent(event);
	event->accept();
	return;
    }
    if( event->button() & Qt::RightButton )
    {
	/**
	   If we don't do this then right-clicking a selected item
	   will cause the selection to be unselected. Whether we
	   accept() the event or not appears to make no difference
	   whatsoever.

	   Took me a frigging hour to find the cause of my selections
	   being lost.
	*/
	return;
    }
    this->QGraphicsView::mousePressEvent(event);
}

QBoard & QBoardView::board()
{
    return impl->board;
}

void QBoardView::dragMoveEvent( QDragMoveEvent * ev )
{
    //qDebug() << "QBoardView::dragMoveEvent() pos =="<<ev->pos();
    return QGraphicsView::dragMoveEvent(ev);
}

void QBoardView::selectAll()
{
    QGraphicsScene * sc = this->scene();
    if( sc )
    {
	QPainterPath path;
	path.addRect( sc->itemsBoundingRect() );
	sc->setSelectionArea( path, Qt::IntersectsItemShape );
    }

}

#include <QContextMenuEvent>
#include "MenuHandlerBoard.h"
void QBoardView::contextMenuEvent( QContextMenuEvent * event )
{
    // i can't believe this is the only way to know if the menu event
    // needs to be passed to an item!
    if( ! this->itemAt( event->pos() ) )
    {
	MenuHandlerBoard mh;
	mh.doMenu( impl->gs, this, event );
    }
    else
    {
	// This will pass on the event to that item. Somehow.
	// Not quite sure why, but it works.
	// There is no public API for doing the proper
	// context menu type conversion, so we have to rely
	// on this voodoo.
	this->QGraphicsView::contextMenuEvent(event);
    }
}

void QBoardView::addItem( QGraphicsItem * vi )
{
    QPoint pos;
    if( impl->placer )
    {
	QPoint p(impl->placer->pos().toPoint());
	// If the size is set, use it. Since pc is
	// very unlikely to have a view so far, its
	// size is not likely to be set.
	QVariant var( vi->boundingRect().size() );
	if( var.isValid() )
	{
	    QSize sz(var.toSize() );
	    pos = p - QPoint( sz.width()/2, sz.height()/2 );
	}
	else
	{
	    pos = p;
	}
    }
    else
    {
	pos = impl->placeAt;
    }
    vi->setPos( pos );
    impl->gs.addItem(vi);
#if 0
    QObject * pc = dynamic_cast<QObject*>(vi);
    if( vi )
    {
	pc->setProperty("pos", pos );
    }
#endif
}
void QBoardView::enablePlacemarker( bool en )
{
    if( en )
    {
	if( ! impl->placer )
	{
	    impl->placer = new QGIPiecePlacemarker;
	    connect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
	    
	    impl->placer->setPos(impl->placeAt);
	    impl->gs.scene()->addItem( impl->placer );
	    QStringList help;
	    help << "<html><body>This is a \"piece placemarker\"."
		 << "To move it, double-click the board or drag it around."
		 << "Pieces which are loaded from individual files (as opposed to being part of a game)"
		 << "start out at this position."
		 << "</body></html>"
		;
	    impl->placer->setToolTip( help.join(" ") );
	}
	return;
    }
    if( impl->placer )
    {
	QObject::disconnect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
    }
    delete impl->placer;
    impl->placer = 0;
}

QPoint QBoardView::placementPos()
{
    return impl->placer
	? impl->placer->pos().toPoint()
	: QPoint(0,0);
}
