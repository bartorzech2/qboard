#ifndef __QGI_H__
#define __QGI_H__
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
	GamePiece, // obsolete
	LineNode,
	LineNodeBinder,
	QGIDot,
	QGIHtml,
	QGIPiece,
	QGIPiecePlacemarker
	};
};
#endif // __QGI_H__
