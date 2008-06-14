#ifndef QBOARD_UTILITY_H_INCLUDED
#define QBOARD_UTILITY_H_INCLUDED
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

#include <QString>
#include <QList>
#include <QDir>
#include <QColor>
#include <QDebug>
class QGraphicsItem;

#define QBOARD_VERBOSE_DTOR if(1) qDebug()

namespace qboard
{
	/** Returns the path to the QBoard home dir (~/.QBoard).
	*/
	QDir home();
	/**
		If fn is absolute and under home() then the home() part
		is stripped from fn and that path is returned, otherwise
		fn is returned as-is.
	*/
	QString homeRelative( QString const & fn );
	
	/**
		Compares the zLevel of the given QGraphicsItem to those items
		which collide with it. If gi has no collisions, then gi->zValue()
		is returned. If it has neighbors, the highest zValue of gi and all
		neighbors, plus some small increment, is returned. If gi and another
		neighbor have the same zValue then we cannot know their exact
		rendering ordering, so we return that value plus some small value.
		
		The intended use is:
		
		\code
		qreal z = nextZLevel(this);
		if( this->zValue() < z) this->setZValue(z);
		\endcode

		or similar.
	*/
	qreal nextZLevel( QGraphicsItem const * gi );
	
	/**
		Destroys the given item using the QBoard-kludgy approach.
		If alsoSelected is true, all selected items are destroyed.
		See destroy(QList) for details about what is NOT destroyed
		and special-case conditions.
	*/
	void destroy( QGraphicsItem *, bool alsoSelected );
	
	/**
		Destroys the given list of items using a QBoard-kludgy approach.
		ONLY top-level (unparented) items in the list are destroyed.
		The reason for this is to work around touchy timing problems
		(i.e. crashes) when deleting children and parents from this
		level of code. Parent objects own their children, so we respect
		that here. By deleting top-level parts, this routine implicitly
		deletes their children.

		QGIGamePiece elements are currently handled specially
		(this is unfortunate, however): the QGIGamePiece and its associated
		GamePiece are both destroyed. 
	*/
	void destroy( QList<QGraphicsItem *>& );
	
	/**
		Given one of the following strings, it returns the appropriate Qt::PenStyle
		value, or NoPen if it finds no match:
		
		"SolidLine", "DashLine", "DotLine", "DashDotLine", "DashDotDotLine"
	*/
	int stringToPenStyle( QString const & );
	/**
		The opposite of stringToPenStyle(). Returns "NoPen" if pen
		is not a known Qt::PenStyle. 
	*/
	QString penStyleToString( int pen );

	/**
		Returns the predefined Qt colors, except for Qt::transparent,
		which is left out because it breaks my particular use case.
	*/
	QList<QColor> colorList();

    /**
       Returns the version of QBoard.
    */
    const QString versionString();

}

#endif // QBOARD_UTILITY_H_INCLUDED
