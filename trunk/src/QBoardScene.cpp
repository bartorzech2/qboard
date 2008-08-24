#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>

#include "QBoardScene.h"
#include "utility.h"

struct QBoardScene::Impl
{
    Impl()
    {
    }
    ~Impl()
    {
    }
};

QBoardScene::QBoardScene() : QGraphicsScene(),
    Serializable("QBoardScene"),
    impl(new Impl)
{
}

QBoardScene::~QBoardScene()
{
    delete impl;
}

static void paintLinesToChildren( QGraphicsItem * qgi,
				  QPainter * painter,
				  QPen const & pen )
{
    typedef QList<QGraphicsItem*> QGIL;
    QGIL ch( qboard::childItems(qgi) );

    if( ch.isEmpty() ) return;
    QRectF prect( qgi->boundingRect() );
    QPointF mid( prect.left() + (prect.width() / 2),
		 prect.top() + (prect.height() / 2) );
    painter->save();
    for( QGIL::iterator it = ch.begin();
	 ch.end() != it; ++it )
    {
	QGraphicsItem * x = *it;
	QRectF xr( x->boundingRect() );
	QPointF xmid( xr.center() );
	//xmid = x->mapToParent( xmid );
	xmid = qgi->mapFromItem( x, xmid );
	painter->setPen( pen );
	painter->drawLine( QLineF( mid, xmid ) );
    }
    painter->restore();
}
void QBoardScene::drawItems( QPainter * painter,
				int numItems,
				QGraphicsItem ** items,
				const QStyleOptionGraphicsItem * options,
				QWidget * widget )
{
#if 1
    this->QGraphicsScene::drawItems( painter, numItems, items, options, widget );
#else
    // This only does what i want when GL mode is on.
    QPen linePen(Qt::red, 2, Qt::DotLine, Qt::FlatCap, Qt::MiterJoin);
    for (int i = 0; i < numItems; ++i) {
	painter->save();
	painter->setMatrix(items[i]->sceneMatrix(), true);
	items[i]->paint(painter, &options[i], widget);
	paintLinesToChildren( items[i], painter, linePen );
	painter->restore();
    }
#endif
}
bool QBoardScene::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    return true;
}

bool QBoardScene::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    return true;
}


