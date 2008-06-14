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

#include "MenuHandlerBoard.h"
//#include "MenuHandlerGeneric.h"
#include "GamePiece.h"
#include "QGIGamePiece.h"
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QIcon>
#include "utility.h"
#include "S11nQt.h"
#include "S11nQtList.h"
#include "S11nClipboard.h"
#include <stdexcept>
#include "QBoard.h"
#include "QBoardView.h"

struct MenuHandlerBoard::Impl
{
    QBoard * board;
    Impl() :
	board(0)
    {
    }
    ~Impl()
    {
	
    }
};

MenuHandlerBoard::MenuHandlerBoard() 
    : impl(new MenuHandlerBoard::Impl)
{
	
}

MenuHandlerBoard::~MenuHandlerBoard()
{
    delete this->impl;
}

void MenuHandlerBoard::doCopy()
{
    if( ! impl->board ) return;
    S11nClipboard & cl( S11nClipboard::instance() );
    if( cl.serialize<Serializable>( *impl->board ) )
    {
	qDebug() << "Copied board to clipboard:";
	s11nlite::save( *cl.contents(), std::cout );
    }

}
void MenuHandlerBoard::doPaste()
{
#if 0 // we have no handle back to the game state!
    S11nClipboard & cl( S11nClipboard::instance() );
    s11n::cleanup_ptr<Serializable> ap( cl.deserialize<Serializable>() );
    if( ! ap.get() ) return;
    Serializable * top = ap.get();
    //if( GamePiece * pc = dynamic_cast<
#endif

}

void MenuHandlerBoard::doMenu( QBoardView * pv, QContextMenuEvent * ev )
{
    ev->accept();
    impl->board = &pv->board();
    QMenu m(pv);
    m.addAction("Board")->setEnabled(false);
    m.addSeparator();
    m.addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy board (not pieces)",this,SLOT(doCopy()) );
    //m.addAction(QIcon(":/QBoard/icon/editpaste.png"),"Paste",this,SLOT(doPaste()) );
    QAction * ac = m.addAction("Toggle OpenGL Mode",pv, SLOT(toggleGLMode()) );
    ac->setCheckable( true );
    ac->setChecked( pv->isGLMode() );
    try
    {
	m.exec( ev->globalPos() );
    }
    catch(std::exception const & ex)
    {
	qDebug() << "Menu handler threw:" << ex.what();
    }
    catch(...)
    {
	qDebug() << "Menu handler threw unknown exception type. Ignoring exception!";
    }

}

