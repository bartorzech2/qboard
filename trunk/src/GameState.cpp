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

    QObject::connect( &m_pc, SIGNAL(pieceAdded(GamePiece*)),
		      this, SLOT(pieceAdded(GamePiece*)) );
    QObject::connect( &m_pc, SIGNAL(pieceRemoved(GamePiece*)),
		      this, SLOT(pieceRemoved(GamePiece*)) );
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

void GameState::pieceAdded( GamePiece * pc )
{
    QGIGamePiece * pv = new QGIGamePiece(pc,m_sc);
    //pv->setVisible(true);
    //pv->update();
    qDebug() << "GameState::pieceAdded() added QGIGamePiece"
	     << static_cast<QObject*>(pv);
    //m_sc->update();
}
void GameState::pieceRemoved( GamePiece * )
{
    // We don't really care - the view will destroy itself when the piece goes.
}

void GameState::clear()
{
    // Pieces should be cleared first, to avoid potential double or otherwise inappropriate
    // deletes due to the use QObject::deleteLater() in QGIGamePiece.
    m_b.clear();
    this->pieces().clearPieces();
    QList<QGraphicsItem *> ql(m_sc->items()); 
    //qDebug() <<"GameState::clear() trying to clear"<<ql.size()<<" QGI items";
    qboard::destroy( ql );
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
    return s11n::serialize_subnode( dest, "board", this->m_b )
	&& s11n::serialize_subnode( dest, "pieces", this->m_pc )
	&& (nonPiece.isEmpty() ? true : s11nlite::serialize_subnode( dest, "graphicsitems", nonPiece ) )
	;
}

bool GameState::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
#if QT_VERSION >= 0x040400
    m_sc->clear();
#else
    m_sc->destroyItemGroup( m_sc->createItemGroup( m_sc->items() ));
#endif
    if( ! s11n::deserialize_subnode( src, "board", this->m_b ) ) return false;
    if( ! s11n::deserialize_subnode( src, "pieces", this->m_pc ) ) return false;
    //qDebug() << "GameState::deserialize() got"<<m_pc.size()<<"pieces.";
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

