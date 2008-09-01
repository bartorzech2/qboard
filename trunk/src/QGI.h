#ifndef QBOARD_QGI_H_INCLUDED
#define QBOARD_QGI_H_INCLUDED
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

#include <QList>
#include <QGraphicsItem>
class QGraphicsSceneMouseEvent;
/**
   QGITypes holds the custom QGraphicsItem::type() values used
   by QBoard-related QGraphicsItem types.
*/
struct QGITypes
{
	/**
	   All QBoard QGraphicsItems should have a type entry here.
	*/
	enum Types {
	_TypesStart = QGraphicsItem::UserType + 100,
	LineNode, // will go away.
	LineNodeBinder,
	QGIDie,
	QGIDot,
	QGIDotLine,
	QGIHider,
	QGIHtml,
	QGIPiece,
	QGIPiecePlacemarker,
	_TypesEnd
	};

    /**
       If the event is a left click then item is moved to the top of
       the view stack via adjustment of its zLevel, based on the
       zLevel of all items intersecting the item. It it is a
       middle click the item is moved to the bottom of the stack.

       This routine does NOT call ev->accept().

       It returns true if it modified the zlevel, otherwise false.
    */
    static bool handleClickRaise( QGraphicsItem * item,
				  QGraphicsSceneMouseEvent * ev );

    /**
       Randomly shuffles the positions of all items in the given
       list. It will not introduce new positions - it randomly
       swaps the positions of the given items.

       An example use for this:

       Create a set of gaming cards (e.g. using QGIHtml items), cover
       them, select a stack of them, then use this routine to shuffle
       the cards. It can also be used to randomize placement of starting
       units for solitaire play of certain games.

       If skipParentedItems is true (the default), any items in the
       list which have a parent item are SKIPPED for purposes of
       shuffling. The reason for this that when children are shuffled
       relative to coordinates of items with other parents, the
       results can be somewhat painful. If you know that all items in
       list share the same parent (or are all top-level), you can
       safely pass false.
    */
    static void shuffleQGIList( QList<QGraphicsItem*> list, bool skipParentedItems = true );
};
#endif // QBOARD_QGI_H_INCLUDED
