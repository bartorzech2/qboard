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

#include "GameState.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QList>
#include "S11nQt.h"
#include "S11nQtList.h"
#include "QGIGamePiece.h"
#include "utility.h"
#include "QBoard.h"

struct GameState::Impl
{
    mutable GamePieceList pieces;
    QBoard board;
    mutable QGraphicsScene * scene;
    QGraphicsItem * lameKludge;
    Impl() :
	pieces(),
	board(),
	scene( new QGraphicsScene( QRectF(0,0,200,200) ) ),
	lameKludge(0)
    {
    }
    ~Impl()
    {
	delete this->scene;
    }
};
GameState::GameState() :
    Serializable("GameState"),
    impl(new Impl)
{
    QObject::connect( &impl->pieces, SIGNAL(pieceAdded(GamePiece*)),
		      this, SLOT(pieceAdded(GamePiece*)) );
    QObject::connect( &impl->pieces, SIGNAL(pieceRemoved(GamePiece*)),
		      this, SLOT(pieceRemoved(GamePiece*)) );
}


GameState::~GameState()
{
    this->clear();
    delete impl;
}

QGraphicsScene * GameState::scene()
{
    return impl->scene;
}

QGraphicsItem * GameState::addPiece( GamePiece * pc )
{
    impl->pieces.addPiece(pc);
    return impl->lameKludge;
}
QGraphicsItem * GameState::pieceAdded( GamePiece * pc )
{
    return (impl->lameKludge = new QGIGamePiece(pc,impl->scene));
    //qDebug() << "GameState::pieceAdded() added QGIGamePiece" << static_cast<QObject*>(pv);
}
void GameState::pieceRemoved( GamePiece * )
{
    // We don't really care - the view will destroy itself when the piece goes.
}

void GameState::clear()
{
    // Pieces should be cleared first, to avoid potential double or otherwise inappropriate
    // deletes due to the use QObject::deleteLater() in QGIGamePiece.
    impl->board.clear();
    this->pieces().clearPieces();
    QList<QGraphicsItem *> ql(impl->scene->items()); 
    //qDebug() <<"GameState::clear() trying to clear"<<ql.size()<<" QGI items";
    qboard::destroy( ql );
}

GamePieceList & GameState::pieces()
{
    return impl->pieces;
}
QBoard & GameState::board()
{
    return impl->board;
}

bool GameState::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    // We need to make sure that the pieces have a 'pos' property set, so we will
    // look at all QGIGamePieces and sync their position properties. So lame.
    // This is why members of this type are mutable.
    typedef QList<QGraphicsItem *> QL;
    QL ql( this->impl->scene->items() );
    QL::iterator it = ql.begin();
    QL::iterator et = ql.end();
    QList<Serializable*> nonPiece;
    for( ; et != it; ++it )
    {
	if( (*it)->parentItem() )
	{ /* assume parent item will serialize the children if needed/desired.
	     In any case, we can't know how to re-build the parent
	     relationships from here. */
	    continue;
	}
	QGIGamePiece * v = dynamic_cast<QGIGamePiece *>( *it );
	// ^^^^ this cast is so Java/C#-ish that it makes me ill.
	if( ! v )
	{
	    Serializable * ser = dynamic_cast<Serializable*>( *it );
	    if( ! ser )
	    {
		qDebug() << "WARNING: GameState::serialize(): skipping non-Serializable QGraphicsItem:"<<*it;
		continue;
	    }
	    nonPiece.push_back(ser);
	    continue;
	}
	GamePiece * pc = v->piece();
	if( ! pc ) continue;
	QVariant ppos( pc->property("pos") );
	if( ! ppos.isValid() )
	{
	    pc->setProperty("pos",v->pos().toPoint() );
	    // ^^^^ Note that we do not use setPieceProperty() to avoid triggering a piecePropertySet() signal. 
	}
    }
    return s11n::serialize_subnode( dest, "board", this->impl->board )
	&& s11n::serialize_subnode( dest, "pieces", this->impl->pieces )
	&& (nonPiece.isEmpty() ? true : s11nlite::serialize_subnode( dest, "graphicsitems", nonPiece ) )
	;
}

bool GameState::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
#if 1
    this->clear();
#else
#if QT_VERSION >= 0x040400
    impl->scene->clear();
#else
    impl->scene->destroyItemGroup( impl->scene->createItemGroup( impl->scene->items() ));
#endif
#endif
    if( ! s11n::deserialize_subnode( src, "board", this->impl->board ) ) return false;
    if( ! s11n::deserialize_subnode( src, "pieces", this->impl->pieces ) ) return false;
    //qDebug() << "GameState::deserialize() got"<<impl->pieces.size()<<"pieces.";
    S11nNode const * ch = s11n::find_child_by_name(src, "graphicsitems");
    if( ch )
    {
	typedef QList<Serializable*> QL;
	QL li;
	if( ! s11n::deserialize( *ch, li ) ) return false;
	QL::iterator it = li.begin();
	QL::iterator et = li.end();
	for( ; et != it; ++it )
	{
	    QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>( *it );
	    if( ! gi )
	    {
		s11n::cleanup_serializable( li );
		return false;
	    }
	    impl->scene->addItem( gi );
	}
    }
    return true;
}

