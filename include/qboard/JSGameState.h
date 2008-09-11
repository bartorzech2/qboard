#ifndef QBOARD_JSGameState_H_INCLUDED
#define QBOARD_JSGameState_H_INCLUDED 1
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
#include <QScriptable>
#include <QScriptValue>
#include <qboard/ScriptQt.h>

class QGraphicsItem;
class GameState;
class QBoardView;

// namespace qboard {
//     template <>
//     struct convert_script_value_f<QBoardView*>
//     {
// 	QScriptValue operator()( QScriptEngine *, QBoardView * const & r) const;
// 	QBoardView * operator()( QScriptEngine *, const QScriptValue & args) const;
//     };
// }

/**
   JSGameState is a JS-side prototype for use as
   a script-side base type of GameState objects.
*/
class JSGameState : public QObject,
		    public QScriptable
{
Q_OBJECT
public:
    explicit JSGameState( QObject * parent = 0 );
    virtual ~JSGameState();

    Q_INVOKABLE QList<QGraphicsItem*> items();
    Q_INVOKABLE QString home() const;
public Q_SLOTS:
    void bogo();

    /**
       See Serializable::s11nSave(). This operates on
       this object's native GameState.
    */
    bool save( QString const & fn, bool autoAddExt = true );

    /**
       See Serializable::s11nLoad(). This operates on
       this object's native GameState.
    */
    bool load( QString const & fn );
    /**
       Calls tgt->setProperty() with the given property.

       This is intended to be called from JS code.

       As a special case, if key starts with a '_' character then this
       function does nothing and returns false. This is intended
       to avoid the accidental injection of "hidden" properties
       into the native property list.
    */
    bool prop( QObject * tgt, QString const & key,
	       QScriptValue const & val );

    /**
       Assumes that props contains a single-dimension set of
       properties. Each property is copied to tgt via
       tgt->setProperty().

       This is intended to be called from JS code.

       Example JS:

       \code
       var pc = qboard.createObject('QGIPiece');
       qboard.props( pc, { pos:QPoint(200,200),
           pixmap:'path/to/my.png'} );
       qboard.addItem( pc );
       \endcode

    */
    bool props( QObject * tgt, QScriptValue const & props );

    /**
       Returns the script-side value of tgt->property(name), or an
       invalid value if the property is not set.

       This is intended to be called from JS code.

       Example JS:

       \code
       var pc = qboard.createObject('QGIPiece');
       qboard.prop( pc, 'pos', QPoint(200,200) );
       qboard.prop( pc, 'pixmap', 'path/to/my.png' );
       \endcode
    */
    QScriptValue
    prop( QObject * tgt, QString const & name );

    /**
       Creates a new object of the given type name.  On success, the
       object is returned on error, 0 is returned. The caller owns the
       returned object unless: if they created item is-a QGraphicsItem then
       addItem(theItem) is called, transfering ownership to the graphics
       scene.

       If props.isObject() is true then this->props(object,props)
       is called to set the properties of the new object.
    */
    QScriptValue
    createObject( QString const & className, QScriptValue const & props = QScriptValue() );

    //QBoardView * createView();
    QScriptValue createView();

private:
    struct Impl;
    Impl * impl;
    GameState * self();
};
#endif // QBOARD_JSGameState_H_INCLUDED
