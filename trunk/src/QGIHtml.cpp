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

#include <qboard/QGIHtml.h>
#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QList.h>
#include <qboard/S11nQt/QByteArray.h>
#include <qboard/S11nQt/QGraphicsItem.h>
#include <qboard/S11nQt/QPointF.h>
#include <qboard/S11nQt/QPoint.h>
#include <qboard/S11nQt/QString.h>
#include <qboard/utility.h>
#include <qboard/MenuHandlerGeneric.h>
#include <qboard/S11nQt/S11nClipboard.h>

#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QKeySequence>
#include <QFont>

Q_DECLARE_METATYPE(QGIHtml*)

struct QGIHtml::Impl
{
    GraphicsItemFlags giflags;
    bool blocked;
    Impl() : blocked(false)
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

void QGIHtml::refreshTransformation()
{
    qboard::rotateAndScale( this,
			    this->property("angle").toDouble(),
			    this->property("scale").toDouble() );
}

bool QGIHtml::event( QEvent * e )
{
    typedef QMap<QString,bool> PropMarkMap;
    static PropMarkMap map;
    if( map.isEmpty() )
    {
	map["pos"] = 1;
	map["html"] = 1;
	map["angle"] = 1;
	map["scale"] = 1;
	map["zLevel"] = 1;
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
	else if( (QString("angle") == key)
		 || (QString("scale") == key) )
	{
	    this->refreshTransformation();
	}
	else if( QString("zLevel") == key )
	{
	    this->setZValue( val.toDouble() );
	}
	else
	{
	    break;
	}
	e->accept();
	this->update();
	return true;
    }
    return impl->blocked
	? true
	: this->QObject::event(e);
}


void QGIHtml::focusInEvent( QFocusEvent * event )
{
    if(0) qDebug() << "QGIHtml::focusInEvent() reason =="<<event->reason();
    /**
       A context menu event is preceeded by a focusIn and followed by
       a focusOut (immediately, before the menu closes). This causes
       us a bit of grief here.
    */
    if( (event->reason() == Qt::PopupFocusReason)
	|| (event->reason() == Qt::MouseFocusReason) )
    {
	return;
    }
    //impl->giflags = this->flags();//Qt4.3: If i call this, then items become immobile!!!
    this->setTextInteractionFlags( Qt::TextEditorInteraction );
    //event->accept();
    this->QGraphicsTextItem::focusInEvent(event);
}
void QGIHtml::focusOutEvent( QFocusEvent * event )
{
    if(0) qDebug() << "QGIHtml::focusOutEvent() blocked ="<<impl->blocked<<", reason =="<<event->reason();
    if( impl->blocked )
    {
	event->accept();
	return;
    }
    impl->blocked = true;
#if (QT_VERSION < 0x040400)
    /**
       Try to work around a problem in Qt 4.3 where this object
       becomes imobile after leaving edit mode. It doesn't seem
       to work 100% of the time, and i can't for the life of me
       figure out why it's inconsistent.
    */
    this->setTextInteractionFlags( Qt::NoTextInteraction ); // can cause endless recursion!
    this->setFlags(impl->giflags);
    event->accept();
    this->QGraphicsTextItem::focusOutEvent(event);
#else
    this->QGraphicsTextItem::focusOutEvent(event);
    this->setTextInteractionFlags( Qt::NoTextInteraction ); // can cause endless recursion!
    event->accept();
#endif
    impl->blocked = false;
}


void QGIHtml::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev )
{
	if( ev->buttons() & Qt::LeftButton )
	{
	    impl->giflags = this->flags();
	    ev->accept();
	    this->setTextInteractionFlags( Qt::TextEditorInteraction );
	    this->setFocus();
	    // ^^^ setFocus() is necessary for correct Qt4.3 interaction.
	}
	else
	{
	    this->QGraphicsTextItem::mouseDoubleClickEvent(ev);
	}
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
    QGITypes::handleClickRaise( this, ev );
#if 0
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
#endif
	//QGITypes::handleClickRaise( this, ev );
	//ev->accept();
	this->QGraphicsTextItem::mousePressEvent(ev);
}
bool QGIHtml::serialize( S11nNode & dest ) const
{
    // TODO: refactor this to use an approach similar to QGIPiece/QGIDot.
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	NT::set( dest, "angle", this->property("angle").toInt() );
	NT::set( dest, "scale", this->property("scale").toDouble() );
	NT::set( dest, "zLevel", this->zValue() );
	{
	    QList<QGraphicsItem *> chgi( qboard::childItems(this) );
	    if( ! chgi.isEmpty() )
	    {
		if( ! s11n::qt::serializeQGIList<Serializable>( s11n::create_child(dest,"children"),
								chgi, false ) )
		{
		    return false;
		}
	    }
	}
	const QString html( this->toHtml() );
	const long thresh = 128;
	if( html.size() > thresh )
	{
	    if( ! s11n::serialize_subnode( dest, "html", html.toUtf8() ) ) return false;
	}
	else
	{
	    if( ! s11n::serialize_subnode( dest, "html", html ) ) return false;
	}
	
	return s11n::serialize_subnode( dest, "pos", this->pos().toPoint() );
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
	    this->setProperty("angle", NT::get( src, "angle", qreal(0.0) ) );
	    double dbl = NT::get( src, "scale", qreal(1.0) );
	    if( dbl == 0.0 ) dbl = 1.0;
	    this->setProperty("scale", QVariant(dbl));
	}
	this->setZValue( NT::get( src, "zLevel", this->zValue() ) );
	S11nNode const * ch = 0;
	{
	    ch = s11n::find_child_by_name(src, "children");
	    if( ch )
	    {
		typedef QList<QGraphicsItem *> QGIL;
		QGIL childs;
		if( -1 == s11n::qt::deserializeQGIList<Serializable>( *ch, childs, this ) )
		{
		    return false;
		}
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

#include <qboard/QGIHider.h>
QGraphicsItem * QGIHtml::hideItems()
{
    QGIHider::hideItems( this );
    return this->parentItem();
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
    : m_gi(0)
{
	
}

MenuHandlerQGIHtml::~MenuHandlerQGIHtml()
{
}
// #include <QMessageBox>
// #include <QUrl>
#include <qboard/utility.h>
void MenuHandlerQGIHtml::showHelp()
{
    qboard::showHelpResource("QGIHtml widget", ":/QBoard/help/classes/QGIHtml.html");
}

void MenuHandlerQGIHtml::clipList( QGIHtml * src, bool copy )
{
    if( !src || ! src->scene() ) return;
    typedef QList<QGIHtml*> QSL;
    QSL sel( qboard::selectedItemsCast<QGIHtml>( src->scene() ) );
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

void MenuHandlerQGIHtml::copyList()
{
    clipList( m_gi, true );
}

void MenuHandlerQGIHtml::cutList()
{
    clipList( m_gi, false );
}

void MenuHandlerQGIHtml::doMenu( QGIHtml * pv, QGraphicsSceneContextMenuEvent * ev )
{
	ev->accept();
	m_gi = pv;
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
	    QObjectPropertyMenu * pm =
		QObjectPropertyMenu::makeNumberListMenu("Rotate",
							list,
							"angle",
							0,360,15);
	    pm->setIcon(QIcon(":/QBoard/icon/rotate_cw.png"));
	    m->addMenu(pm);
	}
	QMenu * mMisc = m->addMenu("Misc.");
	if(1)
	{
	    mMisc->addAction(QIcon(":/QBoard/icon/box_wrapped.png"),
			     "Cover",pv,SLOT(hideItems()));
	}

	if(1)
	{
	    QObjectPropertyMenu * pm =
		QObjectPropertyMenu::makeNumberListMenu("Scale",
							list, "scale",
							0.25, 3.01, 0.25);
	    pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	    mMisc->addMenu(pm);
	}
	m->addSeparator();
	MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( pv, m );
	QMenu * copySub = clipper->addDefaultEntries(m, true, pv->isSelected() );
	if( copySub )
	{
	    copySub->addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy selected as QList<QGIHtml*>",this,SLOT(copyList()) );
	    copySub->addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut selected as QList<QGIHtml*>",this,SLOT(cutList()) );
	}

	m->addSeparator();
	m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
	m->exec( ev->screenPos() );
	delete m;
}
