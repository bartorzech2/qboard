#ifndef QBOARD_ScriptQt_H_INCLUDED
#define QBOARD_ScriptQt_H_INCLUDED 1
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

#include <QString>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
class QScriptContext;
#include <QSharedData>
#include <QString>
#include <QAction>
#include <QScriptable>

#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QList>
class QGraphicsItem;
class QGraphicsScene;


namespace qboard {

#define QTSCRIPT_CONV(PRE,T) \
    QScriptValue PRE ## ToScriptValue(QScriptEngine *, const T &); \
	void PRE ## FromScriptValue(const QScriptValue &, T &)
    QTSCRIPT_CONV(qgi,QGraphicsItem*);
    QTSCRIPT_CONV(qgs,QGraphicsScene*);
    QTSCRIPT_CONV(qgilist,QList<QGraphicsItem*>);
#undef QTSCRIPT_CONV

    /**
       Used by toScriptValue() and fromScriptValue() to do conversions.
       Specialize this to provide custom behaviour.
    */
    template <typename T>
    struct convert_script_value
    {
	/**
	   Default implementation returns eng->toScriptValue(t).
	*/
	QScriptValue operator()( QScriptEngine * eng,const T & t ) const
	{
	    return eng->toScriptValue(t);
	}
	/**
	   Default implementation returns qscriptvalue_cast<T>(obj);
	*/
	T operator()( QScriptEngine * eng,const QScriptValue & obj ) const
	{
	    return qscriptvalue_cast<T>( obj );
	}
    };

    /**
       Front-end for converting types to QScriptValues.
    */
    template <typename T>
    QScriptValue toScriptValue( QScriptEngine * e, T const & t )
    {
	return convert_script_value<T>()( e, t );
    }
    /**
       Front-end for converting QScriptValues to other types.
    */
    template <typename T>
    T fromScriptValue( QScriptEngine * e, QScriptValue const & v )
    {
	return convert_script_value<T>()( e, v );
    }

    /**
       Converts a QScriptValue to/from a QPoint or QPointF.
    */
    template <typename PT>
    struct convert_script_value_point
    {
	/**
	   Creates {x:pt.x(),y:pt.y()}
	*/
	QScriptValue operator()( QScriptEngine *, const PT & pt) const;
	/**
	   If args has a length property then it is assumed to contain
	   at least two values, the first two of which are used as
	   this object's x/y coordinates, otherwise it must contain
	   numeric x and y properties.
	*/
	PT operator()( QScriptEngine *, const QScriptValue & args) const;
    };

    template <>
    struct convert_script_value<QPoint> : convert_script_value_point<QPoint> {};

    template <>
    struct convert_script_value<QPointF> : convert_script_value_point<QPointF> {};


    /**
       Converts QRect/QRectF to/from QScriptValue.
    */
    template <typename RT>
    struct convert_script_value_rect
    {
	/**
	   Creates {left:r.left(),top:r.top(),width:r.width(),height:r.height()}
	*/
	QScriptValue operator()( QScriptEngine *, const RT & r) const;
	/**
	   If args is an array or special arguments object, it is assumed to
	   contain 4 numeric values, otherwise it is assumed to be an
	   object containing these numeric properties: top, left, width, height.
	   Returns a rectangle with those bounds.
	*/
	RT operator()( QScriptEngine *, const QScriptValue & args) const;
    };

    template <>
    struct convert_script_value<QRect> : convert_script_value_rect<QRect> {};

    template <>
    struct convert_script_value<QRectF> : convert_script_value_rect<QRectF> {};


    /**
       Converts QSize/QSizeF to/from QScriptValue.
    */
    template <typename ST>
    struct convert_script_value_size
    {
	/**
	   Creates object {width:sz.width(),height:sz.height()}
	*/
	QScriptValue operator()( QScriptEngine *, const ST & sz ) const;

	/**
	   If args is an array or has a .length property then it is
	   assumed to contain at least 2 numeric values (width and
	   height), otherwise it is assumed to contain the numeric
	   properties 'width' and 'height'. Returns a size object of
	   the given width and height.
	*/
	ST operator()( QScriptEngine *, const QScriptValue & args) const;
    };

    template <>
    struct convert_script_value<QSize> : convert_script_value_size<QSize> {};

    template <>
    struct convert_script_value<QSizeF> : convert_script_value_size<QSizeF> {};

    /**
       Converts QColor to/from QScriptObjects.
    */
    template <>
    struct convert_script_value<QColor>
    {
	/**
	   Creates JS object {red:c.red(),green:c.green(),blue:c.blue(),alpha:c.alpha()}
	*/
	QScriptValue operator()( QScriptEngine *, const QColor & c ) const;
	/**
	   If args is an array or has a .length property then it must
	   contain one of:

	   - a single string (a color name)

	   - 0 to 4 numeric values, in the order (red,green,blue,alpha).
	   Any missing values get a default (0 for rgb and 255 for alpha).
	   alpha may be either integer 0-255 or floating point 0.0-1.0.

	   Otherwise it must contain the properties ('red','green','blue'),
	   and optionally 'alpha', with the same values as described above
	   (except that there are no defaults - a missing property is interpreted
	   as a value of 0).

	   Returns a QColor object matching the given parameters.
	*/
	QColor operator()( QScriptEngine *, const QScriptValue & args) const;
    };


    /**
       Creates a new QScriptEngine (which the caller owns).  It is
       preconfigured with some custom functions and type conversions.
    */
    QScriptEngine * createScriptEngine( QObject * parent = 0 );

    /**
       ScriptPacket is intended to be a scriptable package of code for
       use in event handlers and such. It is copyable and uses
       reference counting and copy-on-write to reduce the actual
       copying which goes on.
    */
    class ScriptPacket : public QObject
    {
	Q_OBJECT;
    public:
	/**
	   Initialized the object with the given JS engine, JS code
	   string, and optional script name (used in error reporting).
	*/
	ScriptPacket( QScriptEngine * e = 0,
		      QString const & code = QString(),
		      QString const & name = QString(),
		      QObject * parent = 0 );
	/**
	   Copies rhs.
	*/
	ScriptPacket( ScriptPacket const & rhs );
	/**
	   Copies rhs.
	*/
	ScriptPacket & operator=(ScriptPacket const & rhs );
	virtual ~ScriptPacket();
	/**
	   Returns this object's JS code.
	*/
	QString code() const;
	/**
	   Returns this object's script's name.
	*/
	QString name() const;
	/**
	   Sets the script's name, for error reporting purposes.
	*/
	void setScriptName(QString const &);
	/**
	   Sets this object's script engine. Does not change ownership
	   of the engine.
	*/
	void setEngine( QScriptEngine * );
	/**
	   Returns this object's script engine. Does not change
	   ownership of the engine.
	*/
	QScriptEngine * engine() const;

	/**
	   If quickCheck is true (the default) then this function
	   returns true if this->engine() is not null, else false. If
	   quickCheck is false then it returns true only if engine()
	   is not null and engine()->canEvaluate(code()) is true.
	*/
	bool isValid( bool quickCheck = true ) const;
	/**
	   Equivalent to eval().
	*/
	QScriptValue operator()() const;
    public Q_SLOTS:
        /**
	   Evaluates this->code() and returns the result.
	*/
        QScriptValue eval() const;
	/**
	   Sets this object's script code.
	*/
	void setCode( QString const & );
    private:
	/** Implementation detail. */
	struct Impl : public QSharedData
	{
	    QString code;
	    QString name;
	    mutable QScriptEngine * js;
	    Impl();
	    ~Impl();
	};
	QSharedDataPointer<Impl> impl;
    };


    /**
       JavaScriptAction is a QAction type which evaluates JavaScript
       code when it is triggered.

    */
    class JavaScriptAction : public QAction
    {
	Q_OBJECT;
    public:
	/**
	   Constructs a new action object with the given label, JS
	   engine, and JS code. eng is set as this object's Qt parent.
	*/
	JavaScriptAction(QString const & label,
			 QScriptEngine & eng,
			 QString const & code  );
	virtual ~JavaScriptAction();
    private Q_SLOTS:
        void evaluateJS();
    private:
	void setup();
	struct Impl;
	Impl * impl;
    };


    /**
       ScriptArgv is intended to simplify access to QScriptValue
       arrays by providing ++ and [] operators. This makes
       accessing arguments very C-like.

       example:

       \code
       ScriptArgv argv(ctx);
       if( argv.argc() == 2 )
       {
           int x = (argv++).toInt32(); // ctx->argument(0)
           int y = argv().toInt32(); // ctx->argument(1)
       }
       \endcode

       or:

       \code
       for( ScriptArgv argv(ctx); argv.isValid(); ++argv )
       {
           qDebug() << argv.value().toString();
       }
       \endcode
    */
    class ScriptArgv
    {
    public:
	/**
	   Initialized argument traversal for the given context.
	   Does not affect ownership of cx.

	   Results of this object's operators are undefined if
	   cx->argumentCount() (or the arguments array) changes during
	   the lifetime of this object.
	*/
	explicit ScriptArgv( QScriptContext * cx );

	/**
	   Returns the current value, or an invalid value if this
	   object has been incremented out of range.
	*/
	QScriptValue value() const;

	/**
	   Equivalent to value().
	*/
	operator QScriptValue() const;

	/**
	   Equivalent to value().
	*/
	QScriptValue operator()() const;

	/**
	   Prefix ++. Increments the internal counter and returns that
	   item. Returns invalid values once incremented out of
	   bounds.

	   Remember that the default starting argument position is 0,
	   so calling (++thisObj) as the first operation would skip
	   the first argument.
	*/
	QScriptValue operator++();
	/**
	   Postfix ++. Returns the current item and increments the
	   internal counter. Returns invalid values once incremented
	   out of bounds.
	*/
	QScriptValue operator++(int);

	/**
	   Returns the script value at the given index, or an invalid
	   value if the index is out of bounds. Note that this index
	   is not affected by the ++ operators, and is always relative
	   to 0.
	*/
	QScriptValue operator[](int) const;

	/**
	   Equivalent to operator[].
	*/
	QScriptValue operator()(int) const;

	/**
	   Returns this object's argument count.
	*/
	int argc() const;

	/**
	   Resets the ++ operators, such that (thisObj++) will again
	   return the first value.
	*/
	void reset();

	/**
	   Returns true only if this object has arguments and has not
	   been incremented out of bounds.
	*/
	bool isValid() const;

    private:
	QScriptContext * m_cx;
	int m_at;
	int m_max;
    };


    /**
       JSVariantPrototype is intended to be used as a JS prototype
       object for JS-side QVariant values. My initial attempts at
       setting it as the default prototype for all QVariants failed,
       so here's how to do it:

       First, set up a prototype object somewhere in your app (you only
       need one instance):

       \code
	QScriptValue proto = engine->newQObject(new JSVariantPrototype(engine));
	\endcode

	Then you can apply that prototype object to all QVariants of your
	choosing:

       \code
       QVariant var( ... );
       QScriptValue val( engine->newVariant( var ) );
       val.setPrototype( proto );
       \endcode

       Now val will have the additional script-side API provided by
       this type.

       In theory one can call:

       \code
       engine->setDefaultPrototype(
           qMetaTypeId<QVariant>(),
           engine->newQObject(proto));
       \endcode

       to set the prototype engine-wide, but this isn't working for
       me. Dunno why.

    */
    class JSVariantPrototype :
	public QObject,
	public QScriptable
    {
	Q_OBJECT
    public:
	/**
	   Sets this object's QObject parent to parent, transfering
	   ownership if parent is not 0.
	*/
	explicit JSVariantPrototype( QObject * parent = 0 );
	virtual ~JSVariantPrototype();

    public Q_SLOTS:
        /** A text/experiment function. */
	void foo();
	/**
	   Returns a stringified form of this object's
	   QVariant. String string will be the same as it is when a
	   QVariant is passed to qDebug(), so it cannot be eval'd
	   or anything like that.
	*/
	QString toString();

	/**
	   Returns this object's QVariant's userType() value.
	*/
	int variantType();

	/**
	   Tries to convert this object's QVariant to
	   a generic JS object. Null is returned if no
	   conversion is possible. On success, a conversion
	   of the variant is returned. For simple PODs
	   (int, double, string, bool) this is a value.
	   For objects, the exact properties depend on
	   this->variantType().

	   Current supported types, aside from the basic
	   PODs:

	   - QVariant::Color, returns a string value

	   - QVariant::DateTime, returns a new JS Date object

	   - QVariant::Point/PointF, JS object: {x:X,y:Y}

	   - QVariant::Rect/RectF, JS object: {top:X,left:Y,width:W,height:H}

	   - QVariant::RegExp, returns a new JS RegExp object

	   - QVariant::Size/SizeF, JS object: {width:W,height:H}

	   Note that objects created this way cannot simply be passed
	   back to routines which expect those native types. e.g. you
	   cannot pass {x:1,y:2} to a routine which expects a QPoint,
	   even though this routine will convert a QPoint to an object
	   with x/y properties.
	*/
	QScriptValue jsValue();

	/**
	   see qboard::toSource<QVariant>().
	*/
	QString toSource();
    private:
	struct Impl;
	Impl * impl;
    };


    /**
       to_source_f is a helper to implement classical JavaScript
       toSource() features for native types.
    */
    template <typename T>
    struct to_source_f
    {
	/**
	   Default implementation returns a string which is only
	   useful for debuggering.
	*/
	QString operator()( T const & ) const
	{
	    return QString("'UNSPECIALIZED to_source_f<T>'");
	}
    };
    /**
       A specialization for pointer types which can handle null
       values. For non-null pointers it is equivalent to
       to_source_f<T>.
    */
    template <typename T>
    struct to_source_f<T*>
    {
	QString operator()( T const * & x ) const
	{
	    return x
		? to_source_f<T>()( *x )
		: QString("null");
	}
    };

    /**
       Returns to_source_f<T>()( x ).
    */
    template <typename T>
    QString toSource( T const & x )
    {
	return to_source_f<T>()( x );
    }

    /**
       to_source_f specialization for QPoint.
    */
    template <>
    struct to_source_f<QPoint>
    {
	QString operator()( QPoint const & ) const;
    };
    /**
       to_source_f specialization for QPointF.
    */
    template <>
    struct to_source_f<QPointF>
    {
	QString operator()( QPointF const &  ) const;
    };

    /**
       to_source_f specialization for QRectF.
    */
    template <>
    struct to_source_f<QRect>
    {
	QString operator()( QRect const & ) const;
    };
    /**
       to_source_f specialization for QRectF.
    */
    template <>
    struct to_source_f<QRectF>
    {
	QString operator()( QRectF const & ) const;
    };

    /**
       to_source_f specialization for QScriptValue.
    */
    template <>
    struct to_source_f<QScriptValue>
    {
	QString operator()( QScriptValue const & ) const;
    };

    /**
       to_source_f specialization for QSize.
    */
    template <>
    struct to_source_f<QSize>
    {
	QString operator()( QSize const & ) const;
    };
    template <>
    struct to_source_f<QSizeF>
    {
	QString operator()( QSizeF const & ) const;
    };

    /**
       to_source_f specialization for QString.
       Takes special care to quote the string.
    */
    template <>
    struct to_source_f<QString>
    {
	QString operator()( QString const &  ) const;
    };

    /**
       to_source_f specialization for QVariant.
    */
    template <>
    struct to_source_f<QVariant>
    {
	/**
	   Returns the JS source code form of v's value.
       
	   Only the following QVariant::Type types are support:
       
	   - Int
	   - Color (returned as a simple string, not full color info)
	   - Double
	   - Point/PointF
	   - Size/SizeF
	   - String
	   - Rect/RectF

	   All others will result in the value "undefined"
	   being returned.
	*/
	QString operator()( QVariant const & v) const;
    };

    /**
       A helper for to_source_f which can to-source
       any type supported by QString::arg(). DO NOT
       use this for QString types, as this will not
       quote them properly.
    */
    struct to_source_pod_f
    {
	template <typename T>
	QString operator()( T const x ) const
	{
	    return QString("%1").arg(x);
	}
	QString operator()( QString const x ) const
	{
	    return to_source_f<QString>()(x);
	}
    };

    /**
       to_source_f specialization for int.
    */
    template <>
    struct to_source_f<int> : to_source_pod_f {};
    /**
       to_source_f specialization for long.
    */
    template <>
    struct to_source_f<long> : to_source_pod_f {};
    /**
       to_source_f specialization for size_t.
    */
    template <>
    struct to_source_f<size_t> : to_source_pod_f {};
    /**
       to_source_f specialization for qreal.
    */
    template <>
    struct to_source_f<qreal> : to_source_pod_f {};
    /**
       to_source_f specialization for bool.
    */
    template <>
    struct to_source_f<bool> : to_source_pod_f {};



} // namespace

#endif // QBOARD_ScriptQt_H_INCLUDED
