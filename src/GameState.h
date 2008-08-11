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
#include "GamePiece.h"
class QBoard;
#include <QGraphicsScene>
class QGraphicsItem;
class GameState : public QObject, public Serializable
{
    Q_OBJECT
public:
    GameState();
    virtual ~GameState();
    /**
       This holds all GamePieces. When a piece
       is added to this list, it is automatically
       added to this object with a new View object
       for the piece. When a piece is removed, it's
       view is only automatically removed if the
       piece is destroyed.
     */
    GamePieceList & pieces();
    /**
       This object's game board.
    */
    QBoard & board();
    /** Serializes this object to.
	
    The following data are serialized:
		
    - All objects in this->pieces().
		
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

    /**
       This is functionally identical to calling
       pieces().addPiece(pc), but this returns
       the new QGraphicsItem associated with the
       piece.
    */
    QGraphicsItem * addPiece( GamePiece * pc );


public Q_SLOTS:
    void clear();
private Q_SLOTS:
    /**
       Creates a new View for pc and adds it to the scene.
    */
    QGraphicsItem * pieceAdded( GamePiece * pc );
    /**
       Currently does nothing. It should find the associated
       View and remove it.
    */
    void pieceRemoved( GamePiece * pc );

private:
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
