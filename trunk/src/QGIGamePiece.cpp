#include "QGIGamePiece.h"
#include "GamePiece.h"
#include "utility.h"
#include <QDebug>
#include <QFont>
#include "MenuHandlerPiece.h"
#include "utility.h"

struct QGIGamePiece::Impl
{
	QColor backgroundColor;
	QColor borderColor;
	int borderSize;
	QPixmap pixmap;
	int borderLineStyle;
	int alpha;
	int borderAlpha;
	/**
		When block is true, QGIGamePiece will reject piece property updates.
		This is a kludge to allow QGIGamePiece to properly update the piece
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
QGIGamePiece::QGIGamePiece(GamePiece * pc, QGraphicsScene * sc )
    : QObject(),
      QGraphicsPixmapItem(),
      Serializable("QGIGamePiece"),
      m_pc(0),
      impl(new QGIGamePiece::Impl)
{
    this->setup();
    this->setPiece(pc);
    if( sc ) sc->addItem( this );
}

QGIGamePiece::QGIGamePiece()
    : QObject(),
      QGraphicsPixmapItem(),
      Serializable("QGIGamePiece"),
      m_pc(0),
      impl( new QGIGamePiece::Impl )
{
    this->setup();
}

void QGIGamePiece::setup()
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );
}

QGIGamePiece::~QGIGamePiece()
{
	QBOARD_VERBOSE_DTOR << "~QGIGamePiece()";
#if 1
	if( m_pc )
	{
		//GamePiece * pc = m_pc;
		this->setPiece(0);
		//pc->deleteLater();
		//delete pc;
	}
#endif
	delete this->impl;
}
void QGIGamePiece::updatePiecePos(bool onlyIfNotSet)
{
	if( ! m_pc ) return;
	QVariant var = m_pc->property("pos");
	if( onlyIfNotSet )
	{
	    if( var.isValid() )
	    {
		this->setPos( var.toPointF() );
	    }
	}
	m_pc->setPos( this->pos().toPoint() );
#if 0
	qDebug() << "QGIGamePiece::updatePiecePos("
		 <<onlyIfNotSet<<") ="<<this->pos().toPoint()
		 << "var="<<var;
#endif
	//m_pc->setPos(this->pos().toPoint());
}

void QGIGamePiece::connect( GamePiece * gp )
{
	if( !gp || (gp == this->m_pc) ) return;
	if( this->m_pc ) m_pc->removeViewRef();
	this->m_pc = gp;
	m_pc->addViewRef();
	this->updatePiecePos(true); // lame kludge
	this->QObject::connect(gp,SIGNAL(destructing(GamePiece *)),SLOT(pieceDestroyed(GamePiece*)));
	this->QObject::connect(gp,SIGNAL(piecePropertySet(char const *, GamePiece *)),
		SLOT(piecePropertySet(char const *)));
}
void QGIGamePiece::disconnect( GamePiece * gp )
{
	if( (!gp) || (m_pc != gp) ) return;
	this->m_pc = 0;
	QObject::disconnect(gp,SIGNAL(destructing(GamePiece *)),
		this,SLOT(pieceDestroyed(GamePiece*)));
	QObject::disconnect(gp,SIGNAL(piecePropertySet(char const *, GamePiece *)),
		this, SLOT(piecePropertySet(char const *)));
	gp->removeViewRef();
}
void QGIGamePiece::pieceDestroyed( GamePiece * pc )
{
	//qDebug() << "QGIGamePiece::pieceDestroyed()"<<pc;
	if( pc && (pc == m_pc) )
	{
		//qDebug() << "QGIGamePiece::pieceDestroyed() disconnecting"<<pc;
		this->disconnect(pc);
		this->hide();
		this->deleteLater();
	}
}
void QGIGamePiece::syncProperties()
{
	if( ! this->m_pc ) return;
	typedef QList<QByteArray> QL;
	QL ql( this->m_pc->dynamicPropertyNames() );
	QL::const_iterator it( ql.begin() );
	QL::const_iterator et( ql.end() );
	for( ; et != it; ++it )
	{
		this->piecePropertySet( it->constData() );
	}
}

GamePiece * QGIGamePiece::piece()
{
	return this->m_pc;
}
void QGIGamePiece::setPiece( GamePiece * pc )
{
	if( m_pc == pc ) return;
	if( this->m_pc )
	{
		this->disconnect(m_pc);
	}
	if( pc )
	{
		this->connect(pc);
		this->syncProperties();
	}
}

bool QGIGamePiece::event( QEvent * e )
{
	if( e->type() == QEvent::DynamicPropertyChange )
	{
		e->accept();
		this->update();
		return true;
	}
	return QObject::event(e);
}

void QGIGamePiece::piecePropertySet( char const *pname )
{
	QVariant var( m_pc->property(pname) );
	qDebug() << "GamePiece::piecePropertySet("<<pname<<")[block="<<impl->block<<"] val="<<var;
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
		this->setPos( var.toPoint() );
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
	if( "angle" == key )
	{
		qreal ang( var.toDouble() );
		QRectF rec(this->boundingRect());
		qreal x = rec.width()/2;
		qreal y = rec.height()/2;
		setTransform(QTransform().translate(x, y).rotate(ang).translate(-x, -y));
		return;
	}
	if( "pixmap" == key )
	{
		QPixmap pix;
		QString fname( var.toString() );
		if( pix.load( fname ) && m_pc )
		{
			// FIXME: this can theoretically ping-png forever if
			// two views are watching the same piece, as they only
			// block their own updates. We can't just call
			// m_pc->setProperty() and avoid a signal fire b/c
			// arbitrary listeners would then miss this update.
			impl->block = true;
			m_pc->setPieceProperty("size",pix.size());
			impl->block = false;
		}
		else
		{
			pix = QPixmap(300,40);
			QPainter pain(&pix);
			pain.fillRect( pix.rect(), Qt::yellow );
			QFont font( "courier", 9 );
			pain.setFont( font ); 
			pain.setPen( QPen(Qt::red) );
			pain.drawText(5,font.pointSize()+6,"Error loading pixmap:");
			pain.drawText(5,pix.height()-6,fname);
		}
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
	}
}
#include <QGraphicsItem>
void QGIGamePiece::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    if( (!m_pc) ) //  || (!(ev->buttons() & Qt::LeftButton)) )
	{
		this->QGraphicsPixmapItem::mousePressEvent(ev);
		return;
	}
	else if( ev->buttons() & Qt::RightButton )
	{
	    // Weird: if we don't do this, the QGraphicsView
	    // doesn't know how to keep us selected (if we're selected)
	    // and also has trouble knowing whether to show a menu
	    // or not.
	    // qDebug() <<"QGIGamePiece::mousePressEvent() RMB:"<<ev;
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
			if( m_pc )
			{
				m_pc->setPieceProperty("zLevel",zV);
			}
			else
			{
				this->setZValue(zV);
			}
		}
	}
	this->QGraphicsPixmapItem::mousePressEvent(ev);
}

void QGIGamePiece::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev )
{
	this->QGraphicsPixmapItem::mouseDoubleClickEvent(ev);
}
void QGIGamePiece::mouseMoveEvent( QGraphicsSceneMouseEvent * ev )
{
	this->QGraphicsPixmapItem::mouseMoveEvent(ev);
}
void QGIGamePiece::mouseReleaseEvent( QGraphicsSceneMouseEvent * ev )
{
	this->QGraphicsPixmapItem::mouseReleaseEvent(ev);
	// Update GamePiece position the hard way.
	// i hate doing this here, but it's a decent workaround for the time begin.
	// We could use changeEvent(), but that causes a HUGE number of updates
	// to m_pc.
	if( m_pc )
	{
		this->updatePiecePos(false);
	}
}

#include <QGraphicsSceneContextMenuEvent>

void QGIGamePiece::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	event->accept();
	MenuHandlerPiece mh;
	mh.doMenu( this, event );
	//this->QGraphicsPixmapItem::contextMenuEvent(event);
}
void QGIGamePiece::destroyWithPiece()
{
	if( this->m_pc )
	{
		this->m_pc->deleteLater();
	}
	else
	{
		this->deleteLater();
	}
}
QRectF QGIGamePiece::boundingRect() const
{
	return QRectF( impl->pixmap.isNull()
		? (m_pc ? m_pc->geom() : QRect(0,0,40,40))
		: impl->pixmap.rect() );
}

QVariant QGIGamePiece::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
#if 0
	if( change == ItemPositionHasChanged )
	{
		if( !impl->block && m_pc )
		{
			impl->block = true;
			// this causes WAY too many events to fire:
			//m_pc->setPieceProperty("pos",value);
			m_pc->setProperty("pos",value);
			impl->block = false;
			// ^^^ We have to be careful here to avoid that we 
			// end up moving this object again during this handler, which the Qt docs say
			// not to do.
			// For the basic use case, setProperty() is fine, but if there are other objects
			// watching m_pc (e.g. an editor widget) then the pos property change won't be
			// visible, so we need setPieceProperty(). Thus the impl->block kludge to avoid
			// a potential event loop.
		}
	}
	return value;
#else
	return QGraphicsPixmapItem::itemChange(change,value);
#endif
}
void QGIGamePiece::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
#if 0
	this->QGraphicsPixmapItem::paint(painter,option,widget);
	return;
#endif
	if( ! this->m_pc )
	{
		this->QGraphicsPixmapItem::paint(painter,option,widget);
		return;
	}

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
void QGIGamePiece::dragMoveEvent( QGraphicsSceneDragDropEvent * event )
{
    this->ensureVisible();
    qDebug() << "QGIGamePiece::dragMoveEvent() pos="<<event->pos();
    this->QGraphicsPixmapItem::dragMoveEvent(event);
}


bool QGIGamePiece::serialize( S11nNode & dest ) const
{
    if( (!m_pc) || ! this->Serializable::serialize( dest ) ) return false;
    return s11nlite::serialize_subnode<GamePiece>( dest, "piece", *m_pc );
}
bool QGIGamePiece::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    if( ! m_pc ) this->setPiece( new GamePiece );
    return s11nlite::deserialize_subnode<GamePiece>( src, "piece", *m_pc );
}
