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
#include <QSharedData>
#include <QString>

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
		      QString const & name = QString() );
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

}

#endif // QBOARD_ScriptQt_H_INCLUDED
