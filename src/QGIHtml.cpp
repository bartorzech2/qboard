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

#include "QGIHtml.h"
#include "S11nQt.h"
#include "utility.h"
#include "MenuHandlerGeneric.h"
#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QKeySequence>
#include <QFont>
struct QGIHtml::Impl
{
	Impl()
	{
		
	}
	~Impl()
	{
		
	}
};
QGIHtml::QGIHtml() :
	QGraphicsTextItem(),
	Serializable("QGIHtml"),
	impl(new Impl)
{
	this->setup();
}
void QGIHtml::setup()
{
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
	this->setOpenExternalLinks(true);
	// These widgets normally don't look right with a transparent background,
	// so we explicitly set it to white. Also, if they have no text and therefore
	// no size, they can become invisible when added to a board, so we'll insert
	// some default text.
	this->setHtml("<html><body style='background-color:white'><p>Double-click to edit.</p></body></html>");
}
QGIHtml::~QGIHtml()
{
	QBOARD_VERBOSE_DTOR << "~QGIHtml()";
	delete impl;
}


bool QGIHtml::event( QEvent * e )
{
#if 1
    typedef QMap<QString,bool> PropMarkMap;
    static PropMarkMap map;
    if( map.isEmpty() )
    {
	map["pos"] = 1;
	map["html"] = 1;
	map["angle"] = 1;
    }
    while( e->type() == QEvent::DynamicPropertyChange )
    {
	QDynamicPropertyChangeEvent *chev = dynamic_cast<QDynamicPropertyChangeEvent *>(e);
	if( ! chev ) break; 
	QByteArray bakey( chev->propertyName() );
	const QString key( bakey );
	if(0) qDebug() << "QGIHtml::event(): DynamicPropertyChange: propery key ="<<key;
	if( ! map.contains(key) ) break;
	char const * ckey = bakey.constData();
	QVariant val( this->property(ckey) );
	if( QString("pos") == key )
	{
	    this->setPos( val.toPoint() );
	}
	else if( QString("html") == key )
	{
	    this->setHtml( val.toString() );
	}
	else if( QString("angle") == key )
	{
	    qboard::rotateCentered( this, val.toDouble() );
	}
	else
	{
	    break;
	}
	e->accept();
	this->update();
	return true;
    }
#endif
    return QObject::event(e);
}


void QGIHtml::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev )
{
	if( ev->buttons() & Qt::LeftButton )
	{
		ev->accept();
		this->setTextInteractionFlags( Qt::TextEditorInteraction );
	}
	else
	{
		this->QGraphicsTextItem::mouseDoubleClickEvent(ev);
	}
}


void QGIHtml::focusOutEvent( QFocusEvent * event )
{
	event->accept();
	this->setTextInteractionFlags( Qt::NoTextInteraction );
}

void QGIHtml::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	if( ev->buttons() & Qt::MidButton )
	{
		ev->accept();
		QGIHtmlEditor ed(this);
		ed.exec();
		return;
	}
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
bool QGIHtml::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	NT::set( dest, "angle", this->property("angle").toInt() );
#if 0
	if( this->metaObject()->propertyCount() )
	{
		S11nNode & pr( s11n::create_child( dest, "properties" ) );
		if( ! QObjectProperties_s11n()( pr, *this ) ) return false;
	}
#endif
	const QString html( this->toHtml() );
	const long thresh = 128;
	bool ret = true;
	if( html.size() > thresh )
	{
	    ret = s11n::serialize_subnode( dest, "html", html.toUtf8() );
	}
	else
	{
	    ret = s11n::serialize_subnode( dest, "html", html );
	}
	
	return ret
	    && s11n::serialize_subnode( dest, "pos", this->pos().toPoint() );
}

bool QGIHtml::deserialize(  S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	typedef S11nNodeTraits NT;
	this->setHtml("");
	{
	    QPointF p;
	    if( ! s11n::deserialize_subnode( src, "pos", p ) ) return false;
	    this->setProperty( "pos", p );
	}
	{
	    QString html;
	    if( ! s11n::deserialize_subnode( src, "html", html ) ) return false;
	    this->setHtml(html);
	}
	{
	    int angle = NT::get( src, "angle", int(0) );
	    if( angle != 0.0 )
	    {
		this->setProperty("angle", QVariant(angle));
	    }
	}
#if 0
	S11nNode const * ch = s11n::find_child_by_name(src, "properties");
	return ch
		? QObjectProperties_s11n()( *ch, *this )
		: true;
#endif
	return true;
}

void QGIHtml::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	MenuHandlerQGIHtml mh;
	mh.doMenu( this, event );
}



QGIHtmlEditor::QGIHtmlEditor(QGIHtml * item, QWidget * parent) 
	: QDialog(parent),
	Ui::QGIHtmlEditor(),
	m_item(item)
{
	this->setupUi(this);
	this->textEdit->setPlainText( item->toHtml() );
}
#include <QFont>
void QGIHtmlEditor::textBold(bool b)
{
	QFont fn( this->m_item->font() );
	fn.setWeight( b ? QFont::Bold : QFont::Normal);
	m_item->setFont(fn);
}

void QGIHtmlEditor::accept()
{
    m_item->setHtml( this->textEdit->toPlainText() );
    this->QDialog::accept(); 
}

void QGIHtmlEditor::reject()
{
	this->QDialog::reject(); 
}

MenuHandlerQGIHtml::MenuHandlerQGIHtml()
{
	
}

MenuHandlerQGIHtml::~MenuHandlerQGIHtml()
{
}
// #include <QMessageBox>
// #include <QUrl>
#include "utility.h"
void MenuHandlerQGIHtml::showHelp()
{
    qboard::showHelpResource("QGIHtml widget", ":/QBoard/help/classes/QGIHtml.html");
}
void MenuHandlerQGIHtml::doMenu( QGIHtml * pv, QGraphicsSceneContextMenuEvent * ev )
{
	ev->accept();
	MenuHandlerCommon proxy;
	QMenu * m = proxy.createMenu( pv );
	QList<QObject*> list;
	if( pv->isSelected() )
	{
	    list = qboard::selectedItemsCast<QObject>( pv->scene() );
	}
	else
	{
	    list.push_back(pv);
	}
	if(1)
	{
	    QObjectPropertyMenu * pm = QObjectPropertyMenu::makeIntListMenu("Rotate",list,"angle",0,360,15);
	    pm->setIcon(QIcon(":/QBoard/icon/rotate_cw.png"));
	    m->addMenu(pm);
	}
#if 1
	m->addSeparator();
	MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( pv, m );
	m->addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy",clipper,SLOT(clipboardCopy()) );
	m->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut",clipper,SLOT(clipboardCut()) );
#endif
	m->addSeparator();
	m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
	m->exec( ev->screenPos() );
	delete m;
}
