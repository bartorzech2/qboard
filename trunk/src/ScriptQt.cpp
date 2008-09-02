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
#include <QGraphicsScene>
#include <QSharedData>
#include <QIcon>
#include <QScriptValueIterator>
#include <QList>
#include <QMessageBox>
#include <QMetaType>
#include <QVariant>

#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QDateTime>

Q_DECLARE_METATYPE(QList<QGraphicsItem*>)
Q_DECLARE_METATYPE(QVariant)

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
	    case QVariant::Color:
		return qboard::toSource( x.value<QString>() ); break;
	  default:
	      break;
#undef DO
	};
	return QString("undefined");
    }

    struct to_source_f_object
    {
	QString operator()( QScriptValue const & x ) const
	{
	    if( ! x.isObject() ) return QString("undefined"); // should we return an empty string?
	    QScriptValueIterator it( x );
	    QByteArray ba;
 	    QBuffer buf(&ba);
	    buf.open(QIODevice::WriteOnly);
	    bool isAr = x.isArray();
	    char const * opener = (isAr ? "[" : "{");
	    char const * closer = (isAr ? "]" : "}");
	    char const * sep = ",";
	    //str << opener;
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
	    QString ret(ba);
	    if(0) qDebug() << "to_source_f_object() returning:"<<ret
			   << "\nbytecount="<<ba.count();
	    return ret;
	}
    };


    QString to_source_f<QScriptValue>::operator()( QScriptValue const & x ) const
    {
	if( x.isUndefined() ) return "undefined";
	if( x.isNull() ) return "null";

#define TODO(A,B,C)
#define DO(IS,T,TO) \
	if( x.IS() ) return toSource<T>( x.TO() );

	DO(isVariant,QVariant,toVariant); // must come before the others!
	DO(isBoolean,bool,toBoolean);
	DO(isNumber,qreal,toNumber);
	DO(isString,QString,toString);
	TODO(isRegExp,QRegExp,toRegExp);
	if( x.isArray() || x.isObject() )
	{
	    return to_source_f_object()( x );
	}

#undef DO
#undef TODO
	return QString("undefined");
    }


    template <typename PT>
    QScriptValue convert_script_value_point<PT>::operator()(QScriptEngine*eng,
							   const PT & src) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("x", QScriptValue(eng, src.x()));
	obj.setProperty("y", QScriptValue(eng, src.y()));
	return obj;
    }

    template <typename PT>
    PT convert_script_value_point<PT>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	qreal x = 0;
	qreal y = 0;
	if( args.isArray() ||
	    ! args.property("length").isUndefined() )
	{
	    if(0) qDebug() << "qpointFromScriptValue(): args smells like an array";
	    x = args.property(0).toNumber();
	    y = args.property(1).toNumber();
	}
	else
	{
	    if(0) qDebug() << "qpointFromScriptValue(): args doesn't smell like an array";
	    x = args.property("x").toNumber();
	    y = args.property("y").toNumber();
	}
	return PT(x,y);
    }

//     template <typename PT>
//     QScriptValue qpointToScriptValue(QScriptEngine *engine, const PT &s)
//     {
// 	if(0) qDebug() << "qpointfToScriptValue(engine,"<<s<<")";
// 	QScriptValue obj = engine->newObject();
// 	obj.setProperty("x", QScriptValue(engine, s.x()));
// 	obj.setProperty("y", QScriptValue(engine, s.y()));
// 	return obj;
//     }

//     template <typename PT>
//     void qpointFromScriptValue(const QScriptValue &args, PT &s)
//     {
// 	if(0) qDebug() << "qpointFromScriptValue(): args count: " << args.property("length").toInt32()
// 		 << ", tosource="<<toSource(args);
// 	qreal x = 0;
// 	qreal y = 0;
// 	if( args.isArray() ||
// 	    ! args.property("length").isUndefined() )
// 	{
// 	    if(0) qDebug() << "qpointFromScriptValue(): args smells like an array";
// 	    x = args.property(0).toNumber();
// 	    y = args.property(1).toNumber();
// 	}
// 	else
// 	{
// 	    if(0) qDebug() << "qpointFromScriptValue(): args doesn't smell like an array";
// 	    x = args.property("x").toNumber();
// 	    y = args.property("y").toNumber();
// 	}
// 	s = PT(x,y);
// 	if(0) qDebug() << "qpointFromScriptValue("<<toSource( args )<<") ="<<s;
//     }

    template <typename PT>
    QScriptValue QPoint_ctor(QScriptContext *ctx, QScriptEngine *eng)
    {
	PT pt = fromScriptValue<PT>( eng, ctx->argumentsObject() );
	return eng->toScriptValue( pt );
    }


    template <typename RT>
    QScriptValue convert_script_value_rect<RT>::operator()(QScriptEngine*eng,
							   const RT & s) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("top", QScriptValue(eng, s.left()));
	obj.setProperty("left", QScriptValue(eng, s.top()));
	obj.setProperty("width", QScriptValue(eng, s.width()));
	obj.setProperty("height", QScriptValue(eng, s.height()));
	return obj;
    }

    template <typename RT>
    RT convert_script_value_rect<RT>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	qreal l = 0;
	qreal t = 0;
	qreal w = 0;
	qreal h = 0;
#define ARG(X) args.property(X).toNumber()
	if( args.isArray() ||
	    ! args.property("length").isUndefined() )
	{
	    l = ARG(0);
	    t = ARG(1);
	    w = ARG(2);
	    h = ARG(3);
	}
	else
	{
	    l = ARG("left");
	    t = ARG("top");
	    w = ARG("width");
	    h = ARG("height");
	}
#undef ARG
	return RT( l,t,w,h);
    }

    template <typename RT>
    QScriptValue QRect_ctor(QScriptContext *context, QScriptEngine *engine)
    {
	RT r = fromScriptValue<RT>( engine, context->argumentsObject() );
	return engine->toScriptValue( r );
    }

    template <typename ST>
    QScriptValue convert_script_value_size<ST>::operator()(QScriptEngine*eng,
							   const ST & s) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("width", QScriptValue(eng, s.width()));
	obj.setProperty("height", QScriptValue(eng, s.height()));
	return obj;
    }

    template <typename ST>
    ST convert_script_value_size<ST>::operator()( QScriptEngine *,
						  const QScriptValue & args ) const
    {
	qreal w = 0;
	qreal h = 0;
#define ARG(X) args.property(X).toNumber()
	if( args.isArray() ||
	    ! args.property("length").isUndefined() )
	{
	    w = ARG(0);
	    h = ARG(1);
	}
	else
	{
	    w = ARG("width");
	    h = ARG("height");
	}
#undef ARG
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


    QScriptValue convert_script_value<QColor>::operator()(QScriptEngine*eng,
							  const QColor & col) const
    {
	QScriptValue obj = eng->newObject();
	obj.setProperty("red", QScriptValue(eng, col.red()));
	obj.setProperty("green", QScriptValue(eng, col.green()));
	obj.setProperty("blue", QScriptValue(eng, col.blue()));
	obj.setProperty("alpha", QScriptValue(eng, col.alpha()));
	return obj;
    }

    QColor convert_script_value<QColor>::operator()( QScriptEngine *,
						     const QScriptValue & args ) const
    {
	int r = 0;
	int g = 0;
	int b = 0;
	qreal a = 255;
#define ARG(X) args.property(X).toInt32()
	if( args.isArray() ||
	    ! args.property("length").isUndefined() )
	{
	    int argc = args.property("length").toInt32();
	    if( 1 == argc )
	    {
		QScriptValue arg = args.property(0);
		return arg.isString()
		    ? QColor( arg.toString() )
		    : QColor( arg.toInt32() );
	    }
	    r = (argc > 0) ? ARG(0) : 0;
	    g = (argc > 1) ? ARG(1) : 0;
	    b = (argc > 2) ? ARG(2) : 0;
	    a = (argc > 3)
		? args.property(3).toNumber()
		: 255.0;
	}
	else
	{
	    r = ARG("red");
	    g = ARG("green");
	    b = ARG("blue");
	    QScriptValue av( args.property("alpha") );
	    if( ! av.isUndefined() )
	    {
		a = av.toNumber();
	    }
	}
#undef ARG
	QColor c( r, g, b );
	if( a < 1.0 ) c.setAlphaF(a);
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

    QScriptValue qgsToScriptValue(QScriptEngine *engine, QGraphicsScene* const &in)
    {
	QObject * obj = dynamic_cast<QObject*>(in);
	return obj
	    ? engine->newQObject(obj)
	    : engine->nullValue();
    }
    
    void qgsFromScriptValue(const QScriptValue &object, QGraphicsScene* &out)
    {
	out = dynamic_cast<QGraphicsScene*>(object.toQObject());
    }

    typedef QList<QGraphicsItem*> QGIList;
    QScriptValue qgilistToScriptValue(QScriptEngine *js, QGIList const &in)
    {
	QScriptValue ar = js->newArray();
	unsigned int ndx = 0;
	foreach( QGraphicsItem * gi, in )
	{
	    QScriptValue o = qgiToScriptValue(js,gi);
	    if( o.isObject() )
	    {
		ar.setProperty( ndx++, o );
	    }
	}
	return ar;
    }

    void qgilistFromScriptValue(const QScriptValue &jso, QGIList &out)
    {
	if( ! jso.isObject() ) return;
	QScriptValueIterator it( jso );
	while( it.hasNext() )
	{
	    it.next();
	    QGraphicsItem * qgi = 0;
	    qgiFromScriptValue( it.value(), qgi );
	    if( ! qgi ) continue;
	    out.push_back( qgi );
	}
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

	int rc = ( qrand() % (high - low + 1) ) + low;
	return QScriptValue(eng, rc);
    }


    QScriptEngine * createScriptEngine( QObject * parent )
    {
	QScriptEngine * js = new QScriptEngine( parent );
	QScriptValue glob( js->globalObject() );

	qScriptRegisterMetaType(js, qgiToScriptValue, qgiFromScriptValue);
	qScriptRegisterMetaType(js, qgsToScriptValue, qgsFromScriptValue);
	qScriptRegisterMetaType(js, qgilistToScriptValue, qgilistFromScriptValue);

	if(0)
	{
	    // can't seem to tap in to the core QVariant prototypes... :(
	    JSVariantPrototype * proto = new JSVariantPrototype(js);
	    js->setDefaultPrototype(qMetaTypeId<QVariant>(),
				    js->newQObject(proto));
	}

	glob.setProperty("QColor", js->newFunction(QColor_ctor));
	glob.setProperty("QRect", js->newFunction(QRect_ctor<QRect>));
	glob.setProperty("QRectF", js->newFunction(QRect_ctor<QRectF>));
	glob.setProperty("QSize", js->newFunction(QSize_ctor<QSize>));
	glob.setProperty("QSizeF", js->newFunction(QSize_ctor<QSizeF>));
	glob.setProperty("QPoint", js->newFunction(QPoint_ctor<QPoint>));
	glob.setProperty("QPointF", js->newFunction(QPoint_ctor<QPointF>));

	glob.setProperty("alert",
			 js->newFunction(js_alert),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("confirm",
			 js->newFunction(js_confirm),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("randomInt",
			 js->newFunction(js_randomInt),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
	glob.setProperty("toSource",
			 js->newFunction(js_toSource),
			 QScriptValue::ReadOnly | QScriptValue::Undeletable );
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
	return m_cx && (m_max>=0) && (m_at<=m_max);
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

    QString JSVariantPrototype::toString()
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
