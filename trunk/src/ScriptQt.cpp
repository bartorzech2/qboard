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

#include "ScriptQt.h"
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QDebug>
#include <QGraphicsItem>
#include <QSharedData>

namespace qboard {

 
    QScriptValue qpointToScriptValue(QScriptEngine *engine, const QPoint &s)
    {
	qDebug() << "qpointToScriptValue(engine,"<<s<<")";
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", QScriptValue(engine, s.x()));
	obj.setProperty("y", QScriptValue(engine, s.y()));
	return obj;
    }

    void qpointFromScriptValue(const QScriptValue &obj, QPoint &s)
    {
	s.setX( obj.property("x").toInt32() );
	s.setY( obj.property("y").toInt32() );
	qDebug() << "qpointFromScriptValue(engine,"<<s<<")";
    }

    QScriptValue QPoint_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	int argc = context->argumentCount();
	qDebug() << "QPoint_ctor(cx,engine) argc =="<<argc;
#if 0
	int x = (argc>0) ? context->argument(0).toInt32() : 0;
	int y = (argc>1) ? context->argument(1).toInt32() : x;
	return engine->toScriptValue(QPoint(x, y));
#else
	if( 1 == argc )
	{
	    QScriptValue arg = context->argument(0);
	    if( arg.isNumber() )
	    { // QPoint(x,x)
		int x = arg.toInt32();
		QPoint p(x,x);
		return engine->toScriptValue(p);
	    }
	    else if( arg.isObject() )
	    { // copy ctor
		QPoint pt;
		qpointFromScriptValue(arg,pt);
		return engine->toScriptValue(pt );
	    }
	    return QScriptValue();
	}
	else if( 2 == argc )
	{ // QPoint(x,y)
	    int x = (argc>0) ? context->argument(0).toInt32() : 0;
	    int y = (argc>1) ? context->argument(1).toInt32() : x;
	    return engine->toScriptValue(QPoint(x,y));
	}
	return QScriptValue();
#endif
    }

    QScriptValue QSize_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	int argc = context->argumentCount();
	int x = (argc>0) ? context->argument(0).toInt32() : 0;
	int y = (argc>1) ? context->argument(1).toInt32() : x;
	return engine->toScriptValue(QSize(x, y));
    }

    QScriptValue QColor_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	int argc = context->argumentCount();
	if( ! argc ) return engine->toScriptValue(QColor());
	if( 1 == argc )
	{
	    QScriptValue arg = context->argument(0);
	    if( arg.isNumber() )
	    {
		return engine->toScriptValue(QColor(arg.toInt32()));
	    }
	    else if( arg.isString() )
	    {
		return engine->toScriptValue(QColor(arg.toString()));
	    }
	    return QScriptValue();
	}
	else if( (3 == argc) || (4 == argc) )
	{
	    int arg = 0;
	    int r = context->argument(arg++).toInt32();
	    int g = context->argument(arg++).toInt32();
	    int b = context->argument(arg++).toInt32();
	    QColor col(r,g,b);
	    if( argc == 4 )
	    {
		col.setAlpha( context->argument(arg++).toInt32());
	    }
	    qDebug() <<"Creating QColor:"<<col;
	    return engine->toScriptValue(col);
	}
	return QScriptValue();
    }

    QScriptValue qgiToScriptValue(QScriptEngine *engine, QGraphicsItem* const &in)
    {
	QObject * obj = dynamic_cast<QObject*>(in);
	return obj
	    ? engine->newQObject(obj)
	    : QScriptValue();
    }
    
    void qgiFromScriptValue(const QScriptValue &object, QGraphicsItem* &out)
    {
	out = dynamic_cast<QGraphicsItem*>(object.toQObject());
    }

    QScriptValue qsizeToScriptValue(QScriptEngine *engine, const QSize &s)
    {
	QScriptValue obj = engine->newObject();
	obj.setProperty("width", QScriptValue(engine, s.width()));
	obj.setProperty("height", QScriptValue(engine, s.height()));
	return obj;
    }

    void qsizeFromScriptValue(const QScriptValue &obj, QSize &s)
    {
	s.setWidth( obj.property("width").toInt32() );
	s.setHeight( obj.property("height").toInt32() );
    }
   
    QScriptEngine * createScriptEngine( QObject * parent )
    {
	QScriptEngine * js = new QScriptEngine( parent );
	QScriptValue glob( js->globalObject() );

	qScriptRegisterMetaType(js, qgiToScriptValue, qgiFromScriptValue);

	glob.setProperty("QSize", js->newFunction(QSize_ctor));
// 	qScriptRegisterMetaType(js, qsizeToScriptValue, qsizeFromScriptValue);

	glob.setProperty("QColor", js->newFunction(QColor_ctor));

	glob.setProperty("QPoint", js->newFunction(QPoint_ctor));
 	/**
	   Damn... if i have both a QPoint ctor and to/fromScriptValue routines
	   then none of it works.
	*/
	//qScriptRegisterMetaType(js, qpointToScriptValue, qpointFromScriptValue);
	return js;
    }



    ScriptPacket::Impl::Impl() : code(),
				 name(),
				 js(0)
    {
    }
    ScriptPacket::Impl::~Impl()
    {
    }

    ScriptPacket::ScriptPacket( QScriptEngine * js,
				QString const & code,
				QString const & name )
	: QObject(),
	impl(new Impl)
    {
	impl->js = js;
	impl->code = code;
	impl->name = name;
    }

    ScriptPacket::ScriptPacket( ScriptPacket const & rhs )
	: QObject(),
	  impl(rhs.impl)
    {
    }
    ScriptPacket & ScriptPacket::operator=(ScriptPacket const & rhs )
    {
	if( this != &rhs )
	{
	    this->impl = rhs.impl;
	}
	return *this;
    }

    bool ScriptPacket::isValid( bool quickCheck ) const
    {
	if( quickCheck )
	{
	    return 0 != impl->js;
	}
	else
	{
	    return impl->js && impl->js->canEvaluate( impl->code );
	}
    }

    QScriptEngine * ScriptPacket::engine() const
    {
	return impl->js;
    }
    QScriptValue ScriptPacket::eval() const
    {
	return impl->js
	    ? impl->js->evaluate( impl->code,
				  impl->name.isEmpty()
				  ? "ScriptPacket"
				  : impl->name )
	    : QScriptValue();
    }

    QScriptValue ScriptPacket::operator()() const
    {
	return this->eval();
    }

    void ScriptPacket::setEngine( QScriptEngine * js )
    {
	impl->js = js;
    }
    void ScriptPacket::setCode( QString const & code )
    {
	impl->code = code;
    }

    void ScriptPacket::setScriptName( QString const & name )
    {
	impl->name = name;
    }

    ScriptPacket::~ScriptPacket()
    {
    }




} //namespace