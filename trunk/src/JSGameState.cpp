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

#include "JSGameState.h"
#include "GameState.h"
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>
#include <QGraphicsItem>
#include <QScriptValueIterator>

#define SELF(RV) GameState *self = this->self(); \
    QScriptEngine * js = this->engine(); \
    if(!self || !js) return RV;

struct JSGameState::Impl
{
    Impl()
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
    GameState * s = qscriptvalue_cast<GameState*>(thisObject());
    if( ! s )
    {
	this->context()->throwError("JSGameState::self() got a null pointer.");
    }
    return s;
}

bool JSGameState::prop( QObject * obj,
		      QString const & name,
		      //QVariant const & val
		      QScriptValue const & val
		      )
{
    if( ! obj || name.isEmpty() ) return false;
    QVariant var(val.toVariant());
    qDebug() << "JSGameState::prop(obj,"<<name<<","<<var<<")";
    obj->setProperty(name.toAscii().constData(),var);
    return val.isValid();
}
QScriptValue
JSGameState::prop( QObject * obj,
		 QString const & name )
{
    SELF(this->engine()->nullValue());
    return (obj && !name.isEmpty())
	? js->newVariant( obj->property(name.toAscii().constData()) )
	: js->nullValue();
}

bool JSGameState::props( QObject * tgt, QScriptValue const & props )
{
    qDebug() << "JSGameState::prop(obj,properties)";
    if( !tgt || ! props.isObject() ) return false;
    QScriptValueIterator it( props );
    while( it.hasNext() )
    {
	it.next();
	prop( tgt, it.name(), it.value() ); // .toVariant() );
    }
    return true;
}

QObject * JSGameState::createObject( QString const & className,
				     QScriptValue const & props )
{
    SELF(0);
    // FIXME: try loading via Qt's metatype system if s11n::cl fails.
    Serializable * s = s11n::cl::classload<Serializable>( className.toAscii().constData() );
    if( ! s )
    {
	qDebug() <<"JSGameState::createObject("<<className<<") classload failed.";
	return 0;
    }
    QObject * o = dynamic_cast<QObject*>(s);
    if( ! o )
    {
	qDebug() <<"JSGameState::createObject("<<className<<") object is-not-a QObject.";
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
	self->addItem(git, true);
    }
#if 0
    QScriptValue v = js->newQObject(o, QScriptEngine::AutoOwnership
				    //,QScriptEngine::AutoCreateDynamicProperties
				    //| QScriptEngine::PreferExistingWrapperObject
				    );
    // never being called when obj.pos called.
    //v.setProperty("pos", impl->js->newFunction(getObjectPos), QScriptValue::PropertyGetter );
    QScriptValue fun = impl->js->newFunction(jsEtGameProperty);
    if( ! fun.isFunction() )
    {
	qDebug() <<"JSGameState::createObject(obj) prop function creation failed.";
    }
    else
    {
	// WTF does this end up undefined in script:
	v.setProperty("xyz",fun);
	//v.setProperty("xyz",QScriptValue(impl->js,"wtf"));
    }
#endif
    return o;
}


#undef SELF
