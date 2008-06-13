#include "GameState.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QList>
#include "S11nQt.h"
#include "S11nQtList.h"
#include "QGIGamePiece.h"
#include "utility.h"
GameState::GameState(  ) :
	Serializable("GameState"),
	m_pc(),
	m_b(),
	m_sc( new QGraphicsScene( QRectF(0,0,200,200) ) )
{
}


GameState::~GameState()
{
	this->clear();
	delete m_sc;
}

QGraphicsScene * GameState::scene()
{
	return m_sc;
}

void GameState::clear()
{
	// Pieces should be cleared first, to avoid potential double or otherwise inappropriate
	// deletes due to the use QObject::deleteLater() in QGIGamePiece.
	this->pieces().clearPieces();
	QList<QGraphicsItem *> ql(m_sc->items()); 
	//qDebug() <<"GameState::clear() trying to clear"<<ql.size()<<" QGI items";
	qboard::destroy( ql );

}

size_t GameState::takePieces( GamePieceList &other )
{
	GamePieceList tmp;
	tmp.blockSignals(true);
	tmp.takePieces( other );
	qDebug() << tmp.size() << other.size();
	GamePieceList::iterator it = tmp.begin();
	GamePieceList::iterator et = tmp.end();
	size_t ret = 0;
	for( ; et != it; ++it, ++ret )
	{
		GamePiece * pc = *it;
		this->pieces().addPiece(*it);
		new QGIGamePiece(pc,this->m_sc);
	}
	tmp.clearNoDelete();
	return ret;
}
GamePieceList & GameState::pieces()
{
	return m_pc;
}
QBoard & GameState::board()
{
	return m_b;
}

bool GameState::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	// We need to make sure that the pieces have a 'pos' property set, so we will
	// look at all QGIGamePieces and sync their position properties. So lame.
	// This is why members of this type are mutable.
	typedef QList<QGraphicsItem *> QL;
	QL ql( this->m_sc->items() );
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
			if( ! ser ) continue;
			nonPiece.push_back(ser);
			continue;
		}
		GamePiece * pc = v->piece();
		if( ! pc ) continue;
		pc->setProperty("pos",v->pos());
		// ^^^^ Note that we do not use setPieceProperty() to avoid triggering a piecePropertySet() signal. 
	}
	return s11n::serialize_subnode( dest, "board", this->m_b )
		&& s11n::serialize_subnode( dest, "pieces", this->m_pc )
		&& (nonPiece.isEmpty() ? true : s11nlite::serialize_subnode( dest, "graphicsitems", nonPiece ) )
		;
}

bool GameState::deserialize(  S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	m_pc.clearPieces();
#if QT_VERSION >= 0x040400
	m_sc->clear();
#else
	m_sc->destroyItemGroup( m_sc->createItemGroup( m_sc->items() ));
#endif
	if( ! s11n::deserialize_subnode( src, "board", this->m_b ) ) return false;
	if( ! s11n::deserialize_subnode( src, "pieces", this->m_pc ) ) return false;
	//qDebug() << "GameState::deserialize() got"<<m_pc.size()<<"pieces.";
	GamePieceList::iterator it = m_pc.begin();
	GamePieceList::iterator et = m_pc.end();
	for( ; et != it; ++it )
	{
		new QGIGamePiece((*it),m_sc);
	}
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
			m_sc->addItem( gi );
		}
	}
	return true;
}

