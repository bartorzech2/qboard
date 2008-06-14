#include <QScrollBar>
#include <QImage>
#include <cmath>
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>

#if ! defined(QBOARDVIEW_USE_OPENGL)
#  define QBOARDVIEW_USE_OPENGL 0
/**
   Using GL mode for QBoardView seems to make many operations much
   faster, especially when zoomed/rotated.  However, the screen is not
   always updated properly in GL mode. :(
*/
#endif
#if QBOARDVIEW_USE_OPENGL
#include <QGLWidget>
#endif


#include "QBoardView.h"
#include "utility.h"

QBoardView::QBoardView( QBoard & b, QGraphicsScene * par ) :
    QGraphicsView(par),
    m_b(b),
    m_scale(1.0),
    m_pzoom(-1.0)
{
    //this->setCacheMode(QGraphicsView::CacheBackground);
    connect( &m_b, SIGNAL(loaded()), this, SLOT(updateBoardPixmap()) );
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setInteractive(true);
    this->setTransformationAnchor(QGraphicsView::NoAnchor);
    //this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setBackgroundBrush(QColor("#abb8fb"));
#if QBOARDVIEW_USE_OPENGL
    // Speeds up rendering a lot, especially for scaled views:
    this->setViewport( new QGLWidget );
#endif

}

QBoardView::~QBoardView()
{
    QBOARD_VERBOSE_DTOR << "~QBoardView()";
}
void QBoardView::persistentZoom( qreal r)
{
    // this->zoom( (this->m_pzoom = r) ); // doing this alone causes scrollbars to disappear in some cases?
    this->m_pzoom = r;
    this->updateBoardPixmap(); // kludge to work around missing scrollbars

}

void QBoardView::setHandDragMode(bool handMode )
{
    this->setDragMode( handMode
		       ? QGraphicsView::ScrollHandDrag
		       : QGraphicsView::RubberBandDrag);
}

QSize QBoardView::sizeHint() const
{
    QSizeF sz;
    if( ! this->m_b.pixmap().isNull() )
    {
	sz = this->m_b.pixmap().size();
	if( this->m_pzoom > 0.0 )
	{
	    sz.setHeight( sz.height() * m_pzoom );
	    sz.setWidth( sz.width() * m_pzoom );
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
    //this->setBackgroundBrush(m_b.pixmap());
    if( m_b.pixmap().isNull() ) return;
    QSize isz = m_b.pixmap().size();
    if( this->m_pzoom <= 0.0 )
    {
	this->resize(isz);
    }
    this->resetTransform();
    this->resetMatrix();
    QRectF rect( 0, 0, isz.width(),  isz.height() );
    this->setSceneRect( rect );
    //this->setBackgroundBrush(m_b.pixmap());
    if( this->m_pzoom > 0.0 )
    {
	this->zoom( m_pzoom );
    }
    else
    {
	this->updateGeometry();
    }
}

void QBoardView::drawBackground( QPainter *p, const QRectF & rect )
{
    if( this->m_b.pixmap().isNull() )
    {
	this->QGraphicsView::drawBackground(p,rect);
	return;
    }
    p->fillRect( rect, this->backgroundBrush() );
    p->drawPixmap(rect,m_b.pixmap(),rect);
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
    this->zoom( m_scale + BoardZoomScaleFactor );
}


void QBoardView::zoomOut()
{
    this->zoom( m_scale - BoardZoomScaleFactor );
}

void QBoardView::zoom( qreal z )
{
    // i don't get why this is so much work. The examples in the qt docs
    // make it look much simpler.
    if( (z < 0.10) || (z>4.01) ) return; // arbitrary! 
    if( z == m_scale ) return;
    m_scale = z;
    QSizeF isz(m_b.pixmap().size());
    if( 1.0 != z )
    {
	isz.scale( std::ceil( isz.width() * m_scale ),
		   std::ceil( isz.height() * m_scale ),
		   Qt::IgnoreAspectRatio );
    }
    //these resets reset our pos to (0,0) and undoes rotation, which is annoying. But we need one of them. :/
    this->resetTransform(); 
    //this->resetMatrix();
    this->resize(isz.toSize());
    this->scale( m_scale, m_scale );
    this->updateGeometry(); // without this, scrollbars get out of sync.
}

void QBoardView::zoomReset()
{
    this->zoom(1.0);
}

void QBoardView::mousePressEvent( QMouseEvent * event )
{
    if (QGraphicsItem *item = itemAt(event->pos())) {
	qDebug() << "QBoardView::mousePressEvent(): You clicked on item" << item;
    } else {
	qDebug() << "QBoardView::mousePressEvent(): You didn't click on an item.";
    }
    if( event->button() & Qt::MidButton )
    { // center view on clicked pos.
	event->accept();
	this->centerOn( this->mapToScene(event->pos()) );
	return;
    }
    if( event->button() & Qt::RightButton )
    {
	event->ignore();
	return;
    }
    this->QGraphicsView::mousePressEvent(event);
}

QBoard & QBoardView::board()
{
    return m_b;
}

void QBoardView::dragMoveEvent( QDragMoveEvent * ev )
{
    ev->ignore();
    qDebug() << "QBoardView::dragMoveEvent() pos =="<<ev->pos();
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
    //     this->QGraphicsView::contextMenuEvent(event);
    //     return;
    // i can't believe this is the only way to know if the menu event
    // needs to be passed to an item!
    // FUCK! It propagates to all stacked children no matter
    // what i do!!!!
    QGraphicsItem * it = this->itemAt( event->pos() );
    if( ! it )
    {
	MenuHandlerBoard mh;
	mh.doMenu( this, event );
    }
    else
    {
	event->accept();
	this->QGraphicsView::contextMenuEvent(event);
    }
}
