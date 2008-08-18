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
#include "S11nClipboard.h"

struct GameState::Impl
{
    mutable GamePieceList pieces;
    QBoard board;
//     QPoint placeAt;
    mutable QGraphicsScene * scene;
    /** lameKludge exists for the "addPiece() QGraphicsItem return workaround" */
    QGraphicsItem * lameKludge;
    Impl() :
	pieces(),
	board(),
// 	placeAt(0,0),
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

// QPoint GameState::placementPos() const
// {
//     return impl->placeAt;
// }

// void GameState::setPlacementPos( QPoint const & p )
// {
//     qDebug() <<"GameState::setPlacementPos("<<p<<")";
//     impl->placeAt = p;
// }


QGraphicsScene * GameState::scene()
{
    return impl->scene;
}

QGraphicsItem * GameState::addPiece( GamePiece * pc )
{
    if( ! pc ) return 0;
    impl->pieces.addPiece(pc);
    QGraphicsItem * x = impl->lameKludge;
    impl->lameKludge = 0;
    return x;
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
    /**
       FIXME:

       This code is largely here to accomodate different types of
       Views for GamePiece- vs non-GamePiece objects. We need to
       come up with a way to eliminate this special-case crap.
    */
    QList<Serializable*> nonPiece;
    // We need to make sure that the pieces have a 'pos' property set, so we will
    // look at all QGIGamePieces and sync their position properties. So lame.
    // This is why members of this type are mutable.
    typedef QList<QGraphicsItem *> QL;
    QL ql( this->impl->scene->items() );
    QL::iterator it = ql.begin();
    QL::iterator et = ql.end();
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
	    qDebug() << "WARNING: GameState::serialize() kludge: setting 'pos' property for piece.";
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
	try
	{
	    if( ! s11n::deserialize( *ch, li ) ) return false;
	    QL::iterator it = li.begin();
	    QL::iterator et = li.end();
	    for( ; et != it; ++it )
	    {
		QGraphicsItem * gi = dynamic_cast<QGraphicsItem*>( *it );
		//li.erase(it);
		if( ! gi )
		{
		    s11n::cleanup_serializable( li );
		    return false;
		}
		impl->scene->addItem( gi );
	    }
	    li.clear();
	}
	catch(...)
	{
	    s11n::cleanup_serializable( li );
	    qDebug() << "GameState::deserialize() caught exception. Cleaning up and passing it on.";
	    throw;
	}
    }
    return true;
}

static void adjustPos( QObject * obj, QPoint const & pos )
{
    if( ! obj ) return;
    QVariant vpos( obj->property("pos") );
    QPoint cpos = vpos.isValid() ? vpos.toPoint() : QPoint();
    QPoint pD;
    if( ! pos.isNull() && ! cpos.isNull() )
    {
	pD = cpos - pos;
    }
#if 1
    QPoint newpos = cpos.isNull() ?  pos : cpos;
    qDebug() << "adjustPos("<<obj<<","<<pos<<"): delta="<<pD<<"cpos="<<cpos<<"newpos="<<newpos;
    newpos -= pD;
    qDebug() << "adjustPos("<<obj<<","<<pos<<"): delta="<<pD<<"cpos="<<cpos<<"newpos="<<newpos;
    if( newpos != cpos )
    {
	obj->setProperty("pos", newpos);
    }
#else
    QPoint newpos = cpos.isNull() ? pos : cpos;
    if( ! pD.isNull() )
    {
	newpos -= pD;
	qDebug() << "adjustPos("<<obj<<","<<pos<<"): delta="<<pD<<"cpos="<<cpos<<"newpos="<<newpos;
	if( newpos != cpos )
	{
	    obj->setProperty("pos", newpos);
	}
    }
#endif

}

bool GameState::pasteTryHarder( S11nNode const & root,
				QPoint const & pos )
{
    try {
	if( impl->board.deserialize(root) )
	{
	    return true;
	}
    }
    catch(...) {}
   try {
	GamePiece * pc = s11nlite::deserialize<GamePiece>(root);
	if( pc )
	{
	    //adjustPos( pc, pos );
	    pc->setProperty( "pos", pos );
	    this->addPiece(pc);
	    return true;
	}
    }
    catch(...) {}
    GamePieceList * gli = 0;
    try {
	gli = s11nlite::deserialize<GamePieceList>(root);
	if( gli )
	{
	    QPoint xpos( pos );
	    QPoint step(8,8);
	    //qDebug() << "GameState::pasteTryHarder(): pasting GamePieceList @"<<pos;
	    impl->scene->clearSelection();
	    for( GamePieceList::iterator it = gli->begin();
		 it != gli->end(); ++it )
	    {
		(*it)->setProperty("pos", xpos);
		this->addPiece( *it )->setSelected(true);
		xpos += step;
	    }
	    gli->clearPieces(false);
	    delete gli;
	    return true;
	}
    }
    catch(...) {
	delete gli;
	gli = 0;
    }
    try {
	Serializable * ser = s11nlite::deserialize<Serializable>(root);
	if( ser )
	{
	    QGraphicsItem * qgi = dynamic_cast<QGraphicsItem*>(ser);
	    if( qgi )
	    {
		QObject * obj = dynamic_cast<QObject*>(ser);
		if( obj )
		{
		    adjustPos( obj, pos );
		    obj->setProperty( "pos", pos );
		}
		qDebug() << "GameState::pasteTryHarder(): pasting Serializable QGI";
		this->scene()->addItem( qgi );
		return true;
	    }
	    s11n::cleanup_serializable<Serializable>( ser );
	}
    }
    catch(...) {}

    return false;
}


char const * GameState::KeyClipboard = "GameStateClipboardData";
bool GameState::pasteClipboard( QPoint const & target )
{
    qDebug() << "GameState::pasteClipboard("<<target<<")";
    typedef QList<QGraphicsItem *> QGIL;
    typedef QList<Serializable *> SerL;
    S11nNode const * root = S11nClipboard::instance().contents();
    typedef S11nNodeTraits TR;
    if( ! root ) return false;
    if( (TR::name(*root) != KeyClipboard ) )
    {
	return root ? this->pasteTryHarder(*root,target) : false;
    }
    QPoint origin(0,0);
    S11nNode const * node = 0;
    if( (node = s11n::find_child_by_name(*root, "metadata")) )
    {
	s11nlite::deserialize_subnode( *node, "originPoint", origin );
	node = 0;
    }
    QPoint pD;
    if( ! origin.isNull() ) // && !target.isNull()
    {
	pD = origin - target;
    }
    node = s11n::find_child_by_name(*root, "pieces");
    QGIL newSelected;
    if( node )
    {
	GamePieceList list;
	if( ! s11nlite::deserialize( *node, list ) )
	{
	    return false;
	}
	GamePiece * pc = 0;
	bool gotOrigin = false;
	for( GamePieceList::iterator it = list.begin();
	     list.end() != it;
	     ++it )
	{
	    pc = *it;
	    QPoint pcpos;
	    QVariant vpos( pc->property("pos") );
	    if( ! vpos.isValid() )
	    {
		pcpos = target;
	    }
 	    else
 	    {
 		pcpos = vpos.toPoint();
 	    }
	    if( ! gotOrigin )
	    {
		gotOrigin = true;
		if( ! origin.isNull() )
		{
		    if( target.isNull() )
		    {
			pD = QPoint();
		    }
		    else
		    {
			pD = origin - target;
		    }
		}
		else
		{
		    if( target.isNull() )
		    {
			pD = QPoint();
			origin = pcpos;
		    }
		    else
		    {
			origin = pcpos;
			pD = pcpos - target;
		    }
// 		    pD = (target.isNull()
// 			  ? pcpos
// 			  : target)
// 			- target;
		}
	    }
	    pcpos -= pD;
	    pc->setPieceProperty("pos",pcpos);
	    newSelected.push_back( this->addPiece( pc ) );
	    if(0) qDebug() << "qboard::pasteGraphicsItems() pasting piece:"<<pc
			   << "\norigin ="<<origin<<", target ="<<target<<", pcpos ="<<pcpos<<"delta ="<<pD;
	}
	list.clearPieces(false);
	//causing a crash: this->pieces().takePieces( list );
    }
    node = s11n::find_child_by_name(*root, "board");
    if( node )
    {
	if( ! s11nlite::deserialize( *node, impl->board ) )
	{
	    if(1) qDebug() << "qboard::pasteGraphicsItems() pasting deserialization of board failed!";
	}
    }
    node = s11n::find_child_by_name(*root, "graphicsitems");
    if( node )
    {
	if(0) qDebug() << "qboard::pasteGraphicsItems() pasting items";
	SerL list;
	if( ! s11nlite::deserialize( *node, list ) )
	{
	    if(1) qDebug() << "qboard::pasteGraphicsItems() pasting deserialization of graphicsitems failed!";
	    return false;
	}
	for( SerL::iterator it = list.begin();
	     it != list.end(); ++it )
	{
	    QGraphicsItem * qgi = dynamic_cast<QGraphicsItem*>( *it );
	    if( qgi )
	    {
		QPoint newpos( qgi->pos().toPoint() - pD );
		qgi->setPos( newpos );
		if(0) qDebug() << "qboard::pasteGraphicsItems() adding Serializable QGI to scene:"
			       << "newpos ="<<newpos
			       << qgi;
		newSelected.push_back(qgi);
		this->scene()->addItem(qgi);
		continue;
	    }
	    if(0) qDebug() << "qboard::pasteGraphicsItems() warning: skipping non-piece, non-QGI Serializable in input.";
	    delete( *it );
	}
    }

    if( ! newSelected.empty() )
    {
	this->scene()->clearSelection();
	QGIL::iterator it = newSelected.begin();
	for( ; newSelected.end() != it; ++it )
	{
	    (*it)->setSelected(true);
	}
	newSelected.clear();
    }
    return true;
}
