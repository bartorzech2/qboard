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

#include "GameState.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QList>
#include <QScriptEngine>
#include <QScriptValueIterator>

#include <stdexcept>

#include "S11nQt.h"
#include "S11nQtList.h"
#include "QGIPiece.h"
#include "utility.h"
#include "QBoard.h"
#include "QBoardScene.h"
#include "S11nClipboard.h"
#include "ScriptQt.h"

#include <s11n.net/s11n/s11n_debuggering_macros.hpp>

struct GameState::Impl
{
    QBoard board;
//     QPoint placeAt;
    QGraphicsScene * scene;
    QScriptValue jsThis;
    QScriptEngine * js;
    Impl() :
	board(),
	scene( new QBoardScene() ),
	jsThis(),
	js(0)
    {
	scene->setSceneRect( QRectF(0,0,200,200) );
    }
    ~Impl()
    {
	delete this->js;
	delete this->scene;
    }


};
/**
   Requires:

   $1 = QObject target
   $2 = property name
   $3 = value
*/
static QScriptValue jsEtGameProperty(QScriptContext *context,
				     QScriptEngine *engine)
{
    const int argc = context->argumentCount();
    qDebug() << "jsEtGameProperty: argc =="<<argc;
    if( argc < 2 ) return QScriptValue();
    int arg = 0;
    QScriptValue so( context->argument(arg++) );
    QObject * o = so.toQObject();
    if( ! o ) return QScriptValue();
    if( argc == 3 )
    {
	QString k = context->argument(arg++).toString();
	if( k.isEmpty() ) return QScriptValue();
	QScriptValue sv( context->argument(arg++) );
	QVariant v( sv.toVariant() );
	//if( ! v.isValid() ) return QScriptValue();
	char const * key = k.toAscii().constData();
	qDebug() << "jsEtGameProperty: SET"<<k<<" ="<<v;
	so.setProperty( key, sv );
	o->setProperty( key, v );
	return QScriptValue();
    }
    else if( 2 == argc )
    {
	QString key = context->argument(1).toString();
	if( ! key.isEmpty() ) return QScriptValue();
	QVariant const var( o->property(key.toAscii().constData()) );
	qDebug() << "jsEtGameProperty: GET"<<key<<" ="<<var;
	if( ! var.isValid() ) return QScriptValue();
	const int vt = var.type();
	switch(vt)
	{
#define VT(T,F) case QVariant::T: return QScriptValue(engine,F);
	    VT(Int,var.toInt());
	    VT(UInt,var.toUInt());
	    VT(Double,qsreal(var.toDouble()));
	    VT(Bool,var.toBool());
	    VT(String,var.toString());
#undef VT
#define VT(T,V) case QVariant::T: return engine->newVariant( var.value<V>() );
	    VT(Point,QPoint);
	    VT(Size,QSize);
#undef VT
	  default:
	      break;
	};
    }
    return QScriptValue();
}


//Q_DECLARE_METATYPE(QGraphicsItem*);



GameState::GameState() :
    QObject(),
    Serializable("GameState"),
    impl(new Impl)
{
    this->setup();
}


GameState::~GameState()
{
    this->clear();
    delete impl;
}

void GameState::setup()
{
    impl->js = qboard::createScriptEngine(this);
    impl->jsThis = impl->js->newQObject( this,
					 QScriptEngine::QtOwnership
					 //QScriptEngine::AutoCreateDynamicProperties
					 );
    QScriptValue glob( impl->js->globalObject() );
    glob.setProperty( "qboard", impl->jsThis );
    if( ! impl->jsThis.isQObject() )
    {
	throw std::runtime_error("GameState::setup(): jsThis is not a QObject.");
    }
    QScriptValue sval;
#if 0
    sval = impl->js->newFunction(jsEtGameProperty);
    if( ! sval.isFunction() )
    {
	throw std::runtime_error("GameState::setup(): could not create JS function.");
    }
    impl->jsThis.setProperty( "P", sval );
    impl->jsThis.setProperty( "x", QScriptValue(impl->js,"hi") );
#endif
    sval = impl->js->newQObject(&impl->board,
				QScriptEngine::QtOwnership
				//,QScriptEngine::AutoCreateDynamicProperties
				);
    impl->jsThis.setProperty( "board", sval );
}
// QPoint GameState::placementPos() const
// {
//     return impl->placeAt;
// }

// void GameState::setPlacementPos( QPoint const & p )
// {
//     qDebug() <<"GameState::setPlacementPos("<<p<<")";
//     impl->placeAt = p;
// }


QScriptEngine & GameState::jsEngine() const
{
    return *impl->js;
}
#if 0
static QScriptValue getObjectPos(QScriptContext *context, QScriptEngine *engine)
 {
     qDebug() << "GameState static getObjectPos()";
     QGraphicsItem * gi = engine->fromScriptValue<QGraphicsItem*>( context->argument(0) );
     return gi
	 ? QScriptValue(engine,"this is not a point")
	 : engine->toScriptValue<QPoint>( gi->pos().toPoint() );
//      if (context->argumentCount() == 1) // writing?
//          callee.setProperty("value", context->argument(0));
//     return callee.property("value");
}
#endif

// static QScriptValue pieceSetProp(QScriptContext *ctx,
// 				 QScriptEngine *eng)
// {
//     if( ! ctx || !eng) return QScriptValue();
//     return QScriptValue();
// }

QObject * GameState::createObject( QString const & className,
				   QScriptValue const & props )
{
    Serializable * s = s11n::cl::classload<Serializable>( className.toAscii().constData() );
    if( ! s )
    {
	qDebug() <<"GameState::createObject("<<className<<") classload failed.";
	return 0;
    }
    QObject * o = dynamic_cast<QObject*>(s);
    if( ! o )
    {
	qDebug() <<"GameState::createObject(obj) object not-a QGraphicsItem.";
	s11n::cleanup_serializable( s );
	return 0;
    }
    if( props.isObject() )
    {
	this->props( o, props );
    }
    QGraphicsItem * git = dynamic_cast<QGraphicsItem*>(s);
    if( git )
    {
	this->addItem(git);
    }
    QScriptValue v = impl->js->newQObject(o,
					  QScriptEngine::AutoOwnership
					  //,QScriptEngine::AutoCreateDynamicProperties
					  //| QScriptEngine::PreferExistingWrapperObject
					  );
    v.setProperty("x", QScriptValue(impl->js,"hi"));
    // never being called when obj.pos called.
    //v.setProperty("pos", impl->js->newFunction(getObjectPos), QScriptValue::PropertyGetter );
    QScriptValue fun = impl->js->newFunction(jsEtGameProperty);
    if( ! fun.isFunction() )
    {
	qDebug() <<"GameState::createObject(obj) prop function creation failed.";
    }
    else
    {
	// WTF does this end up undefined in script:
	v.setProperty("xyz",fun);
	//v.setProperty("xyz",QScriptValue(impl->js,"wtf"));
    }

    return o;
}

#if 0
//bool GameState::addObject( QScriptValue vobj )
bool GameState::addObject( QObject * obj ) //QScriptValue const & vobj )
{
    //if( !vobj || ! vobj->isQObject() ) return false;
    //QObject * obj = vobj->toQObject();
    QGraphicsItem * it = dynamic_cast<QGraphicsItem*>( obj );
    if( ! it )
    {
	qDebug() <<"GameState::addObject(obj) object not-a QGraphicsItem.";
	return 0;
    }
#if 0
    QScriptValue fun = impl->js->newFunction(jsEtGameProperty);
    if( ! fun.isFunction() )
    {
	qDebug() <<"GameState::addObject(obj) prop function creation failed.";
	return 0;
    }
    // WTF does this end up undefined in script:
    //vobj->.setProperty("prop",fun);
#endif
    this->addItem( it );
    return true;
}
#endif
bool GameState::prop( QObject * obj,
		      QString const & name,
		      //QVariant const & val
		      QScriptValue const & val
		      )
{
    if( ! obj || name.isEmpty() ) return false;
    QVariant var(val.toVariant());
    qDebug() << "GameState::prop(obj,"<<name<<","<<var<<")";
    obj->setProperty(name.toAscii().constData(),var);
    return val.isValid();
}
QScriptValue
GameState::prop( QObject * obj,
		 QString const & name )
{
    return (obj && !name.isEmpty())
	? impl->js->newVariant( obj->property(name.toAscii().constData()) )
	: QScriptValue();
}

bool GameState::props( QObject * tgt, QScriptValue const & props )
{
    qDebug() << "GameState::prop(obj,properties)";
    if( !tgt || ! props.isObject() ) return false;
    QScriptValueIterator it( props );
    while( it.hasNext() )
    {
	it.next();
	prop( tgt, it.name(), it.value() ); // .toVariant() );
    }
    return true;
}

QScriptValue GameState::evalScriptFile( QString const & fn )
{
    QFile scriptFile(fn);
    QString sname;
    QString contents;
    if (!scriptFile.open(QIODevice::ReadOnly))
    {
	sname = QString("GameState::evalScriptFile(%1)").arg(fn);
	contents = QString("new Error(\"GameState::evalScriptFile() could not open file [%1]\")").arg(fn);
    }
    else
    {
	sname = fn;
	QTextStream stream(&scriptFile);
	contents = stream.readAll();
	scriptFile.close();
    }
    return impl->js->evaluate( contents, sname );
}

QGraphicsScene * GameState::scene()
{
    return impl->scene;
}

bool GameState::addItem( QGraphicsItem * it )
{
    return it
	? (impl->scene->addItem( it ),true)
	: false;
}


void GameState::clear()
{
    // Pieces should be cleared first, to avoid potential double or otherwise inappropriate
    // deletes due to the use QObject::deleteLater() in QGIGamePiece.
    impl->board.clear();
    QList<QGraphicsItem *> ql(impl->scene->items()); 
    //qDebug() <<"GameState::clear() trying to clear"<<ql.size()<<" QGI items";
    qboard::destroyToplevelItems( ql );
}

QBoard & GameState::board()
{
    return impl->board;
}


bool GameState::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    QList<Serializable*> serItems;
    typedef QList<QGraphicsItem *> QL;
    QL ql( this->impl->scene->items() );
    QL::iterator it = ql.begin();
    QL::iterator et = ql.end();
    for( ; et != it; ++it )
    {
	if( (*it)->parentItem() )
	{ /* assume parent item will serialize the children if needed/desired.
	     In any case, we can't know how to re-build the parent
	     relationships from here. */
	    continue;
	}
	Serializable * ser = dynamic_cast<Serializable*>( *it );
	if( ! ser )
	{
	    qDebug() << "WARNING: GameState::serialize(): skipping non-Serializable QGraphicsItem:"<<*it;
	    continue;
	}
	serItems.push_back(ser);
    }
    return s11n::serialize_subnode( dest, "board", this->impl->board )
	&& (serItems.isEmpty() ? true : s11nlite::serialize_subnode( dest, "graphicsitems", serItems ) )
	;
}

bool GameState::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    this->clear();
    if( ! s11n::deserialize_subnode( src, "board", this->impl->board ) ) return false;
    S11nNode const * ch = s11n::find_child_by_name(src, "graphicsitems");
    if( ch )
    {
	typedef QList<Serializable*> QL;
	QL li;
	try
	{
	    if( ! s11n::deserialize( *ch, li ) ) return false;
	    QL::iterator it = li.begin();
	    QL::iterator et = li.end();
	    for( ; et != it; ++it )
	    {
		QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>( *it );
		//li.erase(it);
		if( ! gi )
		{
		    s11n::cleanup_serializable( li );
		    return false;
		}
		this->addItem( gi );
	    }
	    li.clear();
	}
	catch(...)
	{
	    s11n::cleanup_serializable( li );
	    qDebug() << "GameState::deserialize() caught exception. Cleaning up and passing it on.";
	    throw;
	}
    }
    return true;
}

static void adjustPos( QGraphicsItem * obj, QPoint const & pos )
{
    if( ! obj ) return;
    QVariant vpos( obj->pos() );
    QPoint cpos = vpos.isValid() ? vpos.toPoint() : QPoint();
    QPoint pD;
    if( ! pos.isNull() && ! cpos.isNull() )
    {
	pD = cpos - pos;
    }
    QPoint newpos = cpos.isNull() ?  pos : cpos;
    if(0) qDebug() << "adjustPos("<<obj<<","<<pos<<"): delta="<<pD<<"cpos="<<cpos<<"newpos="<<newpos;
    newpos -= pD;
    if(0) qDebug() << "adjustPos("<<obj<<","<<pos<<"): delta="<<pD<<"cpos="<<cpos<<"newpos="<<newpos;
    if( newpos != cpos )
    {
	obj->setPos( newpos );
    }
}

bool GameState::pasteTryHarder( S11nNode const & root,
				QPoint const & pos )
{
    // FIXME: refactor this into reusable parts!
    if(0) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<")";
    /**
       We will be randomly attempting deserializations which are likely
       to fail. This may cause warning messages from s11n, via its
       debugging facility, so we turn those off for the life of this
       function.
    */
    s11n::debug::trace_mask_changer sentry( s11n::debug::TRACE_NEVER );
    try {
	if( impl->board.deserialize(root) )
	{
	    return true;
	}
    }
    catch(...) {}


    try {
	Serializable * ser = s11nlite::deserialize<Serializable>(root);
	if( ser )
	{
	    if(0) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") <Serializable>";
	    QGraphicsItem * qgi = dynamic_cast<QGraphicsItem*>(ser);
	    if( qgi )
	    {
		if(1) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") Serializable QGI";
		adjustPos( qgi, pos );
		this->addItem( qgi );
		return true;
	    }
	    if(1) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") non-QGI Serializable";
	    s11n::cleanup_serializable<Serializable>( ser );
	}
    }
    catch(...) {}

    typedef QList<Serializable*> SerList;
    SerList serToClean;
    SerList pli;
    try {
	if( s11nlite::deserialize(root, pli) )
	{
	    if(1) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") QList<Serializable*>";
	    const int stride = 10;
	    QPoint step(stride,stride);
    	    QPoint xpos( pos );
	    //qDebug() << "GameState::pasteTryHarder(): pasting QList<Serializable*> @"<<pos;
	    impl->scene->clearSelection();
	    qreal zL = -1;
	    //QRectF bounds( impl->scene->sceneRect() );
	    for( SerList::iterator it = pli.begin();
		 it != pli.end(); ++it )
	    {
		QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>(*it);
		if( ! gi )
		{
		    serToClean.push_back(*it);
		    if(1) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") skipping non-QGraphicsItem Serializable:"<<(*it)->s11nClass();
		    continue;
		}
		if(0) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") got QGraphicsItem:"<<gi;
		if( zL < 0 ) zL = gi->zValue();
		if( zL < 0 ) zL = 0.01;
		gi->setZValue(zL);
		zL += 0.01;
		gi->setSelected(true);
#if 0
// i would like to do this, but bounds is always (0,0,200,200), and i don't know why.
		QRectF gb( gi->boundingRect() );
		qDebug() << "pos="<<pos<<", xpos="<<xpos<<", step="<<step<<", bounds="<<bounds<<", gi->bounds="<<gb;
		if( (xpos.x() + gb.width()) >= bounds.right() )
		{
		    step = QPoint(-stride, stride);
		}
		else if( xpos.x() <= bounds.left() )
		{
		    step = QPoint(stride,stride);
		}
		if( (xpos.y() + gb.height()) >= bounds.bottom() )
		{
		    step = QPoint(step.x(),-stride);
		}
		else if( xpos.y() <= bounds.top() )
		{
		    step = QPoint(step.x(),stride);
		}
#endif
		gi->setPos( xpos );
		xpos += step;
		this->addItem( gi );
	    }
	    pli.clear();
	    s11n::cleanup_serializable( serToClean );
	    return true;
	}
    }
    catch(...) {
	s11n::cleanup_serializable( pli );
	s11n::cleanup_serializable( serToClean );
	return false;
    }
    if(1) qDebug() << "GameState::pasteTryHarder(node,"<<pos<<") could not find a match!";

    return false;
}


char const * GameState::KeyClipboard = "QBoardClipboard";
bool GameState::pasteClipboard( QPoint const & target )
{
    if(0) qDebug() << "GameState::pasteClipboard("<<target<<")";
    typedef QList<QGraphicsItem *> QGIL;
    typedef QList<Serializable *> SerL;
    S11nNode const * root = S11nClipboard::instance().contents();
    typedef S11nNodeTraits TR;
    if( ! root ) return false;
    if( (TR::name(*root) != KeyClipboard ) )
    {
	return root ? this->pasteTryHarder(*root,target) : false;
    }
    QPoint origin(0,0);
    S11nNode const * node = 0;
    if( (node = s11n::find_child_by_name(*root, "metadata")) )
    {
	s11nlite::deserialize_subnode( *node, "originPoint", origin );
	node = 0;
    }
    QPoint pD;
    node = s11n::find_child_by_name(*root, "graphicsitems");
    QGIL newSelected;
    if( node )
    {
	bool gotOrigin = false;
	if(0) qDebug() << "qboard::pasteClipboard() pasting items";
	SerL list;
	if( ! s11nlite::deserialize( *node, list ) )
	{
	    if(1) qDebug() << "qboard::pasteClipboard() pasting deserialization of graphicsitems failed!";
	    return false;
	}
	for( SerL::iterator it = list.begin();
	     it != list.end(); ++it )
	{
	    QGraphicsItem * qgi = dynamic_cast<QGraphicsItem*>( *it );
	    if( ! qgi )
	    {
		if(1) qDebug() << "qboard::pasteClipboard() warning: skipping non-piece, non-QGI Serializable in input.";
		delete( *it );
		continue;
	    }
	    QPoint newpos = qgi->pos().toPoint();
	    if( ! gotOrigin )
	    {
		gotOrigin = true;
		if( origin.isNull() )
		{
		    if( target.isNull() )
		    {
			pD = QPoint();
			origin = newpos;
		    }
		    else
		    {
			origin = newpos;
			pD = newpos - target;
		    }
		}
		else
		{
		    if( target.isNull() )
		    {
			pD = QPoint();
		    }
		    else
		    {
			pD = origin - target;
		    }
		}
	    }
	    newpos -= pD;

#if 1	    
	    qgi->setPos( newpos );
#else
	    if( QObject * qobj = dynamic_cast<QObject*>(*it) )
	    {
		qobj->setProperty( "pos", newpos );
	    }
	    else
	    {
		qgi->setPos( newpos );
	    }
#endif
	    if(0) qDebug() << "qboard::pasteClipboard() adding Serializable QGI to scene:"
			   << "newpos ="<<newpos
			   << qgi;
	    newSelected.push_back(qgi);
	    this->addItem(qgi);
	    continue;
	}
    }

    node = s11n::find_child_by_name(*root, "board");
    if( node )
    {
	if( ! s11nlite::deserialize( *node, impl->board ) )
	{
	    if(1) qDebug() << "qboard::pasteClipboard() pasting deserialization of board failed!";
	}
    }

    if( ! newSelected.empty() )
    {
	this->scene()->clearSelection();
	QGIL::iterator it = newSelected.begin();
	for( ; newSelected.end() != it; ++it )
	{
	    (*it)->setSelected(true);
	}
	newSelected.clear();
    }
    return true;
}

