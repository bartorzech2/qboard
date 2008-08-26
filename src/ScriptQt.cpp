#include "ScriptQt.h"
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QDebug>
#include <QGraphicsItem>

namespace qboard {

    QScriptValue QPoint_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	int argc = context->argumentCount();
	int x = (argc>0) ? context->argument(0).toInt32() : 0;
	int y = (argc>1) ? context->argument(1).toInt32() : x;
	return engine->toScriptValue(QPoint(x, y));
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
    
    QScriptEngine * createScriptEngine( QObject * parent )
    {
	QScriptEngine * js = new QScriptEngine( parent );
	QScriptValue glob( js->globalObject() );
	glob.setProperty("QPoint", js->newFunction(QPoint_ctor));
	glob.setProperty("QSize", js->newFunction(QSize_ctor));
	glob.setProperty("QColor", js->newFunction(QColor_ctor));
	qScriptRegisterMetaType(js, qgiToScriptValue, qgiFromScriptValue);
	return js;
    }

} //namespace
