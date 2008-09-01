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

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
class QScriptContext;
#include <QSharedData>
#include <QString>
#include <QAction>

namespace qboard {

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

} // namespace

#include <QList>
#include <QGraphicsItem>
Q_DECLARE_METATYPE(QList<QGraphicsItem*>)

#endif // QBOARD_ScriptQt_H_INCLUDED
