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
#include "S11nClipboard.h"
#include <QDebug>
//#include <Q>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "Serializable.h"
#include "QGIGamePiece.h"
#include "GamePiece.h"
#include "utility.h"

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
	qboard::clipboardGraphicsItems( gvi, copy );
    }
    else
    {
	if( gvi->parentItem() ) return;
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
    qboard::destroy( this->gitem, true );
}
void SceneSelectionDestroyer::destroyItem(QGraphicsItem * item)
{
    if( item == this->gitem ) gitem = 0;
    qboard::destroy( item, false );
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
    QString label = gi->isSelected() ? tr("Selected items...") : tr("Item...");
#if 0
    QVariant vp( QVariant::fromValue( gi ) );
    QGraphicsItem * gpt = vp.value<QGraphicsItem*>(); 
    qDebug() << "vp=="<<vp<<"ptr="<<gpt;
#endif
    QMenu * menu = new QMenu(label);
    menu->addAction(label)->setEnabled(false);
    SceneSelectionDestroyer * destroyer = new SceneSelectionDestroyer( menu, gi );
    menu->addAction(QIcon(":/QBoard/icon/button_cancel.png"),"Destroy",destroyer,SLOT(destroyItem()) );
    menu->addSeparator();
#if 0
    MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( gi, menu );
    menu->addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy",clipper,SLOT(clipboardCopy()) );
    menu->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut",clipper,SLOT(clipboardCut()) );
#endif
    return menu;
}


QObjectPropertyAction::QObjectPropertyAction(
					     QObject * obj,
					     QString const & propName,
					     QVariant const & val,
					     QObject * parent )
    : QAction(val.toString(),parent),
      m_o(obj),
      m_key(propName),
      m_val(val)
{
    if( val.type() == QVariant::Color )
    {
	QPixmap px(16,16);
	px.fill(val.value<QColor>());
	this->setIcon(QIcon(px));
    }
#if 1
    if( val == obj->property(propName.toAscii()) )
    {
	this->setCheckable(true);
	this->setChecked(true);
    }
#endif
    connect( m_o, SIGNAL(destroyed()), this, SLOT(dtorDisconnect()) );
    connect( this, SIGNAL(triggered()), this, SLOT(setProperty()) );
}
QObjectPropertyAction::~QObjectPropertyAction()
{
	
}

void QObjectPropertyAction::dtorDisconnect()
{
    this->m_o = 0;
}


void QObjectPropertyAction::setProperty()
{
    if( ! m_o ) return;
    m_o->setProperty(m_key.toAscii(), m_val);
}

QObjectPropertyMenu::QObjectPropertyMenu( const QString & title,
					  QObject * obj,
					  QString const & propName,
					  QWidget * parent ) :
    QMenu(title,parent),
    m_o(obj),
    m_key(propName)
{
}

QAction * QObjectPropertyMenu::addItem( QVariant const & val, QString const & lbl)
{
    QObjectPropertyAction * ac = new QObjectPropertyAction( m_o, m_key, val, this );
    if( ! lbl.isEmpty() )
    {
	ac->setText(lbl);
    }
    this->addAction(ac);
    return ac;
}

QObjectPropertyMenu::~QObjectPropertyMenu()
{
}


QObjectPropertyMenu * QObjectPropertyMenu::makePenStyleMenu( QObject * pv, char const * propertyName )
{
    QObjectPropertyMenu * pm = new QObjectPropertyMenu("Style", pv, propertyName, 0 );
    for( int i = Qt::NoPen; i < Qt::CustomDashLine; ++i )
    {
	pm->addItem( QVariant(i), qboard::penStyleToString(i) );
    }
    return pm;
}

QObjectPropertyMenu * QObjectPropertyMenu::makeIntListMenu(
							   char const * lbl, 
							   QObject * pv,
							   char const * propertyName,
							   int from,
							   int to,
							   int step )
{
    QObjectPropertyMenu * pm = new QObjectPropertyMenu(lbl, pv, propertyName, 0 );
    for( int i = from; i < to; i += step )
    {
	pm->addItem( QVariant(i) );
    }
    return pm;
}

QObjectPropertyMenu *
QObjectPropertyMenu::makeAlphaMenu(
				   QObject * obj,
				   char const * propertyName,
				   QColor const & hint )
{

    QObjectPropertyMenu * pm = new QObjectPropertyMenu("Transparency", obj, propertyName, 0 );
    const short opaque = 255;
    const qreal step = opaque/10.0;
    int pct = 0;
    for( qreal i = 0.0; i <= opaque; i += step, pct += 10)
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

QObjectPropertyMenu * QObjectPropertyMenu::makeColorMenu(
							 QObject * pv,
							 char const * propertyName,
							 char const * alphaName )
{
    QObjectPropertyMenu * mc = new QObjectPropertyMenu("Color",pv,propertyName,0);
    mc->setIcon(QIcon(":/QBoard/icon/colorize.png"));
    if( alphaName )
    {
	QColor hint( pv->property(propertyName).value<QColor>() );
	mc->addMenu( QObjectPropertyMenu::makeAlphaMenu(pv,alphaName, hint) );
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

