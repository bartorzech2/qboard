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
#include <QtGlobal>
#include <QGraphicsScene>
class QPoint;
class QGraphicsItem;
class GameState;

#define QBOARD_VERBOSE_DTOR if(0) qDebug()

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
       Works like clipboardGraphicsItems(), but takes a reference
       point (origin) to store in the serialized data. That point is
       used by paste operations to calculate the new position of
       pasted items, so as to keep their relative positions intact.
    */
    bool clipboardScene( QGraphicsScene * gsc, bool copy, QPoint const & origin );
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

    /**
       Transforms qgi to be rotated around its centerpoint.
    */
    void rotateCentered( QGraphicsItem * qgi, qreal angle );

    /**
       Calculates the bounding rectangle of one or more QGraphicsItems.

       If qgi is selected, then this function calculates the bounds
       point of the largest bounding rectangle of all selected
       items. If qgi is not selected then the bounds of qgi returned.

       If skipParented is true then this function skips over any QGI
       with a parent, because not skipping them would hose our current
       use-case. As a special case, if (!qgi->isSelected()) then
       the skipParented flag is ignored. In that case, the bounds of
       the qgi are calculated in its parent's coord space, not the
       scene coord space.
    */
    QRectF calculateBounds( QGraphicsItem * qgi, bool skipParented = true );

    /**
       Calculates the middle point of calculateBounds(qgi).
    */
    QPointF calculateCenter( QGraphicsItem * qgi, bool skipParented = true );


    /**
       For each selected item in sc, an attempt is made to cast it to
       a (T*). On success, the (T*) is added to the which is returned
       by this function. This does not change ownership of sc nor the
       (T*) contained in the returned list.

       T must be a non-cv-qualified type.
    */
    template <typename T>
    static QList<T*> selectedItemsCast( QGraphicsScene * sc )
    {
	typedef QList<QGraphicsItem*> QGIL;
	QGIL sel = sc ? sc->selectedItems() : QGIL();
	typedef QList<T *> OL;
	OL ol;
	for( QGIL::iterator it = sel.begin();
	     sel.end() != it; ++it )
	{
	    T * obj = dynamic_cast<T*>( *it );
	    if( obj ) ol.push_back(obj);
	}
	return ol;
    }

    
    /**
       Copies all "dynamic" properties from src to dest.
       Returns the number of properties copied.
    */
    int copyProperties( QObject const * src, QObject * dest );

    /**
       Unsets Copies all "dynamic" properties from obj.
       Returns the number of properties cleared.
    */
    int clearProperties( QObject * obj );


    /**
       Creates a new QTransform scaled and rotated to the given
       angle. If scaleY is 0 then it is set to the same as scaleX. If
       center is true (the default) then the transformation is
       centered with the given bounds.
     */
    QTransform rotateAndScale( QRectF const & bounds, qreal angle, qreal scaleX, qreal scaleY = 0, bool center = true );

    /**
       Like rotateAndScale(QRectF,...), where the rect is
       obj->boundingRect(), and the transformation is applied to obj
       using obj->setTransform().
    */
    void rotateAndScale( QGraphicsItem * obj, qreal angle, qreal scaleX, qreal scaleY = 0, bool center = true );

    /**
       Identical to rotateAndScale(QGraphicsItem*,...) except that it
       functions on a QGraphicsView and does not rotate around the center
       (which screws up the view's scrollbars).
    */
    void rotateAndScale( QGraphicsView * obj, qreal angle, qreal scaleX, qreal scaleY = 0 );


    /**
       Flips trans in either a horizontal (if horiz is true) or
       vertical (if horiz is false) direction. Returns trans.

       Note that trans must be translated either -bounds.width() (if
       horiz) or -bounds.height() (if !horiz), to ensure that the
       relative view position is kept intact.
    */
    QTransform & transformFlip( QTransform & trans,
				QSizeF const & bounds,
				bool horiz );

    /**
       Equivalent to transformFlip( trans, bounds.size(), horiz ).
    */
    QTransform & transformFlip( QTransform & trans,
				QRectF const & bounds,
				bool horiz );
    /**
       Transforms gi, as described for transformFlip(QTransform&,...).
    */
    void transformFlip( QGraphicsItem * gi, bool horiz );

    /**
       Transforms gi as described for transformFlip(QTransform&,...).
    */
    void transformFlip( QGraphicsView * gi, bool horiz );


    /**
       Qt 4.3 and 4.4 have different names for QGraphicsItem::children()
       resp. childItems(). This function uses whichever is appropriate
       for your Qt version and returns that list.
    */
    QList<QGraphicsItem*> childItems( QGraphicsItem const * );

}

#endif // QBOARD_UTILITY_H_INCLUDED
