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

#include <QScrollBar>
#include <QImage>
#include <cmath>
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QGraphicsScene>

#include "QBoardView.h"
#include "GameState.h"
#include "GamePiece.h"
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
    Impl(GameState & s)
	: gs(s),
	  board(s.board()),
	  scale(1.0),
	  glmode(false),
	  placer(0),
	  placeAt(20,20)
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
    //this->setCacheMode(QGraphicsView::CacheBackground);
    connect( &impl->board, SIGNAL(loaded()), this, SLOT(updateBoardPixmap()) );
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
    if( impl->board.pixmap().isNull() ) return;
    QSize isz = impl->board.pixmap().size();
    this->resetTransform();
    this->resetMatrix();
    QRectF rect( 0, 0, isz.width(),  isz.height() );
    this->setSceneRect( rect );
    //this->setBackgroundBrush(impl->board.pixmap());
    // Kludge to get minimaps to keep their scale on a reload:
    impl->scale -= 0.01;
    this->zoom( impl->scale + 0.01 );
}

void QBoardView::drawBackground( QPainter *p, const QRectF & rect )
{
    if( this->impl->board.pixmap().isNull() )
    {
	this->QGraphicsView::drawBackground(p,rect);
    }
    else
    {
#if 0
	// todo: only fillRect for pixmaps with alpha
	p->fillRect( rect, this->backgroundBrush() );
#endif
	QRect bogo = impl->board.pixmap().rect();
	p->drawPixmap(bogo, impl->board.pixmap(), bogo );
    }
#if 0
    if( 1 )
    { // temporary workaround while i work out a crash bug in QGIPiecePlacemarker:
	QColor color( Qt::yellow );
	QColor xline( Qt::red );
	const int rad = 12;
	//QRect brect( -rad,-rad,2*rad,2*rad);
	int w = 2*rad;
	int step = int(w / 6);
	int stopX = impl->placeAt.x();
	int x = stopX - rad;
	int y = impl->placeAt.y()-rad;
	int w2 = w;
	int h2 = 2*rad;
	bool which = true;
	p->setPen( QPen( Qt::NoPen ) );
	while( x < stopX )
	{
	    QColor c( which ? xline : color );
	    which = !which;
#if 1
	    QRadialGradient grad(0,0,x);
	    //grad.setFocalPoint(-(w2/3),-(h2/3));
	    grad.setColorAt( 0, c );
	    grad.setColorAt( 1, c.light(130) );
	    p->setBrush( grad );
#else
	    p->setBrush( c );
#endif
	    p->drawEllipse( x, y, w2, h2 );
	    x += step;
	    y += step;
	    w2 -= 2*step;
	    h2 -= 2*step;
	}
    }
#endif // draw placemarker
    
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
    if( (z < 0.10) || (z>4.01) ) return; // arbitrary! 
    if( z == impl->scale ) return;
    impl->scale = z;
    //qDebug() << "QBoardView::zoom()"<<impl->scale;
    //resetTransform() undoes rotation, which is annoying. But we need it to get the behaviour i want. :/
    this->resetTransform();
    this->scale( impl->scale, impl->scale );
    this->updateGeometry(); // without this, scrollbars get out of sync.
}

void QBoardView::zoomReset()
{
    this->zoom(1.0);
}

void QBoardView::placemarkerDestroyed()
{
    if( impl->placer ) // now (or soon) a dangling pointer
    {
	impl->placer = 0;
	this->enablePlacemarker(true);
    }
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
    if( event->button() & Qt::MidButton )
    {
	impl->placeAt = this->mapToScene(event->pos()).toPoint();
	if( impl->placer )
	{
 	    impl->placer->setPos( impl->placeAt );
	}
#if 0
	else
	{
	    // center view on clicked pos.
	    this->centerOn( impl->placeAt );
	}
#endif
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
	mh.doMenu( this, event );
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

void QBoardView::addPiece( GamePiece * pc )
{
    QPoint pos;
    if( impl->placer )
    {
	QPoint p(impl->placer->pos().toPoint());
	// If the size is set, use it. Since pc is
	// very unlikely to have a view so far, its
	// size is not likely to be set.
	QVariant var( pc->property("size") );
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
// 	QScrollBar * sb = this->verticalScrollBar();
// 	pos.setY( sb->sliderPosition() );
// 	sb = this->horizontalScrollBar();
// 	pos.setX( sb->sliderPosition() );
// 	// ^^^ this isn't valid when we're scaled!
    }
    qDebug() << "QBoardView::addPiece() pos ="<<pos;
    pc->setPieceProperty("pos", pos );
    impl->gs.addPiece(pc);
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
		 << "To move it, middle-click the board to move it or drag it around."
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
