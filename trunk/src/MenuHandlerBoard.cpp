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

#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QIcon>

//#include "MenuHandlerGeneric.h"
#include "GameState.h"
#include "utility.h"
#include "S11nClipboard.h"
#include <stdexcept>
#include "QBoard.h"
#include "QBoardView.h"
#include "GL.h"
#include "MenuHandlerGeneric.h"

struct MenuHandlerBoard::Impl
{
    QBoard * board;
    GameState * gs;
    QPoint pastePos;
    Impl() :
	board(0),
	gs(0),
	pastePos(0,0)
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
	//qDebug() << "Copied board to clipboard:";
	//s11nlite::save( *cl.contents(), std::cout );
    }

}
void MenuHandlerBoard::doPaste()
{
    if( ! impl->gs ) return;
    //qboard::pasteGraphicsItems( *impl->gs, impl->pastePos );
    impl->gs->pasteClipboard( impl->pastePos );
}

void MenuHandlerBoard::doMenu( GameState & gs, QBoardView * pv, QContextMenuEvent * ev )
{
    ev->accept();
    impl->gs = & gs;
    impl->pastePos = pv->mapToScene( ev->pos() ).toPoint();
    impl->board = &pv->board();
    QMenu m(pv);
    m.addAction("Board")->setEnabled(false);
    m.addSeparator();
    m.addAction(QIcon(":/QBoard/icon/editcopy.png"),"Copy board (not pieces)",this,SLOT(doCopy()) );

    QList<QGraphicsItem*> selected( pv->scene()->selectedItems() );
    if( ! selected.isEmpty() )
    {
	QGraphicsItem * pv = *(selected.begin());
	MenuHandlerCopyCut * clipper = new MenuHandlerCopyCut( pv, &m );
	m.addAction(QIcon(":/QBoard/icon/editcopy.png"),tr("Copy selected items"),clipper,SLOT(clipboardCopy()) );
	m.addAction(QIcon(":/QBoard/icon/editcut.png"),"Cut selected items",clipper,SLOT(clipboardCut()) );
    }

    QAction * act = m.addAction(QIcon(":/QBoard/icon/editpaste.png"),
				QString("Paste (%1)").arg( S11nClipboard::instance().contentLabel() ),
				this,SLOT(doPaste()) );
    S11nNode * cbc = S11nClipboard::instance().contents();
    if( ! cbc )
    {
	act->setEnabled(false);
    }

    if(1)
    {
	QObjectPropertyMenu * pm =
	    new QObjectPropertyMenu( "Scale", pv, "scale", 0 );
#define ADDIT(Val) pm->addItem(QVariant(Val))->setText(QString("%1%").arg(int(Val*100)));
	ADDIT(0.25);
	ADDIT(0.50);
	ADDIT(1.0);
	ADDIT(2.0);
	ADDIT(4.0);
	ADDIT(8.0);
#undef ADDIT
	pm->setIcon(QIcon(":/QBoard/icon/viewmag.png"));
	m.addMenu(pm);

    }
    if(1)
    {
	QObjectPropertyMenu * pm =
	    QObjectPropertyMenu::makeNumberListMenu("Rotate",
						    pv, "angle",
						    0, 360, 45);
	pm->setIcon(QIcon(":/QBoard/icon/rotate_cw.png"));
	m.addMenu(pm);
    }


#if QBOARD_USE_OPENGL
    {
	m.addSeparator();
	QObjectPropertyAction * ac =
	    new QObjectPropertyAction( pv,
				       "openglMode",
				       QVariant( pv->isGLMode() ? 0 : 1 ) );
	m.addAction( ac );
	ac->setText( tr("Toggle OpenGL Mode") );
	ac->setCheckable( true );
	ac->setChecked( pv->isGLMode() );
    }
#endif
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

