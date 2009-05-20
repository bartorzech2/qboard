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

#include <qboard/JSGameState.h>
#include <qboard/GameState.h>
#include <qboard/QBoardView.h>

#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>
#include <QGraphicsItem>
#include <QScriptValueIterator>

#include <qboard/ScriptQt.h>
#include <qboard/JSQGI.h>
#include <qboard/utility.h>

#define SELF(RV) GameState *self = this->self(); \
    QScriptEngine * js = this->engine(); \
    if(!self || !js) return RV;

struct JSGameState::Impl
{
    qboard::JSQGI * qgiproto;
    QScriptValue qgiprotoj;
    Impl() :
	qgiproto(0),
	qgiprotoj()
    {
    }
    ~Impl()
    {
    }
};

JSGameState::JSGameState( QObject * parent )
    : QObject(parent),
      QScriptable(),
      impl(new Impl)
{
}

JSGameState::~JSGameState()
{
    delete impl;
}

QString JSGameState::home() const
{
    return qboard::home().absolutePath();
}


//QBoardView *
QScriptValue
JSGameState::createView()
{
    SELF(QScriptValue());
    return js->newQObject(new QBoardView(*self),
			  QScriptEngine::ScriptOwnership);
}
void JSGameState::bogo()
{
    qDebug() << "JSGameState::bogo(): self =="<<this->self();
}

bool JSGameState::save( QString const & fn, bool autoAddExt )
{
    return self()->s11nSave( fn, autoAddExt );
}

bool JSGameState::load( QString const & fn )
{
    return self()->s11nLoad( fn );
}

GameState * JSGameState::self()
{
    if( ! this->engine() ) return 0;
#if 1
    if( (!impl->qgiproto) )
    {
	// FIXME: we only need one proto per engine
	impl->qgiproto = new qboard::JSQGI(this);
	impl->qgiprotoj = this->engine()->newQObject(impl->qgiproto);
#if 0
	QScriptValue qgip = this->engine()->defaultPrototype( qMetaTypeId<QGraphicsItem*>() );
	if( qgip.isValid() )
	{
	    qDebug() << "JSGameState::self() found default QGraphicsItem prototype.";
	    impl->qgiprotoj.setPrototype( qgip );
	}
	else
	{
	    impl->qgiprotoj = this->engine()->newQObject(impl->qgiproto);
	}
#endif
    }
#endif
    GameState * s = qscriptvalue_cast<GameState*>(thisObject());
    if( ! s )
    {
	this->context()->throwError("JSGameState::self() got a null pointer.");
    }
    return s;
}

QList<QGraphicsItem*> JSGameState::items()
{
    SELF(QList<QGraphicsItem*>());
    return self->scene()->items();
}

bool JSGameState::prop( QObject * obj,
		      QString const & name,
		      //QVariant const & val
		      QScriptValue const & val
		      )
{
    if( ! obj
	|| name.isEmpty()
	|| name.startsWith("_")
	) return false;
    QVariant var(val.toVariant());
    if(0) qDebug() << "JSGameState::prop(obj,"<<name<<","<<var<<")";
    obj->setProperty(name.toAscii().constData(),var);
    return val.isValid();
}
QScriptValue
JSGameState::prop( QObject * obj,
		   QString const & name )
{
    SELF(QScriptValue())
    QScriptValue ret( js->nullValue() );
    if( obj && !name.isEmpty() )
    { // my setDefaultPrototype() is not working for QVariant, so i'll hack in a prototype here...
	QVariant var( obj->property(name.toAscii().constData()) );
	if(0) qDebug() << "JSGameState::prop(["<<obj<<"],["<<name<<"]) variant =="<<var;
	ret = js->newVariant( var );
	ret.setPrototype( js->globalObject().property("JSVariantPrototype" ) );
    }
    else
    {
	qDebug() << "warning: JSGameState::prop(["<<obj<<"],["<<name<<"]) object and/or name are/is null.";
    }
    return ret;
}

bool JSGameState::props( QObject * tgt, QScriptValue const & props )
{
    if(0) qDebug() << "JSGameState::prop(obj,properties)";
    if( !tgt || ! props.isObject() ) return false;
    QScriptValueIterator it( props );
    while( it.hasNext() )
    {
	it.next();
	prop( tgt, it.name(), it.value() ); // .toVariant() );
    }
    return true;
}
void JSGameState::addItem( QGraphicsItem * it )
{
    SELF();
    self->addItem(it);
}

//QObject *
QScriptValue
JSGameState::createObject( QString const & className,
				     QScriptValue const & props )
{
    QScriptValue dflt;
    SELF(dflt);
    // FIXME: try loading via Qt's metatype system if s11n::cl fails.
    Serializable * s = s11n::cl::classload<Serializable>( className.toAscii().constData() );
    if( ! s )
    {
	qDebug() <<"JSGameState::createObject("<<className<<") classload failed.";
	return dflt;
    }
    QObject * o = dynamic_cast<QObject*>(s);
    if( ! o )
    {
	qDebug() <<"JSGameState::createObject("<<className<<") object is-not-a QObject.";
	s11n::cleanup_serializable( s );
	return dflt;
    }
    if( props.isObject() )
    {
	this->props( o, props );
    }
    QScriptValue jo = js->newQObject(o);//,QScriptEngine::ScriptOwnership);
    QGraphicsItem * git = dynamic_cast<QGraphicsItem*>(s);
    if( git )
    {
// 	QObject oldprops;
// 	qboard::copyProperties(this,&oldprops);
// 	qboard::clearProperties(this);
 	QVariant posval( o->property("pos") );
 	if( posval.isValid() )
 	{
 	    // Kludge to appease qt.gui bindings. If we don't
	    // do this then the 'pos' property overwrites
	    // the pos() function. :(
 	    o->setProperty("pos", QVariant() );
	    git->setPos( posval.value<QPointF>() );
 	}
#if 0
	if( -1 != o->metaObject()->indexOfSignal(SIGNAL(doubleClicked(QGraphicsItem*))) )
	{
	    connect(o,SIGNAL(doubleClicked(QGraphicsItem*)),
		    impl->qgiproto,SIGNAL(doubleClicked(QGraphicsItem*)));
	}
#endif

#if 1
#if 1
	// this partially conflicts with the one used by the qt.gui JS extension
	if(1) qDebug() << "JSGameState::createObject("<<className<<"): setting prototype to JSQGI.";
	jo.setPrototype( impl->qgiprotoj );
	QVariant varo(git ? true : false); // evaluates to bool! // not in qt4.5!
	varo.setValue<QGraphicsItem*>(git);
	qDebug() << "JSGameState::createObject() QGI:"<<varo;
	jo.setData( js->newVariant( varo ) );
	qDebug() << "JSGameState::createObject() re-read:"<<jo.property("__QGI").toVariant();
	//jo.setData( js->newVariant( QVariant(git) ) );
#else
	QScriptValue qgip =
	    //impl->qgiprotoj;
	    this->engine()->defaultPrototype( qMetaTypeId<QGraphicsItem*>() );
	if( qgip.isValid() )
	{
	    qDebug() << "JSGameState::createObject(): setting default QGI JS prototype.";
	    jo.setPrototype( qgip );
	}
	else
	{
	    qDebug() << "JSGameState::createObject(): no default QGI JS prototype found.";
	}
#endif
#endif
// 	qboard::copyProperties(&oldprops,this);
 	if( posval.isValid() )
 	{
 	}
	self->addItem(git);
    }
    return jo;
}


#undef SELF
