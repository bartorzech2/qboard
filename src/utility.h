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

class QPoint;
class QGraphicsItem;
class GameState;

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
       Returns the directory (from somewhere under home()/QBoard/...)
       which can be used as a class-specific storage location for persistant
       data. The directory is created if needed, and this function throws
       a std::runtime_error() if the dir can be neither accessed nor created.

       The intention is to give plugins and add-ons a place to store session
       information.
    */
    QDir persistenceDir( QString const & className );

//     /**
//        Like persistenceDir(), but returns a dir for storing class-specific
//        help files.
//     */
//     QDir helpDir( QString const & className );

	
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

    /**
       Tries to load the QBoard text resource with the given name and
       returns its content as a string. An empty string can mean the
       resource was not found or (less likely) empty content.
    */
    QString loadResourceText(QString const & resourceName);

    /**
       Calls loadResourceText() and shows the loaded text
       in a new window.
    */
    void showHelpResource( QString const & title, QString const & res );

    /**
       If origin is not currently selected, then if it is-a
       Serializable it is copied to the system clipboard.  If origin
       is selected, all selected Serializables are copies. If the copy
       parameter is set to false, the item(s) is(are) cut to the
       clipboard instead of copied.

       Returns false if there are no items to serialize or if
       serialization fails. On success it updates the clipboard
       and returns true.

       The x/y coordinates of the origin object are stored in the
       clipboard and are used by pasteGrapihcsItems() to calculate
       a new position for all pasted objects.

       Special cases:

       - QGIGamePiece objects are not serialized directly. Instead
       we serialize their viewed pieces. On deserialization, creation
       of those pieces causes their views to be created. That means
       that polymorphic QGIGamePiece objects are not supported here.
    */
    bool clipboardGraphicsItems( QGraphicsItem * origin, bool copy );

    /**
       This "undoes" the work done by clipboardGraphicsItems(). That is,
       it pastes the clipboarded items back into the game state.

       pos is used as the target for the past. The actual position of
       pasted objects is relative to their initial position at
       copy-time and the position of the initially-copied object (the
       first parameter to clipboardGraphicsItems()). A delta is
       calculated and applied to each pasted items' position. This
       means that a selected group will paste back to its same
       relative arrangement, if though the absolute positions have
       changed.
    */
    bool pasteGraphicsItems( GameState & st, QPoint const & pos );

}

#endif // QBOARD_UTILITY_H_INCLUDED