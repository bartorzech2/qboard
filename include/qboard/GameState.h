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


#include <qboard/Serializable.h>
class QBoard;
#include <QGraphicsScene>
#include <QPoint>
#include <QVariant>
class QGraphicsItem;
class QScriptEngine;
#include <QScriptValue>

/**
   GameState is the central point of a QBoard game. It holds the
   various parts of the game together.

   GameState is essentially made up of:

   - a single QGraphicsScene to put "game pieces" on.

   - an arbitrary number of QGraphicsItem ("game pieces"), which are
   managed by the QGraphicsScene. In order to fully integrate into
   QBoard, each item needs to subclass QObject and Serializable.

   - a single QBoard object (the game board)

   - a QScriptEngine with some QBoard-specific functionality added to
   it.

   To start a "new game", simply create a GameState object, then
   manipulate it at will. When you're ready to save it, use the
   s11nSave() member and use s11nLoad() to load its contents from a
   file or stream.

*/
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



    /**
       Returns the current placement position. See addPiece() for
       details.
    */
    Q_INVOKABLE QPointF placementPos() const;

    /**
       Used by the copy/paste code.
    */
    static char const * KeyClipboard;

    /**
       Returns this object's JS engine.
    */
    QScriptEngine & jsEngine() const;

public Q_SLOTS:
    /**
       Enables a specially-treated "placemarker" item. It can be moved
       around by calling setPlacementPos() or via drag/drop. Calling
       placementPos() will return the position of the placemarker.

       The placemarker is disabled (the default), setPlacementPos()
       and placementPos() will still have an effect, but there will be
       no GUI component for it.
    */
    void enablePlacemarker( bool );


    /**
       Tries to paste the contents of the clipboard into the current
       game. pos is a reference position, and the positions of clipboarded
       data may be adjusted based on this pos.
    */
    bool pasteClipboard( QPoint const & pos );

    /**
       Removes all game items from the game.
    */
    void clear();
    /**
       Sets the "placement position". This position is optionally used
       by addItem() to place new items in the game.
    */
    void setPlacementPos( QPointF const & );

    /**
       If it returns true, it transfers ownership of the item to this
       object's QGraphicsScene, otherwise the caller owns the item.

       If autoPlace is true then the item is moved to placementPos(),
       offset by half of the width/height of the item.
    */
    bool addItem( QGraphicsItem * item, bool autoPlace = false );

    /**
       Evaluates the given file as JavaScript code, in the context of
       this object's JS engine (see jsEngine()). If the file cannot
       be opened in read mode, the returned value will be an Error
       object.
    */
    QScriptValue evalScriptFile( QString const & filename );

    void clipCopySelected();
    void clipCutSelected();
    void clipPaste();
    void selectAll();

private Q_SLOTS:
    void placemarkerDestroyed();

private:
    GameState & operator=(GameState const &); // not implemented!
    GameState(GameState const &); // not implemented!
    /** internal detail. */
    void setup();
    /** internal detail. */
    bool pasteTryHarder( S11nNode const & root,
			 QPoint const & pos );
    struct Impl;
    Impl * impl;
};

Q_DECLARE_METATYPE(GameState*);

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
