#ifndef GAMESTATE_H
#define GAMESTATE_H
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


#include "Serializable.h"
class QBoard;
#include <QGraphicsScene>
#include <QPoint>
#include <QVariant>
class QGraphicsItem;
class QScriptEngine;
#include <QScriptValue>
class GameState : public QObject,
		  public Serializable
{
    Q_OBJECT
public:
    GameState();
    virtual ~GameState();


    /**
       This object's game board.
    */
    QBoard & board();
    /** Serializes this object to.
	
    The following data are serialized:
		
    - this->board().
		
    - All top-level QGraphicsItems in this->scene()
    which derive from Serializable. Parented QGraphicsItems,
    even if Serializable, are not saved by this routine
    because deserialize() cannot know enough information
    to rebuild the QObject ownership heirarchy. This also allows
    parenting Serializables to get away with some weird tricks
    (e.g. see QGILineBinder).
    */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );

    /**
       This object's scene. This object owns it.
    */
    QGraphicsScene * scene();

//     QList<QObject*> selectedObjects();
//     QList<Serializable*> selectedSerializables();


//     /**
//        Returns the current placement position. See addPiece() for
//        details.
//     */
//     QPoint placementPos() const;

    static char const * KeyClipboard;

    QScriptEngine & jsEngine();

public Q_SLOTS:


    /**
       Tries to paste the contents of the clipboard into the current
       game. pos is a reference position, and the positions of clipboarded
       data may be adjusted based on this pos.
    */
    bool pasteClipboard( QPoint const & pos );

    void clear();
//     /**
//        Sets the "placement position". If addPiece(piece,true) is called,
//        the piece is moved to this position.
//     */
//     void setPlacementPos( QPoint const & );
    /**
       If it returns true, it transfers ownership of the item to this
       object's QGraphicsScene, otherwise the caller owns the item.
    */
    bool addItem( QGraphicsItem * item );

#if 0
    /**

    */
    bool addObject( QObject * tgt );
#endif
    //bool addObject( QScriptValue obj );
    /**
       Creates a new object of the given type name.  On success, the
       object is returned on error, 0 is returned. The caller owns the
       returned object.  It should normally be passed to addItem()
       (assuming the type is-also-a QGraphicsItem type).
    */
    QObject * createObject( QString const & className );
    /**
       Calls tgt->setProperty() with the given property.

       This is intended to be called from JS code.
    */
    bool prop( QObject * tgt, QString const &,
	       QScriptValue const & val );
    //QVariant const & val );
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

private:
    void setup();
    bool pasteTryHarder( S11nNode const & root,
			 QPoint const & pos );
    struct Impl;
    Impl * impl;
};

// Register GameState with s11n:
#define S11N_TYPE GameState
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "GameState"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE GameState
#define S11N_TYPE_NAME "GameState"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>



#endif
