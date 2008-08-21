#ifndef MENUHANDLERPIECE_H
#define MENUHANDLERPIECE_H
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
class QGraphicsSceneContextMenuEvent;
class QGIGamePiece;
class QGraphicsScene;

/**
	This is the default context menu handler for QGIGamePieces.
*/
class MenuHandlerPiece : public QObject
{
Q_OBJECT
public:
	MenuHandlerPiece();
	virtual ~MenuHandlerPiece();

public Q_SLOTS:
        void showHelp();
	void doMenu( QGIGamePiece *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
    void copyPieceList();
    bool copyPiece();
    bool cutPiece();
private:
	struct Impl;
	Impl * impl;
};


/**
	A QAction type which sets a property on
	a GamePiece, via a QGIGamePiece object.
	
	It works for selections as well as
	individual items.

*/
class PiecePropertyAction : public QAction
{
Q_OBJECT
public:
	/** Sets this object's target to view->piece(),
		with the given property name and value.
		parent is passed as-is to the QAction ctor.
		When this item is triggered,
		setPieceProperty(propName,val) is called
		on view's GamePiece.

		If obj is destroyed before this object, this action
		disconnects itself from obj and triggering the action
		becomes a no-op.

		When triggered, if view->isSelected(), the action
		is applied to all select QGIGamePiece items.

		Special cases:
		
		- If (val.type() == QVariant::Color) then
		a small "color splotch" icon of that color
		is added to this object.
	*/
	PiecePropertyAction( QGIGamePiece * view,
			QString const & propName,
			QVariant const & val,
			QObject * parent = 0 );
	virtual ~PiecePropertyAction();

private Q_SLOTS:
	void setProperty();
	void dtorDisconnect();
private:

	QGIGamePiece * m_gi;
	QString m_key;
	QVariant m_val;
};

/**
	A menu type to simplify the usage of PiecePropertyAction.
	
	
	Maintainer's note: most of this code
	is almost a 100% copy of the QObjectPropertyAction
	code, except that this copy has to accommodate
	for selections of multiple objects (which it indirectly
	does via PiecePropertyAction).
*/
class PiecePropertyMenu : public QMenu
{
Q_OBJECT
public:
	/**
		Creates a new menu with the given title and parent.
		When actions are added via addItem(QVariant,QString),
		they are PiecePropertyActions which are connected to
		view, using the given property name. The value set
		on the target is the one passed to addItem().
		
		If view->isSelected() is true at the time of the signal,
		then all selected QGIGamePiece items are affected.
	*/
	PiecePropertyMenu( const QString & title,
		QGIGamePiece * view,
		QString const & propName,
		QWidget * parent = 0 );
	virtual ~PiecePropertyMenu();
	/**
		Creates a new QObjectPropertyAction, when triggered, call
		piece->setPieceProperty(propName,val), using the piece and propName
		set in this object's ctor. If lbl is empty then val.toString()
		is used to set the item label. It returns the action, which is
		owned by this object.
	*/
	QAction * addItem( QVariant const & val, QString const & lbl = QString() );
	/**
		Creates a menu of alpha level settings. When triggered,
		pv->piece()->setPieceProperty(propertyName,selectedValue) is called.

		If hint is a valid color, each item in the menu has a color splotch
		of that color, with the appropriate alpha level applied.
		
		The caller owns the returned menu.
		
	*/
	static PiecePropertyMenu * makeAlphaMenu( QGIGamePiece * pv, char const * propertyName, QColor const & hint = QColor() );
	/**
		Creates a menu of common colors. When triggered,
		pv->piece()->setPieceProperty(propertyName,selectedValue) is called.
		If alphaName is non-null, a sub-menu is creates with alpha settings, and when triggered
		piece->setPieceProperty(alphaName,selectedValue) is called.

		The caller owns the returned menu.
	*/
	static PiecePropertyMenu * makeColorMenu( QGIGamePiece * pv, char const * propertyName, char const * alphaName = 0 );
	/**
		Creates a menu of Qt::PenStyle entries. When triggered,
		pv->piece()->setPieceProperty(propertyName,selectedValue) is called.
		
		The caller owns the returned menu.
	*/
	static PiecePropertyMenu * makePenStyleMenu( QGIGamePiece * pv, char const * propertyName );
	/**
		Creates a menu of integer valures, in the range (from,to), in intervales
		of the given step. e.g. (from=1,to=10,step=2) is a list of (1,3,5,7,9).
		When triggered,
		pv->piece()->setPieceProperty(propertyName,selectedValue) is called.
		
		lbl is the name used for the menu. It is required because this function can provide
		no sensible default.

		The caller owns the returned menu.
	*/
	static PiecePropertyMenu * makeNumberListMenu( char const * lbl, QGIGamePiece * pv, char const * propertyName, int from, int to, int step = 1 );
private:
	QGIGamePiece * m_view;
	QString m_key;
};


#endif
