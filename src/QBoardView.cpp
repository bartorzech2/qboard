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
#if QBOARDVIEW_USE_OPENGL
#include <QGLWidget>
#endif


#include "QBoard.h"
#include "utility.h"

struct QBoardView::Impl
{
    QBoard & board;
    qreal scale;
    bool glmode;

    Impl(QBoard &b)
	: board(b),
	  scale(1.0),
	  glmode(false)
    {
    }
};

QBoardView::QBoardView( QBoard & b, QGraphicsScene * par ) :
    QGraphicsView(par),
    impl( new Impl(b) )
{
    //this->setCacheMode(QGraphicsView::CacheBackground);
    connect( &impl->board, SIGNAL(loaded()), this, SLOT(updateBoardPixmap()) );
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setInteractive(true);
    this->setTransformationAnchor(QGraphicsView::NoAnchor);
    //this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setBackgroundBrush(QColor("#abb8fb"));
    this->viewport()->setObjectName( "QBoardViewViewport");
}

QBoardView::~QBoardView()
{
    QBOARD_VERBOSE_DTOR << "~QBoardView()";
    delete impl;
}

void QBoardView::setHandDragMode(bool handMode )
{
    this->setDragMode( handMode
		       ? QGraphicsView::ScrollHandDrag
		       : QGraphicsView::RubberBandDrag);
}


bool QBoardView::isGLMode() const
{
    return QBOARDVIEW_USE_OPENGL
	? impl->glmode
	: false;
}

void QBoardView::toggleGLMode()
{
#if QBOARDVIEW_USE_OPENGL
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
    QWidget * w = impl->glmode
	? new QGLWidget // (QGLFormat(QGL::SampleBuffers))
	: new QWidget; 
    w->setObjectName( "QBoardViewViewport");
    impl->glmode = !impl->glmode;
    setViewport( w );
#endif
}

QSize QBoardView::sizeHint() const
{
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
    this->updateGeometry();
}

void QBoardView::drawBackground( QPainter *p, const QRectF & rect )
{
    if( this->impl->board.pixmap().isNull() )
    {
	this->QGraphicsView::drawBackground(p,rect);
	return;
    }
    p->fillRect( rect, this->backgroundBrush() );
    p->drawPixmap(rect,impl->board.pixmap(),rect);
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
    // i don't get why this is so much work. The examples in the qt docs
    // make it look much simpler.
    if( (z < 0.10) || (z>4.01) ) return; // arbitrary! 
    if( z == impl->scale ) return;
    impl->scale = z;
    qDebug() << "QBoardView::zoom()"<<impl->scale;
#if 0
    QSizeF isz(impl->board.pixmap().size());
    if( 1.0 != z )
    {
	isz.scale( std::ceil( isz.width() * impl->scale ),
		   std::ceil( isz.height() * impl->scale ),
		   Qt::IgnoreAspectRatio );
    }
    this->resize(isz.toSize());
    this->scale( impl->scale, impl->scale );
#else
    QSizeF isz(impl->board.pixmap().size());
    if( 1.0 != z )
    {
	isz.scale( std::ceil( isz.width() * impl->scale ),
		   std::ceil( isz.height() * impl->scale ),
		   Qt::IgnoreAspectRatio );
    }
    //these resets reset our pos to (0,0) and undoes rotation, which is annoying. But we need one of them. :/
    this->resetTransform(); 
    //this->resetMatrix();
    this->resize(isz.toSize());
    this->scale( impl->scale, impl->scale );
#endif
    this->updateGeometry(); // without this, scrollbars get out of sync.
}

void QBoardView::zoomReset()
{
    this->zoom(1.0);
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
    { // center view on clicked pos.
	event->accept();
	this->centerOn( this->mapToScene(event->pos()) );
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
