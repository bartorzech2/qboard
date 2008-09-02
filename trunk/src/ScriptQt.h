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
    QTSCRIPT_CONV(qpoint,QPoint);
    QTSCRIPT_CONV(qpointf,QPointF);
    QTSCRIPT_CONV(qsize,QSize);
    QTSCRIPT_CONV(qgi,QGraphicsItem*);
    QTSCRIPT_CONV(qgs,QGraphicsScene*);
    QTSCRIPT_CONV(qgilist,QList<QGraphicsItem*>);
#undef QTSCRIPT_CONV
    /**
       Creates a new QScriptEngine (which the caller owns).  It is
       preconfigured with qboard-specific functionality.

       The following Qt types can be created in script code,
       but see the major caveat below:

       QColor(int r, int g, int b [, int alpha])

       QPoint( int x, int y )

       QSize( int x, int y )

       Such types can be used as parameters to native code,
       but cannot currently be manipulated directly in JS code.
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

    class JSVariantPrototype :
	public QObject,
	public QScriptable
    {
	Q_OBJECT
    public:
	explicit JSVariantPrototype( QObject * parent = 0 );
	virtual ~JSVariantPrototype();

    public Q_SLOTS:
        /** A text/experiment function. */
	void foo();
	/**
	   Returns a stringified form of this object's
	   QVariant. String string will be the same as it is when a
	   QVariant is passed to qDebug().
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

	   - QVariant::DateTime, a new JS Date object.

	   - QVariant::Point/PointF, JS properties = x, y

	   - QVariant::RegExp, a new JS RegExp

	   - QVariant::Size, JS properties = width, height

	   Note that objects created this way cannot simply
	   be passed back to routines which expect those
	   native types. e.g. you cannot pass {x:1,y:2}
	   to a routine which expects a QPoint, even though
	   this routine will convert a QPoint to an object
	   with x/y properties.
	*/
	QScriptValue value();

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
       to_source_f specialization for QString.
       Takes special care to quote the string.
    */
    template <>
    struct to_source_f<QString>
    {
	QString operator()( QString const &  ) const;
    };
    /**
       to_source_f specialization for QSize.
    */
    template <>
    struct to_source_f<QSize>
    {
	QString operator()( QSize const & ) const;
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
	   - Size
	   - String

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
