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

#include <qboard/GameState.h>

#include <QDebug>
#include <QGraphicsItem>
#include <QList>
#include <QScriptEngine>
#include <QScriptValueIterator>

#include <stdexcept>

#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QList.h>
#include <qboard/QGIPiece.h>
#include <qboard/utility.h>
#include <qboard/QBoard.h>
#include <qboard/QBoardScene.h>
#include <qboard/S11nQt/S11nClipboard.h>
#include <qboard/ScriptQt.h>
#include <qboard/S11nQt/QPoint.h>
#include <s11n.net/s11n/s11n_debuggering_macros.hpp>

#include <qboard/QGIPiecePlacemarker.h>
#include <qboard/JSGameState.h>
#include <qboard/JSQGI.h>


struct GameState::Impl
{
    QBoard board;
    QPointF placeAt;
    QGraphicsScene * scene;
    QScriptValue jsThis;
    QScriptEngine * js;
    QGIPiecePlacemarker * placer;
    Impl() :
	board(),
	placeAt(50,50),
	scene( new QBoardScene() ),
	jsThis(),
	js(0),
	placer(0)
    {
	scene->setSceneRect( QRectF(0,0,200,200) );
	scene->setObjectName("scene");
    }
    ~Impl()
    {
	delete this->placer;
	delete this->js;
	delete this->scene;
    }


};

#if 0
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
#endif // jsEtGameProperty()


QScriptValue getsetPlacementPos(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue obj = ctx->thisObject();
    GameState * s = qscriptvalue_cast<GameState*>(obj);
    if( ! s )
    {
	return eng->evaluate( QString("new Error('%1:%2: Internal error: (GameState*) is missing.')").
			      arg(__FILE__).arg(__LINE__) );
    }
    qboard::ScriptArgv argv(ctx);
    if( 0 == argv.argc() )
    {
	return eng->toScriptValue( s->placementPos() );
    }
    else if( 1 == argv.argc() )
    {
	QPointF p = qboard::fromScriptValue<QPointF>( eng, argv.value() );
	if(0) qDebug() << "Got QPointF pos: " << p;
	s->setPlacementPos( p );
	return argv.value();
    }
    return eng->evaluate("new Error('Incorrect argument count. Usage: (pos = QPoint(x,y)) or (var x = pos)')","error message");
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
    if( impl->placer )
    {
	QObject::disconnect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
    }
    this->clear();
    delete impl;
}
void GameState::setup()
{
    impl->js = qboard::createScriptEngine(this);
    QScriptValue glob( impl->js->globalObject() );

    JSGameState * proto = new JSGameState(this);
    impl->js->setDefaultPrototype(qMetaTypeId<GameState*>(),
				  impl->js->newQObject(proto));
    qboard::JSQGI * protoQGI = new qboard::JSQGI(this);
    impl->js->setDefaultPrototype(qMetaTypeId<QGraphicsItem*>(),
				  impl->js->newQObject(protoQGI));

    impl->jsThis = impl->js->newQObject( this,
					 QScriptEngine::QtOwnership
					 //QScriptEngine::AutoCreateDynamicProperties
					 );
    glob.setProperty( "qboard", impl->jsThis );
    if( ! impl->jsThis.isQObject() )
    {
	throw std::runtime_error("GameState::setup(): jsThis is not a QObject.");
    }
    impl->jsThis.setProperty( "client", impl->js->newObject() );

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
    impl->js->globalObject().setProperty("QT_VERSION",QScriptValue(impl->js,QT_VERSION));
    impl->js->globalObject().setProperty("QBOARD_VERSION",QScriptValue(impl->js,QBOARD_VERSION));
    impl->js->globalObject().setProperty("QBOARD_VERSION_STRING",QScriptValue(impl->js,qboard::versionString()));
    sval = impl->js->newQObject(&impl->board,
				QScriptEngine::QtOwnership
				//,QScriptEngine::AutoCreateDynamicProperties
				);
    impl->jsThis.setProperty( "board", sval );

    impl->jsThis.setProperty("pos",
			     impl->js->newFunction(getsetPlacementPos),
			     QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
    sval = impl->js->newQObject(impl->scene, QScriptEngine::QtOwnership );
    impl->jsThis.setProperty( "scene", sval );

}
QPointF GameState::placementPos() const
{
    return impl->placer
	? impl->placer->pos()
	: impl->placeAt;
}

void GameState::setPlacementPos( QPointF const & p )
{
    //qDebug() <<"GameState::setPlacementPos("<<p<<")";
    if( impl->placer )
    {
	impl->placer->setPos(p);
    }
    impl->placeAt = p;
}

void GameState::placemarkerDestroyed()
{
    /**
       This bit of kludgery is to work around impl->placer being
       destroyed when game state is cleared (destroying all
       QGraphicsItems), as that leads to a dangling impl->placer in
       this class. When that happens, we re-create the placemarker
       if we had one active already.
    */
    if( impl->placer ) // now (or soon) a dangling pointer
    {
	impl->placer = 0;
	this->enablePlacemarker(true);
    }
}

void GameState::enablePlacemarker( bool en )
{
    if( en )
    {
	if( ! impl->placer )
	{
	    impl->placer = new QGIPiecePlacemarker;
	    //impl->placer->setGameState( *this );
	    connect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
	    impl->placer->setZValue(-11111.11111);
	    impl->placer->setPos(impl->placeAt);
	    impl->scene->addItem( impl->placer );
	    QStringList help;
	    help << "<html><body>This is a \"piece placemarker\"."
		 << "To move it, double-click the board or drag it around."
		 << "Pieces which are loaded from individual files (as opposed to being part of a game)"
		 << "start out at this position."
		 << "</body></html>"
		;
	    impl->placer->setToolTip( help.join(" ") );
	}
	return;
    }
    if( impl->placer )
    {
	QObject::disconnect( impl->placer, SIGNAL(destroyed(QObject*)), this, SLOT(placemarkerDestroyed()) );
    }
    delete impl->placer;
    impl->placer = 0;
}



QScriptEngine & GameState::jsEngine() const
{
    return *impl->js;
}

QScriptValue GameState::evalScriptFile( QString const & fn )
{
    return qboard::jsInclude( impl->js, fn );
}

QGraphicsScene * GameState::scene()
{
    return impl->scene;
}

bool GameState::addItem( QGraphicsItem * it, bool autoPlace )
{
    if( ! it ) return false;
    if( autoPlace )
    {
	QPointF pos( this->placementPos() );
	QRectF bounds( it->boundingRect() );
	pos.setX( pos.x() - (bounds.width()/2) );
	pos.setY( pos.y() - (bounds.height()/2) );
	it->setPos(pos);
    }
    impl->scene->addItem( it );
    return true;
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
static QGraphicsItem * firstSelectedQGI( QGraphicsScene * sc )
{
    if( ! sc ) return 0;
    typedef QList<QGraphicsItem *> QGIL;
    QGIL li = sc->selectedItems();
    QGraphicsItem * qgi = 0;
    for( QGIL::iterator it = li.begin();
	 li.end() != it; ++it )
    {
	if( (*it)->isSelected() )
	{
	    qgi = *it;
	    break;
	}
    }
    return qgi;
}

void GameState::clipCopySelected()
{
    qboard::clipboardGraphicsItems( firstSelectedQGI( this->scene() ),  true );
}

void GameState::clipCutSelected()
{
    qboard::clipboardGraphicsItems( firstSelectedQGI( this->scene() ),  false  );
}
void GameState::clipPaste()
{
    this->pasteClipboard( QPoint(0,0) );
}

void GameState::selectAll()
{
    QGraphicsScene * sc = this->scene();
    if( sc )
    {
	QPainterPath path;
	path.addRect( sc->itemsBoundingRect() );
	sc->setSelectionArea( path, Qt::IntersectsItemShape );
    }

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
