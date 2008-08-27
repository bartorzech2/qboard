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

#include "MenuHandlerGeneric.h"

#include <s11n.net/s11n/serialize.hpp>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "utility.h"
#include "S11nClipboard.h"
#include "Serializable.h"
#include "S11nQtList.h"
MenuHandlerCopyCut::MenuHandlerCopyCut(QGraphicsItem * gi,QObject * parent)
    : QObject(parent),
      m_gi(gi)
{
}
MenuHandlerCopyCut::~MenuHandlerCopyCut()
{
	
}

#include "utility.h"
void MenuHandlerCopyCut::clipboard( QGraphicsItem * gvi, bool copy )
{
    if( ! gvi ) return;
    if( gvi->isSelected() )
    {
	qboard::clipboardScene( gvi->scene(), copy,
				qboard::calculateCenter(gvi).toPoint() );
	return;
    }
    else
    {
	//if( gvi->parentItem() ) return;
	Serializable * ser = dynamic_cast<Serializable*>( gvi );
	if( ! ser ) return;
	S11nClipboard::instance().serialize( *ser );
	if( ! copy )
	{
	    QObject * obj = dynamic_cast<QObject *>( ser );
	    if( obj )
	    {
		obj->deleteLater();
	    }
	    else
	    {
		delete gvi;
	    }
	}
    }
}

void MenuHandlerCopyCut::clipboardCopy()
{
    this->clipboard( m_gi, true );
}
void MenuHandlerCopyCut::clipboardCut()
{
    this->clipboard( m_gi, false );
}

void MenuHandlerCopyCut::clipOne( QGraphicsItem * gi, bool copy )
{
    Serializable * ser = dynamic_cast<Serializable*>(gi);
    if( ! ser ) return;
    try
    {
	S11nClipboard::instance().serialize( *ser );
    }
    catch(...)
    {
    }
    if( ! copy )
    {
	QObject * o = dynamic_cast<QObject*>(gi);
	if( o )
	{
	    o->deleteLater();
	}
	else
	{
	    delete gi;
	}
    }
}

void MenuHandlerCopyCut::copyOne()
{
    return clipOne( m_gi, true );
}
void MenuHandlerCopyCut::cutOne()
{
    return clipOne( m_gi, false );
}

void MenuHandlerCopyCut::clipList( QGraphicsItem * gi, bool copy )
{
    if( ! gi ) return;
    typedef QList<Serializable*> QSL;
    QSL sel( qboard::selectedItemsCast<Serializable>( gi->scene() ) );
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

void MenuHandlerCopyCut::copyList()
{
    clipList( m_gi, true );
}

void MenuHandlerCopyCut::cutList()
{
    clipList( m_gi, false );
}

void MenuHandlerCopyCut::cloneItem()
{
    if( ! m_gi ) return;
    QGraphicsScene * sc = m_gi->scene();
    if( ! sc ) return; // shouldn't happen
    typedef QList<QGraphicsItem*> QGIL;
    typedef QPair<Serializable *,QGraphicsItem *> Pair;
    typedef QList<Pair> SL;
    SL sl;
    typedef QList<QGraphicsItem *> QIL;
    QIL gl;
    bool setSelection = m_gi->isSelected();
    if( setSelection )
    {
	gl = sc->selectedItems();
    }
    else
    {
	gl.push_back(m_gi);
    }
    //sl = qboard::graphicsItemsCast<Serializable>( gl );
    foreach( QGraphicsItem * gi, gl )
    {
	Serializable * ser = dynamic_cast<Serializable*>( gi );
	if( ! ser ) continue;
	sl.push_back( Pair(ser, gi) );
    }
    const int offset = 5;
    if( setSelection )
    {
	sc->clearSelection();
    }
    foreach( Pair p, sl )
    {
	Serializable * s = p.first->clone();
	if( ! s )
	{
	    qDebug() << "MenuHandlerCopyCut::cloneItem(): cloning of Serializable failed!";
	    continue;
	}
	QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>(s);
	if( ! gi ) // not possible, i think, but why not...
	{
	    s11n::cleanup_serializable( s );
	    continue;
	}
	QPointF pos( p.second->pos() + QPointF(offset,offset));
	gi->setPos( pos );
	gi->setZValue( gi->zValue() + 0.001 );
	gi->setParentItem( p.second->parentItem() );
	sc->addItem( gi );
	if( setSelection )
	{
	    gi->setSelected( true );
	}
    }
}

QMenu * MenuHandlerCopyCut::addDefaultEntries( QMenu * m, bool cut, bool list )
{
    if( ! m_gi ) return 0;
    if( ! dynamic_cast<Serializable *>( m_gi ) ) return 0;
    QString const icoCopy(":/QBoard/icon/editcopy.png");
    QString const icoCut(":/QBoard/icon/editcut.png");
    m->addAction(QIcon(icoCopy),
		 "Clone",this,SLOT(cloneItem()) );
    if( list && m_gi->isSelected() )
    {
	m->addAction(QIcon(icoCopy),
		     "Copy selected",this,SLOT(clipboardCopy()) );
	if( cut ) m->addAction(QIcon(icoCut),
			       "Cut selected",this,SLOT(clipboardCut()) );

    }
    else
    {
	m->addAction(QIcon(icoCopy),
		     "Copy",this,SLOT(copyOne()) );
	if( cut ) m->addAction(QIcon(icoCut),
			       "Cut",this,SLOT(cutOne()) );
    }
    QMenu * mextra = 0;
    if( list && m_gi->isSelected() )
    {
	mextra = m->addMenu(tr("Clipboard..."));
	mextra->addAction(QIcon(icoCopy),
			  "Copy selected as QList<Serializable*>",
			  this,SLOT(copyList()) );
	if( cut ) mextra->addAction(QIcon(icoCut),
				    "Cut selected as QList<Serializable*>",
				    this,SLOT(cutList()) );
    }
    return mextra;
}

SceneSelectionDestroyer::SceneSelectionDestroyer( QObject * parent, QGraphicsItem * gi ) :
    QObject(parent),
    gitem(gi)
{
}
SceneSelectionDestroyer::~SceneSelectionDestroyer()
{
}
void SceneSelectionDestroyer::destroySelectedItems()
{
    if( ! this->gitem ) return;
    qboard::destroyToplevelItems( this->gitem, true );
}
void SceneSelectionDestroyer::destroyItem(QGraphicsItem * item)
{
    if( item == this->gitem ) gitem = 0;
    if( ! item->parentItem() )
    {
	qboard::destroyToplevelItems( item, false );
    }
    else
    {
	QObject * o = dynamic_cast<QObject*>(item);
	if( o ) o->deleteLater();
	else delete o;
    }
}
void SceneSelectionDestroyer::destroyItem()
{
    if( ! this->gitem ) return;
    if( this->gitem->isSelected() )
    {
	this->destroySelectedItems();
    }
    else
    {
	this->destroyItem( this->gitem );
    }
}
QMenu * MenuHandlerCommon::createMenu( QGraphicsItem *gi ) //, QGraphicsSceneContextMenuEvent *ev )
{
    QString label;
    QString ico;
    if( gi->isSelected() )
    {
	label = tr("Selected items...");
	ico = ":/QBoard/icon/multiple.png";
    }
    else
    {
	label = tr("Item...");
    }
    QMenu * menu = new QMenu(label);
    QAction * act = menu->addAction(label);

    if( !ico.isEmpty() )
    {
	act->setIcon(QIcon(ico));
	act->setEnabled(false);
    }
    else
    {
#if 1
	QRectF bounds = gi->boundingRect();
	QPixmap tgt(bounds.size().toSize());
	tgt.fill(QColor(Qt::transparent));
	QPainter pain(&tgt);
	QStyleOptionGraphicsItem bogus;
	bogus.exposedRect = bounds;
	gi->paint( &pain, &bogus, 0 );
	act->setIcon( QIcon( tgt.scaled(16,16) ) );
#endif
    }
    SceneSelectionDestroyer * destroyer = new SceneSelectionDestroyer( menu, gi );
    menu->addAction(QIcon(":/QBoard/icon/button_cancel.png"),"Destroy",destroyer,SLOT(destroyItem()) );
    menu->addSeparator();
    return menu;
}

struct QObjectPropertyAction::Impl
{
    typedef QList<QObject *> ListType;
    ListType list;
    QString key;
    QVariant val;
    Impl() :
	list(),
	key(),
	val()
    {
    }
    ~Impl()
    {}
};

QObjectPropertyAction::QObjectPropertyAction(
					     QObject * obj,
					     QString const & propName,
					     QVariant const & val,
					     QObject * parent )
    : QAction(val.toString(),parent),
      impl(new Impl)
{
    impl->list.push_back(obj);
    impl->key = propName;
    impl->val = val;
    this->setup();
}
QObjectPropertyAction::QObjectPropertyAction(
					     QList<QObject *> objs,
					     QString const & propName,
					     QVariant const & val,
					     QObject * parent )
    : QAction(val.toString(),parent),
      impl(new Impl)
{
    impl->list = objs;
    impl->key = propName;
    impl->val = val;
    this->setup();
}
QObjectPropertyAction::~QObjectPropertyAction()
{
    delete this->impl;
}

void QObjectPropertyAction::setup()
{
#if 1
    if( 1 == impl->list.count() )
    {
	QObject * obj = *(impl->list.begin());
	if( impl->val == obj->property(impl->key.toAscii()) )
	{
	    this->setCheckable(true);
	    this->setChecked(true);
	}
    }
#endif

    if( impl->val.type() == QVariant::Color )
    {
	QPixmap px(16,16);
	px.fill(impl->val.value<QColor>());
	this->setIcon(QIcon(px));
    }
#if 0
    // For our current use cases, this is overkill...
    for( Impl::ListType::iterator it = impl->list.begin();
	 impl->list.end() != it; ++it )
    {
	connect( *it, SIGNAL(destroyed(QObject*)), this, SLOT(dtorDisconnect(QObject*)) );
    }
#endif
    connect( this, SIGNAL(triggered()), this, SLOT(setProperty()) );
}


void QObjectPropertyAction::dtorDisconnect(QObject * obj)
{
    impl->list.removeAll(obj);
}


void QObjectPropertyAction::setProperty()
{
    for( Impl::ListType::iterator it = impl->list.begin();
	 impl->list.end() != it; ++it )
    {
	(*it)->setProperty( impl->key.toAscii(), impl->val );
    }
}



struct QObjectPropertyMenu::Impl
{
    typedef QList<QObject *> ListType;
    ListType list;
    QString key;
    Impl() :
	list(),
	key()
    {
    }
    ~Impl()
    {}
};
QObjectPropertyMenu::QObjectPropertyMenu( const QString & title,
					  QObject * obj,
					  QString const & propName,
					  QWidget * parent ) :
    QMenu(title, parent),
    impl(new Impl)
{
    impl->list.push_back(obj);
    impl->key = propName;
}
QObjectPropertyMenu::QObjectPropertyMenu( const QString & title,
					  QList<QObject *> list,
					  QString const & propName,
					  QWidget * parent ) :
    QMenu(title, parent),
    impl(new Impl)
{
    impl->list = list;
    impl->key = propName;
}
QObjectPropertyMenu::~QObjectPropertyMenu()
{
    delete impl;
}

QAction * QObjectPropertyMenu::addItem( QVariant const & val, QString const & lbl)
{
    QObjectPropertyAction * ac = new QObjectPropertyAction( impl->list, impl->key, val, this );
    if( ! lbl.isEmpty() )
    {
	ac->setText(lbl);
    }
    this->addAction(ac);
    return ac;
}


QObjectPropertyMenu * QObjectPropertyMenu::makePenStyleMenu(
			   QList<QObject *> objs,
			   char const * propertyName )
{
    QObjectPropertyMenu * pm = new QObjectPropertyMenu("Style", objs, propertyName, 0 );
    for( int i = Qt::NoPen; i < Qt::CustomDashLine; ++i )
    {
	pm->addItem( QVariant(i), qboard::penStyleToString(i) );
    }
    return pm;
}

QObjectPropertyMenu * QObjectPropertyMenu::makePenStyleMenu( QObject * pv, char const * propertyName )
{
    QList<QObject*> li;
    li.push_back( pv );
    return makePenStyleMenu( li, propertyName );
}

QObjectPropertyMenu * QObjectPropertyMenu::makeNumberListMenu(
				      char const * lbl, 
				      QList<QObject *> objs,
				      char const * propertyName,
				      qreal from,
				      qreal to,
				      qreal step )
{
    QObjectPropertyMenu * pm = new QObjectPropertyMenu(lbl, objs, propertyName, 0 );
    for( qreal i = from; i < to; i += step )
    {
	pm->addItem( QVariant(i) );
    }
    return pm;
}

QObjectPropertyMenu * QObjectPropertyMenu::makeNumberListMenu(
				   char const * lbl, 
				   QObject * pv,
				   char const * propertyName,
				   qreal from,
				   qreal to,
				   qreal step )
{
    QList<QObject*> foo;
    foo.push_back(pv);
    return makeNumberListMenu(lbl,foo,propertyName,from,to,step);
}

QObjectPropertyMenu * QObjectPropertyMenu::makeAlphaMenu(
				QList<QObject *> objs,
				char const * propertyName,
				QColor const & hint )
{
    QObjectPropertyMenu * pm = new QObjectPropertyMenu("Transparency", objs, propertyName, 0 );
    const short opaque = 255;
    const qreal step = opaque/10.0;
    int pct = 100;
    for( qreal i = opaque; i >= 0; i -= step, pct -= 10)
    {
	QString lbl;
	if( i == 0 )
	{
	    lbl = QString("Transparent");
	}
	else if( i==opaque )
	{
	    i = opaque;
	    lbl = QString("Opaque");
	}
	else
	{
	    lbl = QString("%1%").arg(pct);
	}
	QAction * ac = pm->addItem( QVariant(i), lbl );
	if( hint.isValid() )
	{
	    QColor c( hint );
	    c.setAlpha(int(i));
	    QPixmap px(16,16);
	    px.fill(c);
	    ac->setIcon(px);
	}
    }
    return pm;
}

QObjectPropertyMenu *
QObjectPropertyMenu::makeAlphaMenu(
				   QObject * obj,
				   char const * propertyName,
				   QColor const & hint )
{
    QList<QObject*> li;
    li.push_back(obj);
    return makeAlphaMenu( li, propertyName, hint );
}

QObjectPropertyMenu * QObjectPropertyMenu::makeColorMenu(
					 QList<QObject*> list,
					 char const * propertyName,
					 char const * alphaName )
{
    if( list.isEmpty() ) return 0;
    QObjectPropertyMenu * mc = new QObjectPropertyMenu("Color",list,propertyName,0);
    mc->setIcon(QIcon(":/QBoard/icon/colorize.png"));
    if( alphaName )
    {
	QObject * pv = *(list.begin());
	QColor hint( pv->property(propertyName).value<QColor>() );
	mc->addMenu( QObjectPropertyMenu::makeAlphaMenu(list,alphaName, hint) );
    }
    typedef QList<QColor> QL; 
    QL colors( qboard::colorList() ); 
    QL::iterator it = colors.begin();
    for( ; colors.end() != it; ++it )
    {
	QVariant var( *it );
	QString name( (*it).name() );
	mc->addItem( var, name  );
    }
    return mc;
}

QObjectPropertyMenu * QObjectPropertyMenu::makeColorMenu(
					 QObject * pv,
					 char const * propertyName,
					 char const * alphaName )
{
    QList<QObject*> li;
    li.push_back(pv);
    return makeColorMenu(li, propertyName, alphaName);
}
