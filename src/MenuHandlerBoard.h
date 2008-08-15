#ifndef MENUHANDLERBOARD_H
#define MENUHANDLERBOARD_H
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

#include <QObject>
#include <QMenu>

//#include "MenuHandlerGeneric.h"
class QBoardView;
class QGraphicsScene;
class QContextMenuEvent;
class GameState;
/**
	This is the default context menu handler for QBoardViews.
*/
class MenuHandlerBoard : public QObject
{
Q_OBJECT
public:
	MenuHandlerBoard();
	virtual ~MenuHandlerBoard();

public Q_SLOTS:
	void doMenu( GameState & gs, QBoardView *, QContextMenuEvent * );
    void doCopy();
    void doPaste();
private:
	struct Impl;
	Impl * impl;
};


#endif
