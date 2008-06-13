#ifndef MENUHANDLERBOARD_H
#define MENUHANDLERBOARD_H

#include <QObject>
#include <QMenu>

//#include "MenuHandlerGeneric.h"
class QBoardView;
class QGraphicsScene;
class QContextMenuEvent;
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
	void doMenu( QBoardView *, QContextMenuEvent * );
    void doCopy();
    void doPaste();
private:
	struct Impl;
	Impl * impl;
};


#endif
