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

#include <qboard/ScriptQt.h>
#include <qboard/QBoardView.h>
#include <qboard/utility.h>

#include <QApplication>
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSharedData>
#include <QIcon>
#include <QScriptValueIterator>
#include <QList>
#include <QMessageBox>
#include <QMetaType>
#include <QVariant>
#include <QFile>

#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QDateTime>

Q_DECLARE_METATYPE(QList<QGraphicsItem*>)
Q_DECLARE_METATYPE(QVariant)

#if 0
extern void com_trolltech_qt_core_ScriptPlugin(const QString &key, QScriptEngine *engine);
extern void com_trolltech_qt_gui_ScriptPlugin(const QString &key, QScriptEngine *engine);
// extern void com_trolltech_qt_network_ScriptPlugin(const QString &key, QScriptEngine *engine);
extern void com_trolltech_qt_uitools_ScriptPlugin(const QString &key, QScriptEngine *engine);
#endif

/**
   An unfortunate kludge, this is used by QList<QScriptValue>::find().

   We could use std::set<QScriptValue,CustomLessFtor>, but we have no
   sensible way to perform < on QScriptValue objects.
*/
bool operator==(QScriptValue const & lhs, QScriptValue const & rhs)
{
    return lhs.strictlyEquals(rhs);
}

#include <set> // QList doesn't support a custom compare functor

namespace qboard {

    QString to_source_f<QPoint>::operator()( QPoint const & x ) const
    {
	return QString("QPoint(%1,%2)").arg(x.x()).arg(x.y());
    }

    QString to_source_f<QPointF>::operator()( QPointF const & x ) const
    {
	return QString("QPointF(%1,%2)").arg(x.x()).arg(x.y());
    }
    QString to_source_f<QSize>::operator()( QSize const & x ) const
    {
	return QString("QSize(%1,%2)").arg(x.width()).arg(x.height());
    }
    QString to_source_f<QSizeF>::operator()( QSizeF const & x ) const
    {
	return QString("QSizeF(%1,%2)").arg(x.width()).arg(x.height());
    }


    QString to_source_f<QRectF>::operator()( QRectF const & x ) const
    {
	return QString("QRectF(%1,%2,%3,%4)").
	    arg(x.left()).
	    arg(x.top()).
	    arg(x.width()).
	    arg(x.height());
    }

    QString to_source_f<QRect>::operator()( QRect const & x ) const
    {
	return QString("QRect(%1,%2,%3,%4)").
	    arg(x.left()).
	    arg(x.top()).
	    arg(x.width()).
	    arg(x.height());
    }

    QString to_source_f<QColor>::operator()( QColor const & x ) const
    {
	return QString("QColor(%1,%2,%3,%4)").
	    arg(x.red()).
	    arg(x.green()).
	    arg(x.blue()).
	    arg(x.alphaF());
    }


    QString to_source_f<QString>::operator()( QString const & x ) const
    {
	bool hasSQ = x.contains('\'');
	if( ! hasSQ ) return QString("'%1'").arg(x);
	bool hasDQ = x.contains('"');
	if( ! hasDQ ) return QString("\"%1\"").arg(x);
	QString y(x);
	return QString("\"%1\"").arg(
				     y.replace('"',"\\\"") );
    }

    QString to_source_f<QVariant>::operator()( QVariant const & x ) const
    {
	switch( x.userType() )
	{
#define DO(VT,T) \
	    case QVariant::VT: \
		return qboard::toSource( x.value<T>() ); break;

	    DO(Int,int);
	    DO(Double,int);
	    DO(String,QString);
	    DO(Point,QPoint);
	    DO(PointF,QPointF);
	    DO(Rect,QRect);
	    DO(RectF,QRectF);
	    DO(Size,QSize);
	    DO(SizeF,QSizeF);
	    DO(Color,QColor);
	  default:
	      break;
#undef DO
	};
	return QString("undefined");
    }

//     struct ScriptValLess
//     {
// 	mutable QScriptValue func;
// 	explicit ScriptValLess( QScriptEngine * e )
// 	{
// 	    func = e
// 		? e->evaluate("function(l,r){return l<r;}")
// 		: QScriptValue();
// 	}
// 	bool operator()(QScriptValue const & lhs, QScriptValue const & rhs) const
// 	{
// 	    return func.call( QScriptValue(),
// 			      QScriptValueList()
// 			      << lhs << rhs ).toBoolean();
// 	}
//     };

    //typedef std::set<QScriptValue,ScriptValLess> ScriptValList;
    typedef QList<QScriptValue> ScriptValList;
    static ScriptValList & scriptValList()
    {
	static ScriptValList bob;
	return bob;
    }

    QString to_source_f_object::operator()( QScriptValue const & x ) const
    {
	if( ! x.isObject() ) return QString("undefined"); // should we return an empty string?
	if( x.isNull() ) return QString("null");
	if( x.isFunction() ) return x.toString(); // QString("('[toSource() cannot handle functions]',null)");
	if( scriptValList().contains(x) )
	{
	    return QString("('[toSource() skipping circular object reference!]',null)");
	}
	scriptValList().append(x);
	QScriptValueIterator it( x );
	QByteArray ba;
	QBuffer buf(&ba);
	// Note that we use QBuffer, instead of QDataStream because
	// writing to a QDataStream serializes in a custom binary format. :(
	// FIXME: replace this with QTextStream.
	buf.open(QIODevice::WriteOnly);
	bool isAr = x.isArray();
	char const * opener = (isAr ? "[" : "{");
	char const * closer = (isAr ? "]" : "}");
	char const * sep = ",";
	buf.write(opener);
	while( it.hasNext() )
	{
	    it.next();
	    if(0) qDebug() << "to_source_object_f: child:"<<it.name();
	    QString sub;
	    if( isAr )
	    {
		sub = toSource( it.value() );
	    }
	    else
	    {
		sub = QString("%1:%2").
		    arg(it.name()).
		    arg( toSource( it.value() ) );
	    }
	    buf.write( sub.toAscii().constData(), sub.size() );
	    if( it.hasNext() ) buf.write(sep);
	}
	buf.write(closer);
	buf.close();
	scriptValList().removeAll(x);
	QString ret(ba);
	return ret;
    }


    QString to_source_f<QScriptValue>::operator()( QScriptValue const & x ) const
    {
	if( x.isUndefined() ) return "undefined";
	if( x.isNull() ) return "null";
	if(0) qDebug() << "to_source_f<QScriptValue>("<<x.toString()<<")";

#define TODO(A,B,C)
#define DO(IS,T,TO) \
	if( x.IS() ) {\
	    if(0) qDebug() << "to_source_f<QScriptValue>(is-a: "<<# IS<<")"; \
	    return toSource<T>( x.TO() );\
	}


	DO(isVariant,QVariant,toVariant); // must come before the others!
	DO(isBoolean,bool,toBoolean);
	DO(isNumber,qreal,toNumber);
	DO(isString,QString,toString);
	TODO(isRegExp,QRegExp,toRegExp);
	if( x.isArray() || x.isObject() )
	{
	    if(0) qDebug() << "to_source_f<QScriptValue>(is-a: array or object)";
	    return to_source_f_object()( x );
	}

#undef DO
#undef TODO
	return QString("undefined");
    }


    /**
       Internal helper to get proper numeric types for some algos.
    */
    template <typename PT>
    struct point_valtype
    {
	typedef qreal value_type;
    };

    template <>
    struct point_valtype<QPoint>
    {
	typedef int value_type;
    };
    template <>
    struct point_valtype<QSize>
    {
	typedef int value_type;
    };
    template <>
    struct point_valtype<QRect>
    {
	typedef int value_type;
    };

    template <typename PT>
    QScriptValue convert_script_value_f_point<PT>::operator()(QScriptEngine*eng,
							   const PT & src) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("x", QScriptValue(eng, src.x()));
	obj.setProperty("y", QScriptValue(eng, src.y()));
	return obj;
    }

    /**
       Accepts arguments as follows:

       If args is not an array then it must be an object with numeric
       x/y properties.

       If args is an array, IF the first element is an object then
       that object is treated as described above. Otherwise there must
       be at least 2 numeric arguments, which are used as x/y values.

       So, in JS that's:

       PT(x,y)

       PT({x:N,y:N})

    */
    template <typename PT>
    PT convert_script_value_f_point<PT>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	typedef typename point_valtype<PT>::value_type value_type;
	value_type x = 0;
	value_type y = 0;
	QScriptValue obj;
	bool smellArray = (args.isArray() || ! args.property("length").isUndefined());
	if( smellArray )
	{
	    if(0) qDebug() << "Looks like arguments array.";
	    obj = args.property(0);
	}
	else if( args.isObject() )
	{
	    if(0) qDebug() << "Looks like an object.";
	    obj = args;
	}

	if( smellArray && !obj.isObject() )
	{
	    if(0) qDebug() << "Trying arguments array.";
	    x = value_type(args.property(0).toNumber());
	    y = value_type(args.property(1).toNumber());
	}
	else
	{
	    if(0) qDebug() << "Trying object x/y:" << obj.toString() << ":" << toSource( obj );
	    if(0) qDebug() << "obj.property(x).toNumber():"<<obj.property("x").toNumber();
	    x = value_type(obj.property("x").toNumber());
	    y = value_type(obj.property("y").toNumber());
	}
	if(0) qDebug() << "PT:"<<PT(x,y);
	return PT(x,y);
    }

    /**
       See convert_script_value_f_point<PT>.
    */
    template <typename PT>
    QScriptValue QPoint_ctor(QScriptContext *ctx, QScriptEngine *eng)
    {
	PT pt = fromScriptValue<PT>( eng, ctx->argumentsObject() );
	return eng->toScriptValue( pt );
    }


    template <typename RT>
    QScriptValue convert_script_value_f_rect<RT>::operator()(QScriptEngine*eng,
							   const RT & s) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("top", QScriptValue(eng, s.left()));
	obj.setProperty("left", QScriptValue(eng, s.top()));
	obj.setProperty("width", QScriptValue(eng, s.width()));
	obj.setProperty("height", QScriptValue(eng, s.height()));
	return obj;
    }

    /**
       Converts JS arguments to a rectangle type RT. Arguments
       must be compatible with one of:

       RT(number,number,number,number)

       RT({left:N,top:N,width:N,height:N})

    */
    template <typename RT>
    RT convert_script_value_f_rect<RT>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	typedef typename point_valtype<RT>::value_type value_type;
	value_type l = 0;
	value_type t = 0;
	value_type w = 0;
	value_type h = 0;
	QScriptValue obj;
	bool smellArray = (args.isArray() ||
			   ! args.property("length").isUndefined());
	if( smellArray )
	{
	    obj = args.property(0);
	}
	else if( args.isObject() )
	{
	    obj = args;
	}

	if( smellArray && !obj.isObject() )
	{
#define ARG(X) value_type(args.property(X).toNumber())
	    l = ARG(0);
	    t = ARG(1);
	    w = ARG(2);
	    h = ARG(3);
#undef ARG
	}
	else
	{
#define ARG(X) value_type(obj.property(X).toNumber())
	    l = ARG("left");
	    t = ARG("top");
	    w = ARG("width");
	    h = ARG("height");
#undef ARG
	}
	return RT( l, t, w, h );
    }

    template <typename RT>
    QScriptValue QRect_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	RT r = fromScriptValue<RT>( engine, context->argumentsObject() );
	return engine->toScriptValue( r );
    }

    template <typename ST>
    QScriptValue convert_script_value_f_size<ST>::operator()(QScriptEngine*eng,
							   const ST & s) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("width", QScriptValue(eng, s.width()));
	obj.setProperty("height", QScriptValue(eng, s.height()));
	return obj;
    }

    /**
       Converts JS arguments to a Size type RT. Arguments
       must be compatible with one of:

       ST(number,number)

       ST({width:N,height:N})
    */
    template <typename ST>
    ST convert_script_value_f_size<ST>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	typedef typename point_valtype<ST>::value_type value_type;
	value_type w = 0;
	value_type h = 0;
	QScriptValue obj;
	bool smellArray = ( args.isArray() ||
			    ! args.property("length").isUndefined() );
	if( smellArray )
	{
	    obj = args.property(0);
	}
	else if( args.isObject() )
	{
	    obj = args;
	}
	if( smellArray && !obj.isObject() )
	{
#define ARG(X) value_type(args.property(X).toNumber())
	    w = ARG(0);
	    h = ARG(1);
#undef ARG
	}
	else
	{
#define ARG(X) value_type(obj.property(X).toNumber())
	    w = ARG("width");
	    h = ARG("height");
#undef ARG
	}
	return ST(w,h);
    }

    template <typename ST>
    QScriptValue QSize_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	ScriptArgv av(context);
	int argc = av.argc();
	int x = (argc>0) ? av[0].toInt32() : 0;
	int y = (argc>1) ? av[1].toInt32() : x;
	return engine->toScriptValue( ST(x,y) );
    }


    QScriptValue convert_script_value_f<QColor>::operator()(QScriptEngine*eng,
							  const QColor & col) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("red", QScriptValue(eng, col.red()));
	obj.setProperty("green", QScriptValue(eng, col.green()));
	obj.setProperty("blue", QScriptValue(eng, col.blue()));
	obj.setProperty("alpha", QScriptValue(eng, col.alpha()));
	return obj;
    }

    QColor convert_script_value_f<QColor>::operator()( QScriptEngine *,
						     const QScriptValue & args ) const
    {
	int r = 0;
	int g = 0;
	int b = 0;
	qreal a = 255;
	QScriptValue obj;
	bool smellArray = ( args.isArray() ||
			    ! args.property("length").isUndefined() );
	if( smellArray )
	{
	    obj = args.property(0);
	}
	else if( args.isObject() )
	{
	    obj = args;
	}

	if( smellArray && !obj.isObject() )
	{
#define ARG(X) args.property(X).toInt32()
	    int argc = args.property("length").toInt32();
	    QScriptValue arg = args.property(0);
	    if( (1 == argc) && arg.isString() )
	    {
		return QColor( arg.toString() );
	    }
	    r = (argc > 0) ? ARG(0) : 0;
	    g = (argc > 1) ? ARG(1) : 0;
	    b = (argc > 2) ? ARG(2) : 0;
	    a = (argc > 3)
		? args.property(3).toNumber()
		: 255.0;
#undef ARG
	}
	else
	{
#define ARG(X) obj.property(X).toInt32()
	    r = ARG("red");
	    g = ARG("green");
	    b = ARG("blue");
#undef ARG
	    QScriptValue av( obj.property("alpha") );
	    if( ! av.isUndefined() )
	    {
		a = av.toNumber();
	    }
	}
	QColor c( r, g, b );
	if( a <= 1.0 ) c.setAlphaF(a);
	else c.setAlpha( int(a) );
	return c;
    }

    QScriptValue QColor_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	ScriptArgv av(context);
	int argc = av.argc();
	if( ! argc ) return engine->toScriptValue(QColor());
	QColor col = fromScriptValue<QColor>(engine, context->argumentsObject() );
	return engine->toScriptValue( col );
    }


    QScriptValue convert_script_value_f<QGraphicsItem*>::operator()(QScriptEngine *eng,
								  QGraphicsItem * const & in) const
    {
	QObject * obj = dynamic_cast<QObject*>(in);
	return obj
	    ? eng->newQObject(obj)
	    : QScriptValue();
    }

    QGraphicsItem* convert_script_value_f<QGraphicsItem*>::operator()( QScriptEngine *,
								     const QScriptValue & args ) const
    {
	return dynamic_cast<QGraphicsItem*>(args.toQObject());
    }

    QScriptValue convert_script_value_f<QGraphicsScene*>::operator()(QScriptEngine *eng,
								  QGraphicsScene * const & in) const
    {
	QObject * obj = dynamic_cast<QObject*>(in);
	return obj
	    ? eng->newQObject(obj)
	    : QScriptValue();
    }

    QGraphicsScene* convert_script_value_f<QGraphicsScene*>::operator()( QScriptEngine *,
								     const QScriptValue & args ) const
    {
	return dynamic_cast<QGraphicsScene*>(args.toQObject());
    }
    

    QScriptValue convert_script_value_f<QList<QGraphicsItem*> >::
    operator()(QScriptEngine *eng,
	       QList<QGraphicsItem *> const & in) const
    {
	QScriptValue ar = eng->newArray();
	unsigned int ndx = 0;
	foreach( QGraphicsItem * gi, in )
	{
	    QScriptValue o = toScriptValue(eng,gi);
	    if( o.isObject() )
	    {
		ar.setProperty( ndx++, o );
	    }
	}
	return ar;
    }

    QList<QGraphicsItem*>
    convert_script_value_f<QList<QGraphicsItem*> >::
    operator()( QScriptEngine *,
		const QScriptValue & args ) const
    {
	QList<QGraphicsItem*> out;
	if( ! args.isObject() ) return out;
	QScriptValueIterator it( args );
	while( it.hasNext() )
	{
	    it.next();
	    QGraphicsItem * qgi =
		fromScriptValue<QGraphicsItem*>(args.engine(),it.value());
	    if( ! qgi ) continue;
	    out.push_back( qgi );
	}
	return out;
    }



#if 0
    QScriptValue qvariantToScriptValue(QScriptEngine *engine,
				       const QVariant &v)
    {
	if(0) qDebug() << "qvariantToScriptValue(engine,"<<v<<")";
	if( ! v.isValid() ) return engine->nullValue();
#define CONV(T) if( v.canConvert<T>() ) return engine->toScriptValue<T>(v.value<T>());
	CONV(QPointF);
	CONV(QPoint);
	CONV(QSize);
	CONV(QGraphicsItem*);
	CONV(QGraphicsScene*);
#undef CONV
#define CONV(T,X) if( v.canConvert<T>() ) return QScriptValue(engine,X);
	CONV(qreal,v.toDouble());
	CONV(int,v.toInt());
#undef CONV
	return QScriptValue(engine,v.toString());
    }

    void qvariantFromScriptValue(const QScriptValue &obj, QVariant &v)
    {
	if(0) qDebug() << "qvariantFromScriptValue(engine,"<<v<<")";
	if( obj.isUndefined() || obj.isNull() ) v = QVariant();
	else if( obj.isVariant() ) v = obj.toVariant();
	else if( obj.isString() ) v = QVariant(obj.toString());
	else if( obj.isBoolean() ) v = QVariant(1);
	else if( obj.isNumber() ) v = QVariant(obj.toNumber());
	else if( obj.isRegExp() ) v = QVariant(obj.toRegExp());
	return;
    }
#endif
    /**
       JS usage:

       alert("Hi!");

       Shows a dialog box with a single OK button. Returns an undefined
       value.
    */
    static QScriptValue js_alert(QScriptContext *ctx, QScriptEngine *eng)
    {
	QMessageBox::warning( 0, "JavaScript alert()",
			      ctx->argument(0).toString(),
			      QMessageBox::Ok, QMessageBox::Ok );
	return eng->undefinedValue();
    }

    /**
       JS usage:

       var x = confirm("Really do .... ???");

       Shows a dialog box asking for confirmation and returns
       true if OK is selected, else false.
    */
    static QScriptValue js_confirm(QScriptContext *ctx, QScriptEngine *eng)
    {
	return QScriptValue(eng,
			    QMessageBox::Ok ==
			    QMessageBox::question( 0,
				   "JavaScript confirm()",
				   ctx->argument(0).toString(),
				   QMessageBox::Ok | QMessageBox::Cancel )
			    );
    }

    /**
       JS usage:

       toSource(value)

       will attempt to create JS source code for the given value. This
       currently has some significant limitations:

       - functions cannot be toSourced

       - Circular references in an object tree may cause this routine
       to loop forever. It should write object references, but it
       doesn't yet (and i'm not sure if QtScript supports them). And
       i've forgotten the JS syntax for them, anyway.
    */
    static QScriptValue js_toSource(QScriptContext *ctx, QScriptEngine *eng)
    {
	return ctx->argumentCount()
	    ? QScriptValue(eng, toSource( ctx->argument(0) ) )
	    : eng->nullValue();
    }


    /**
       JS usage:

       randomInt(n) selects a random integer in the range [0,n). e.g.
       randomInt(6) select a number between 0 and 5.

       randomInt(n,m) selects a number in the range [n,m]. e.g.
       randomInt(1,6) selects a number between 1 and 6.
    */
    static QScriptValue js_randomInt(QScriptContext *ctx, QScriptEngine *eng)
    {
	/**
	   To consider:

	   http://www.shawnolson.net/a/789/make-javascript-mathrandom-useful.html

	   http://thepenry.net/jsrandom.php

	   Math.floor(Math.random() * (max - min + 1) + min)

	   to get numbers between min and max, inclusive.
	*/

	ScriptArgv av(ctx);
	int low = 0;
	int high = 0;
	if( 1 == av.argc() )
	{
	    high = av().toInt32()-1;
	}
	else if( 2 == av.argc() )
	{
	    low = av[0].toInt32();
	    high = av[1].toInt32();
	}
	if( low == high ) return QScriptValue(eng,low);
	if( high < low )
	{
	    int x = high;
	    high = low;
	    low = x;
	}
	static bool seeded;
	if( ! seeded )
	{
	    seeded = true;
	    qsrand( uint(static_cast<void*>(ctx)) );
	}

#if 1
	int rc = ( qrand() % (high - low + 1) ) + low;
	return QScriptValue(eng, rc);
#else
	// Always gives me the same results????
	QString code = QString("Math.floor(Math.random() * (%1 - %2 + 1) + %3)").
	    arg(high).
	    arg(low).
	    arg(low);
	return eng->evaluate( code, "randomInt()" );
#endif
    }

    PathFinder & includePath()
    {
	static PathFinder bob;
	if( bob.empty() )
	{
	    bob.addPrefix( QString("%1/QBoard/js/").
			   arg(qboard::home().absolutePath()),
			   false );
	    bob.addSuffix( ".qs" );
	    bob.addSuffix( ".js" );
	}
	return bob;
    }

    QScriptValue jsInclude( QScriptEngine * eng,
			     QString const & _fn )
    {
	QString fn( includePath().find( _fn ) );
	QScriptContext * ctx = eng->currentContext();
	if( fn.isEmpty() )
	{
	    QString msg = QString("include() could not find file \"%1\"").arg(_fn);
	    return ctx->throwError(QScriptContext::URIError, msg );
	}
	QFile scriptFile(fn);
	if (!scriptFile.open(QIODevice::ReadOnly))
	{
	    QString msg = QString("include() could not open file \"%1\"").arg(fn);
	    return ctx->throwError(QScriptContext::URIError, msg );
	}
	QString contents;
	{
	    QTextStream stream(&scriptFile);
	    contents = stream.readAll();
	    scriptFile.close();
	}
	QScriptValue actObj( eng->globalObject() );
	QScriptValue oldFile = actObj.property("__FILE__");
	actObj.setProperty("__FILE__",QScriptValue(eng,fn));
	QScriptValue ret;
	try
	{
	    ret = eng->evaluate( contents, fn );
	}
	catch(std::exception const &ex)
	{
	    ret = ctx->throwError( QScriptContext::UnknownError,
				   QString("include() threw a native exception: %1").
				   arg(ex.what()) );
	}
	catch(...)
	{
	    ret = ctx->throwError( QScriptContext::UnknownError,
				   QString("include() threw an unknown native exception.") );
	}
	actObj.setProperty("__FILE__",oldFile);
	return ret;
    }


    static QScriptValue js_include2(QScriptContext *ctx, QScriptEngine *eng)
    {

	ScriptArgv av(ctx);
	if( ! av.argc() ) return ctx->throwError(QScriptContext::RangeError,
						 QString("include() cannot be called without parameters"));
	QScriptValue ret = eng->nullValue();
	while( av.isValid() )
	{
	    //qDebug() << "js_include2() trying arg #"<<av.at()<<"of"<<av.argc();
	    ret = jsInclude( eng, (av++).toString() );
	    if( ret.isError() || eng->hasUncaughtException() ) break;
	}
	return ret;
    }

    QScriptValue js_importExtension(QScriptContext *context, QScriptEngine *engine)
    {
	return engine->importExtension(context->argument(0).toString());
    }


    QScriptEngine * createScriptEngine( QObject * parent )
    {
	static bool inited = false;
	if( ! inited )
	{
	    inited = true;
	    QStringList paths = qApp->libraryPaths();
	    QDir qhome( qboard::home() );
	    paths <<  QString("%1/QBoard/plugins").arg(qhome.absolutePath());
	    qApp->setLibraryPaths(paths);
	}
	QScriptEngine * js = new QScriptEngine( parent );
	QScriptValue glob( js->globalObject() );

	{
	    QScriptValue qtj = js->newObject();
	    glob.setProperty("qt", qtj);
	    QScriptValue qappj = js->newQObject(QApplication::instance());
	    qtj.setProperty("app", qappj );
	    glob.setProperty("qApp", qappj );
	    QScriptValue qscript = js->newObject();
	    qscript.setProperty("importExtension", js->newFunction(js_importExtension));
	    qtj.setProperty("script", qscript);
	}

#if 1
	/**
	   HUGE KLUDGE while i try to import more official Qt bindings.
	   This mess instantiates certain templates which otherwise don't
	   get instantiated and therefor don't get put into our library. :(
	*/
	{
	    QScriptValue kludge;
#define INST(T) kludge = toScriptValue( js, T() ); T tmp##T( fromScriptValue<T>(js,QScriptValue(kludge)) );
	    INST(QPoint);
	    INST(QPointF);
	    INST(QRect);
	    INST(QRectF);
	    INST(QSize);
	    INST(QSizeF);
	    INST(QColor);
#undef INST
	}
#endif // instantiation kludge

	qScriptRegisterMetaType(js, 
				qboard::toScriptValue<QList<QGraphicsItem*> >,
				qboard::fromScriptValue<QList<QGraphicsItem*> > );
#if 1
	qScriptRegisterMetaType<QGraphicsItem*>(js,
						qboard::toScriptValue<QGraphicsItem*>,
						qboard::fromScriptValue<QGraphicsItem*> );
	qScriptRegisterMetaType(js,
				qboard::toScriptValue<QGraphicsScene*>,
				qboard::fromScriptValue<QGraphicsScene*> );

	if(1)
	{
	    JSVariantPrototype * proto = new JSVariantPrototype(js);
	    // can't seem to tap in to the core QVariant prototypes... :(
	    // js->setDefaultPrototype(qMetaTypeId<QVariant>(), js->newQObject(proto));
	    glob.setProperty("JSVariantPrototype",
			     js->newQObject(proto),
			     QScriptValue::ReadOnly | QScriptValue::Undeletable );
	}

	glob.setProperty("QColor", js->newFunction(QColor_ctor));
	glob.setProperty("QRect", js->newFunction(QRect_ctor<QRect>));
	glob.setProperty("QRectF", js->newFunction(QRect_ctor<QRectF>));
	glob.setProperty("QSize", js->newFunction(QSize_ctor<QSize>));
	glob.setProperty("QSizeF", js->newFunction(QSize_ctor<QSizeF>));
	glob.setProperty("QPoint", js->newFunction(QPoint_ctor<QPoint>));
	glob.setProperty("QPointF", js->newFunction(QPoint_ctor<QPointF>));
#endif

	glob.setProperty("alert",
			 js->newFunction(js_alert),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("confirm",
			 js->newFunction(js_confirm),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("include",
			 js->newFunction(js_include2),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("randomInt",
			 js->newFunction(js_randomInt),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("toSource",
			 js->newFunction(js_toSource),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );

	if(0)
	{
	    QString res( ":/QBoard/javascript/ClassicalInheritance.inc.js" );
	    QFile inf(res);
	    if( inf.open(QIODevice::ReadOnly) )
	    {
		js->evaluate( inf.readAll(), res );
	    }
	    inf.close();
	}

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
				QString const & name,
				QObject * parent )
	: QObject(parent),
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

    struct JavaScriptAction::Impl
    {
	QScriptEngine * js;
	QString code;
	Impl( QScriptEngine * j, QString const & c )
	    : js(j),
	      code(c)
	{
	}
	~Impl()
	{
	}
    };


    JavaScriptAction::JavaScriptAction( QString const & label,
					QScriptEngine & e,
					QString const & code )
	: QAction( label, &e ),
	  impl( new Impl(&e,code) )
    {
	connect( this, SIGNAL(triggered(bool)),
		 this, SLOT(evaluateJS()) );
	this->setCheckable(false);
	this->setIcon(QIcon(":/QBoard/icon/fileext/js.png"));
    }

    JavaScriptAction::~JavaScriptAction()
    {
	delete this->impl;
    }

    void JavaScriptAction::evaluateJS()
    {
	impl->js->evaluate( impl->code,
			    this->objectName() );
    }

    ScriptArgv::ScriptArgv( QScriptContext * cx )
	: m_cx(cx),
	  m_at(0),
	  m_max(cx ? cx->argumentCount() : -1)
    {

    }

    QScriptValue ScriptArgv::value() const
    {
	return m_cx
	    ? m_cx->argument(m_at)
	    : QScriptValue();
    }
    ScriptArgv::operator QScriptValue() const
    {
	return this->value();
    }

    int ScriptArgv::at() const
    {
	return m_at;
    }

    QScriptValue ScriptArgv::operator()() const
    {
	return this->value();
    }

    QScriptValue ScriptArgv::operator()( int ndx ) const
    {
	return this->operator[]( ndx );
    }

    QScriptValue ScriptArgv::operator++()
    {
	return ( m_cx && (m_at < (m_max-1)) )
	    ? m_cx->argument(++m_at)
	    : QScriptValue();
    }

    QScriptValue ScriptArgv::operator++(int)
    {
	return ( m_cx && (m_at < m_max) )
	    ? m_cx->argument(m_at++)
	    : QScriptValue();
    }

    QScriptValue ScriptArgv::operator[](int ndx) const
    {
	return (m_cx && (m_max>=0) && (ndx>=0) && (ndx<=m_max) )
	    ? m_cx->argument(ndx)
	    : QScriptValue();
    }

    int ScriptArgv::argc() const
    {
	return m_max;
    }

    void ScriptArgv::reset()
    {
	m_at = 0;
    }

    bool ScriptArgv::isValid() const
    {
	return m_cx && (m_max>=0) && (m_at<m_max);
    }


#define SELF(RV) \
    QVariant self( qscriptvalue_cast<QVariant>(thisObject()));	\
	QScriptEngine * js = this->engine();			\
	if( !js) return RV;

    struct JSVariantPrototype::Impl
    {
	Impl()
	{
	}
	~Impl()
	{
	}
    };

    JSVariantPrototype::JSVariantPrototype( QObject * parent )
	: QObject(parent),
	  QScriptable(),
	  impl(0)
    {
    }

    JSVariantPrototype::~JSVariantPrototype()
    {
	delete impl;
    }

    void JSVariantPrototype::foo()
    {
	SELF();
	if(1) qDebug() << "JSVariantPrototype::foo() val="<<self;
    }

    QString JSVariantPrototype::toSource()
    {
	SELF(QString("undefined"));
	return qboard::toSource<QVariant>( self );
    }

    QString JSVariantPrototype::toVariantString()
    {
	SELF(QString("[invalid object]"));
	QString ret;
#if 1
	QByteArray ba;
	QBuffer buf(&ba);
	buf.open(QIODevice::WriteOnly);
	QDebug out(&buf);
	out << self;
	buf.close();
	ret = QString(ba); // ba.data() ? ba.data() : "??WTF??";
#else
  	ret = QString("QVariant(Type=%1)").
  	    arg(self.type());
#endif
	return ret;
    }

    int JSVariantPrototype::variantType()
    {
	SELF(QVariant::Invalid);
	return self.userType();
    }
    QScriptValue JSVariantPrototype::jsValue()
    {
	SELF(QScriptValue());
	if( ! self.isValid() ) return js->nullValue();
	QScriptValue obj( js->nullValue() );
	switch( self.userType() )
	{
#define POD(T,F) case QVariant::T: obj = QScriptValue(js,self.F() ); break
	    POD(Bool,toBool);
	    POD(Color,toString);
	    POD(Double,toDouble);
	    POD(Int,toInt);
	    POD(String,toString);
	  case QVariant::DateTime:
	      obj = js->newDate( self.value<QDateTime>() );
	      break;
	  case QVariant::Point:
	      obj = qboard::toScriptValue( js, self.value<QPoint>() );
	      break;
	  case QVariant::PointF:
	      obj = qboard::toScriptValue( js, self.value<QPointF>() );
	      break;
	  case QVariant::Rect:
	      obj = qboard::toScriptValue( js, self.value<QRect>() );
	      break;
	  case QVariant::RectF:
	      obj = qboard::toScriptValue( js, self.value<QRectF>() );
	      break;
	  case QVariant::RegExp:
	      obj = js->newRegExp( self.value<QRegExp>() );
	      break;
	  case QVariant::Size:
	      obj = qboard::toScriptValue( js, self.value<QSize>() );
	      break;
	  case QVariant::SizeF:
	      obj = qboard::toScriptValue( js, self.value<QSizeF>() );
	      break;
	  default:
	      break;
	};
	return obj;
    }

#undef SELF

} //namespace
