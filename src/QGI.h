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
};
#endif // QBOARD_QGI_H_INCLUDED
