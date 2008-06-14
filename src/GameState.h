#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Serializable.h"
#include "GamePiece.h"
#include "QBoard.h"
#include <QGraphicsScene>
class GameState : public QObject, public Serializable
{
Q_OBJECT
public:
	GameState();
	virtual ~GameState();
	GamePieceList & pieces();
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
	QGraphicsScene * scene();

public Q_SLOTS:
	void clear();
private Q_SLOTS:
    void pieceAdded( GamePiece * pc );
    void pieceRemoved( GamePiece * pc );

private:
	mutable GamePieceList m_pc;
	QBoard m_b;
	mutable QGraphicsScene * m_sc;
};
#endif
