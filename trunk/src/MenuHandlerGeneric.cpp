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
static S11nClipboard & clipboard()
{
    return S11nClipboard::instance();
}

MenuHandlerCopyCut::MenuHandlerCopyCut(QGraphicsItem * gi,QObject * parent)
    : QObject(parent),
      m_gi(gi)
{
}
MenuHandlerCopyCut::~MenuHandlerCopyCut()
{
	
}

void MenuHandlerCopyCut::clipboard( QGraphicsItem * gvi, bool copy )
{
    if( ! gvi ) return;
    //qDebug() <<"MenuHandlerCopyCut::clipboard(item,"<<copy<<")";
    typedef QList<QGraphicsItem *> QL;
    typedef QList<Serializable *> SL;
    SL tops;
    QL toCut;
    if( gvi->isSelected() )
    {
	QL ql( gvi->scene()->selectedItems() );
	for( QL::iterator it = ql.begin(); ql.end() != it; ++it )
	{
	    if( (*it)->parentItem() ) continue;
	    //qDebug() <<"MenuHandlerCopyCut::clipboard() marking " << *it;
	    if( !copy ) toCut.push_back(*it);
	    Serializable * ser = dynamic_cast<Serializable*>(*it);
	    if( ser )
	    {
		//qDebug() <<"MenuHandlerCopyCut::clipboard() marking for CUT " << *it;
		tops.push_back(ser);
	    }
	}
    }
    else
    {
	//qDebug() <<"MenuHandlerCopyCut::clipboard() single object " << gvi;
	if( ! copy ) toCut.push_back(gvi);
	// Yet another QGIGamePiece special case.... we need to do something about that.
	Serializable * ser = dynamic_cast<Serializable*>(gvi);
	if( ! ser )
	{
	    qDebug() << (copy?"copy":"cut")<<"handler cannot handle non-Serializables."
		     << "Skipping object "<<gvi;
	}
	else
	{
	    //qDebug() <<"MenuHandlerCopyCut::clipboard() single object is Serializable ??? " << ser;
	    tops.push_back(ser);
	}
    }
    if( tops.empty() ) return;
    S11nNode * node = S11nNodeTraits::create("serializables");
    try
    {
	if( ! s11nlite::serialize( *node, tops ) )
	{
	    delete node;
	    return;
	}
    }
    catch(std::exception const & ex)
    {
	delete node;
	qDebug() << "MenuHandlerCopyCut::clipboard(): serialization threw:"<<ex.what();
	return;
    }
    tops.clear();
    ::clipboard().slotCut(node);
    if( copy )
    {
	//qDebug() << "MenuHandlerCopyCut::clipboard() copied data.";
    }
    else
    {
	//qDebug() << "MenuHandlerCopyCut::clipboard() cut data.";
	qboard::destroy( toCut );
    }
    S11nClipboard::S11nNode * cont = ::clipboard().contents();
    if( cont )
    {
	qDebug() <<"Clipboard contents:";
	s11nlite::save( *cont, std::cout );
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
    QString label("Item...");
    if( gi->isSelected() )
    {
	label = "Selected items...";
    }
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

