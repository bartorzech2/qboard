#ifndef QBOARD_QBoardScene_H_INCLUDED
#define QBOARD_QBoardScene_H_INCLUDED 1

#include <QObject>
#include <QGraphicsScene>
class QPainter;
class QWidget;
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>

#include <qboard/Serializable.h>

class QBoardScene : public QGraphicsScene,
		    public Serializable
{
Q_OBJECT
public:
    QBoardScene();
    virtual ~QBoardScene();

    /**
       Serializes this object to dest.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

protected:
    virtual void drawItems( QPainter * painter,
			    int numItems,
			    QGraphicsItem ** items,
			    const QStyleOptionGraphicsItem * options,
			    QWidget * widget = 0 );
    bool event( QEvent * event );
private:
    struct Impl;
    Impl * impl;
};

#endif // QBOARD_QBoardScene_H_INCLUDED
