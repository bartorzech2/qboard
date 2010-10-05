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

#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QKeySequence>
#include <QFont>
#include <QtGlobal>

#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QList.h>
#include <qboard/utility.h>
#include <qboard/MenuHandlerGeneric.h>
#include <qboard/S11nQt/QPointF.h>

#include <qboard/Dice.h>
#include <time.h>

namespace qboard {

struct Die::Impl
{
    int sides;
    int low;
    static bool seeded;
    Impl() : sides(6),
	     low(1)
    {
	if( ! seeded )
	{
	    seeded = true;
	    qsrand( time(NULL) );
	}
    }
    ~Impl()
    {
    }
};
bool Die::Impl::seeded = false;

Die::Die(int sides, int lowDigit)
    : impl(new Impl)
{
    impl->sides = sides;
    impl->low = lowDigit;
}

Die::~Die()
{
    delete impl;
}

int Die::roll()
{
    return ( impl->low + ( qrand() % ((unsigned long) (impl->sides - impl->low)+1) ) );
}

void Die::sides( int i )
{
    impl->sides = i;
}
int Die::sides() const
{
    return impl->sides;
}
int Die::low() const
{
    return impl->low;
}
void Die::low( int i )
{
    impl->low = i;
}


bool Die_s11n::operator()( S11nNode & dest, Die const & src ) const
{
    typedef S11nNodeTraits NT;
    NT::set( dest, "sides", src.sides() );
    NT::set( dest, "low", src.low() );
    return true;
}
bool Die_s11n::operator()( S11nNode const & src, Die & dest ) const
{
    typedef S11nNodeTraits NT;
    dest.sides( NT::get( src, "sides", dest.sides() ) );
    dest.low( NT::get( src, "low", dest.low() ) );
    return true;
}


struct QGIDie::Impl
{
    bool active;
    QColor color;
    Die die;
    int rollCount;
    int result;
//     typedef QList<int> IntList;
//     IntList list;
    Impl() : active(false),
	     color(Qt::yellow),
	     die(),
	     rollCount(0),
	     result(0)
	{
		
	}
	~Impl()
	{
		
	}
};

QGIDie::QGIDie() :
    QGraphicsTextItem(),
    Serializable("QGIDie"),
    impl(new Impl)
{
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
#if QT_VERSION >= 0x040400
	this->setCacheMode(DeviceCoordinateCache);
#endif
	this->setProperty("color",QColor(Qt::yellow)); // forces brush to get updated
	this->setHtml("<body bgcolor='#ffffff'>Die roller.<br/>Double-click.</body>");
}

QGIDie::~QGIDie()
{
    QBOARD_VERBOSE_DTOR << "~QGIDie()";
    delete impl;
}

void QGIDie::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	impl->active = true;
	if( (ev->buttons() & Qt::LeftButton) )
	{
	    ev->accept();
	    qreal zV = qboard::nextZLevel(this);
	    if( zV > this->zValue() )
	    {
		this->setZValue(zV);
	    }
	}
	this->QGraphicsTextItem::mousePressEvent(ev);
}

void QGIDie::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	impl->active = false;
	this->QGraphicsTextItem::mouseReleaseEvent(event);
	event->accept();
}

QRectF QGIDie::boundingRect() const
{
#if 1
    return this->QGraphicsTextItem::boundingRect();
#else
	const qreal adjust = 2;
	const qreal r = impl->radius;
	return QRectF(-r - adjust, -r - adjust,2*r + adjust, 2*r + adjust);
#endif
}

QPainterPath QGIDie::shape() const
{
    return this->QGraphicsTextItem::shape();
}

void QGIDie::updateText()
{
    QString html = //QString("<html><body bgcolor='%1'>Roll #%2: %3</body></html>").
	QString("Roll #%1: %2").
	//arg(impl->color.name()).
	arg(impl->rollCount).
	arg(impl->result);
    this->setPlainText( html );
}

void QGIDie::propertySet( char const *pname, QVariant const & var )
{
    // FIXME: treat QVariant::Invalid appropriately for each property
    if(0) qDebug() << "QGIDie::propertySet("<<pname<<") val="<<var;
    std::string key( pname );
    if( "zLevel" == key )
    {
	this->setZValue(var.toDouble());
	return;
    }
    if( "pos" == key )
    {
	this->setPos( var.value<QPointF>() );
	return;
    }
    if( "sides" == key )
    {
	impl->die.sides(var.toInt());
	this->updateText();
	return;
    }
    if( "low" == key )
    {
	impl->die.low(var.toInt());
	this->updateText();
	return;
    }
    if( "color" == key )
    {
	QColor old( impl->color );
	impl->color = var.value<QColor>();
	if( 255 == impl->color.alpha() )
	{
	    impl->color.setAlpha(old.alpha());
	}
	this->updateText();
	return;
    }
    if( "scale" == key )
    {
	qreal scale = var.canConvert<qreal>() ? var.value<qreal>() : 1.0;
	if( 0 == scale ) scale = 1.0;
	qboard::rotateAndScale( this, 0, scale, scale, true);
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
}


int QGIDie::roll()
{
    ++impl->rollCount;
    impl->result = impl->die.roll();
    this->updateText();
    return impl->result;
}

void QGIDie::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{
    this->QGraphicsTextItem::mouseDoubleClickEvent(event);
#if 1
    /**
       this causes errors under Qt 4.4:

QPixmap: Invalid pixmap parameters
QPainter::begin: Cannot paint on a null pixmap
QPainter::renderHints: Painter must be active to set rendering hints
QPainter::setRenderHint: Painter must be active to set rendering hints
QPainter::setRenderHint: Painter must be active to set rendering hints
QPainter::setWorldTransform: Painter not active
QPainter::setWorldTransform: Painter not active
QPainter::save: Painter not active
QPainter::setWorldTransform: Painter not active
QPainter::save: Painter not active
QPainter::save: Painter not active
QPainter::restore: Unbalanced save/restore
QPainter::restore: Unbalanced save/restore
QPainter::restore: Unbalanced save/restore
QPainter::end: Painter not active, aborted
X Error: BadValue (integer parameter out of range for operation) 2
  Major opcode: 53 (X_CreatePixmap)
  Resource id:  0x0
X Error: BadDrawable (invalid Pixmap or Window parameter) 9
  Major opcode: 55 (X_CreateGC)
  Resource id:  0x1800941
X Error: BadGC (invalid GC parameter) 13
  Major opcode: 60 (X_FreeGC)
  Resource id:  0x1800942
X Error: BadDrawable (invalid Pixmap or Window parameter) 9
  Extension:    156 (RENDER)
  Minor opcode: 4 (RenderCreatePicture)
  Resource id:  0x1800941
X Error: RenderBadPicture (invalid Picture parameter) 179
  Extension:    156 (RENDER)
  Minor opcode: 7 (RenderFreePicture)
  Resource id:  0x1800943
X Error: BadPixmap (invalid Pixmap parameter) 4
  Major opcode: 54 (X_FreePixmap)
  Resource id:  0x1800941
    */
    event->accept();
    this->roll();
#endif
    return;
}
bool QGIDie::event( QEvent * e )
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
    return QGraphicsTextItem::event(e);
}
QVariant QGIDie::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
		case ItemPositionHasChanged:
			//foreach (QGILineBinder *edge, m_ed)	edge->adjust();
// 			if( m_line )
// 			{
// 				m_line->adjust();
// 				this->update();
// 			}
		break;
	default:
		break;
	};
	return QGraphicsTextItem::itemChange(change, value);
}
void QGIDie::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *wid)
{
    this->QGraphicsTextItem::paint(painter,opt,wid);
}

void QGIDie::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	event->accept();
	MenuHandlerDie mh;
	impl->active = false;
	mh.doMenu( this, event );
}


bool QGIDie::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	if( this->metaObject()->propertyCount() )
	{
		S11nNode & pr( s11n::create_child( dest, "properties" ) );
		if( ! s11n::qt::QObjectProperties_s11n()( pr, *this ) ) return false;
	}
	return s11n::serialize_subnode( dest, "die", impl->die )
	    && s11n::serialize_subnode( dest, "pos", this->pos() );
}
bool QGIDie::deserialize(  S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	typedef S11nNodeTraits NT;
	if( ! s11n::deserialize_subnode( src, "die", impl->die ) ) return false;
	QPointF p;
	if( ! s11n::deserialize_subnode( src, "pos", p ) ) return false;
	this->setPos( p );
	S11nNode const * ch = s11n::find_child_by_name(src, "properties");
	return ch
		? s11n::qt::QObjectProperties_s11n()( *ch, *this )
		: true;
}

struct MenuHandlerDie::Impl
{
    QGIDie * dot;
    Impl() : dot(0)
    {}
    ~Impl()
    {
    }
};
MenuHandlerDie::MenuHandlerDie() : QObject(), impl(new Impl)
{
	
}
MenuHandlerDie::~MenuHandlerDie()
{
	delete impl;
}

void MenuHandlerDie::showHelp()
{
    qboard::showHelpResource("Lines", ":/QBoard/help/classes/QGIDie.html");
}

void MenuHandlerDie::doMenu( QGIDie *gvi, QGraphicsSceneContextMenuEvent * ev )
{
    if( ! gvi ) return;
    impl->dot = gvi;
    ev->accept();

    typedef QList<QObject *> OL;
    OL selected;
    if(  gvi->isSelected() )
    {
	selected = qboard::selectedItemsCast<QObject>( gvi->scene() );
    }
    else
    {
	selected.push_back( dynamic_cast<QObject*>(gvi) );
    }

    QString label("Die...");
    MenuHandlerCommon proxy;
    QMenu * m = proxy.createMenu( gvi );
    QObjectPropertyMenu * mColor = QObjectPropertyMenu::makeColorMenu(selected, "color", "colorAlpha" );
    mColor->setIcon(QIcon(":/QBoard/icon/color_fill.png"));
    m->addMenu(mColor);

    if(1)
    {
	m->addMenu( QObjectPropertyMenu::makeNumberListMenu(
				    "Radius",
				    selected,
				    "radius",
				    8,37,8) );
    }
    if(1)
    {
	QObjectPropertyMenu * pm =
	    QObjectPropertyMenu::makeNumberListMenu("Scale",
						    selected, "scale",
						    0.5, 3.01, 0.5);
	pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	m->addMenu(pm);
    }

    m->addSeparator();
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( gvi, m );
    clipper->addDefaultEntries( m, true, gvi->isSelected() );
    m->addSeparator();
    m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
    m->exec( ev->screenPos() );
    delete m;
}


} // namespace
