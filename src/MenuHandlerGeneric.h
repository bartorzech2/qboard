#ifndef __MENUHANDLERGENERIC_H__
#define __MENUHANDLERGENERIC_H__
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
class QGraphicsItem;
/**
	An internal helper type to implement destructions of all
	selected QGraphicsItems.
*/
class SceneSelectionDestroyer : public QObject
{
Q_OBJECT
public:
	/** This object's ownership is transfered to parent. qgi
	becomes the target of this object's slots when the attached
	QActions are fired.
	*/
	explicit SceneSelectionDestroyer( QObject * parent, QGraphicsItem * qgi );
	virtual ~SceneSelectionDestroyer();
public Q_SLOTS:
	void destroySelectedItems();
	void destroyItem();
	void destroyItem(QGraphicsItem *);
private:
	QGraphicsItem * gitem;
};

/**
	Implements some menu behaviours common to most
	QBoard-related QGraphicsItems
*/
class MenuHandlerCommon : public QObject
{
Q_OBJECT
public:
	MenuHandlerCommon(){}
	virtual ~MenuHandlerCommon(){}
	/**
		Creates a menu of common options. Caller owns the returned object.
		
		The menu will have a different title, depending on whether or not
		the item is selected. Menu handlers compatible with this are expected
		to honor the item->isSelected() flag and act on all selected items
		(insofar as possible) if it is set.

		Menu items currently include:
		
		- Destroy, which destroys the item and, if it's selected, all selected
		items.
		
		TODOs:
		
		- Copy/cut. This is tricky because only Serializables can be handled by
		our clipboard.
		
		- Paste, if the target supports it (but how to know that?).
	*/
	QMenu * createMenu( QGraphicsItem * ); //, QGraphicsSceneContextMenuEvent * );
};

/**
	Experimental. Don't use this.
*/
class MenuHandlerCopyCut : public QObject
{
Q_OBJECT
public:
	explicit MenuHandlerCopyCut(QGraphicsItem * gi,QObject * parent=0);
	virtual ~MenuHandlerCopyCut();
    QMenu * addDefaultEntries( QMenu *, bool copy = true, bool cut = true );
public Q_SLOTS:
	void clipboardCopy();
	void clipboardCut();
private Q_SLOTS:
    void copyList();
    void cutList();
    void copyOne();
    void cutOne();
    void cloneItem();
private:
    static void clipOne( QGraphicsItem *, bool copy );
    static void clipList( QGraphicsItem *, bool copy );
    static void clipboard( QGraphicsItem *, bool copy );
	QGraphicsItem * m_gi;
};


/**
	A QAction type which calls QObject::setProperty()
	when triggered.
*/
class QObjectPropertyAction : public QAction
{
Q_OBJECT
public:
	/** Sets this object's target to obj,
		with the given property name and value.
		parent is passed as-is to the QAction ctor.
		When this item is triggered,
		obj->setProperty(propName,val) is called.

		If obj is destroyed before this object, this action
		disconnects itself from obj and triggering the action
		becomes a no-op.
		
		Special cases:
		
		- If (val.type() == QVariant::Color) then
		a small "color splotch" icon of that color
		is added to this object.
	*/
	QObjectPropertyAction( QObject * obj,
			QString const & propName,
			QVariant const & val,
			QObject * parent = 0);

	QObjectPropertyAction( QList<QObject *> objs,
			QString const & propName,
			QVariant const & val,
			QObject * parent = 0);
	virtual ~QObjectPropertyAction();
private Q_SLOTS:
	void setProperty();
	void dtorDisconnect(QObject*);
private:
    void setup();
    struct Impl;
    Impl * impl;
};

/**
	A menu type to simplify the usage of QObjectPropertyAction.
*/
class QObjectPropertyMenu : public QMenu
{
Q_OBJECT
public:
	/**
		Creates a new menu with the given title and parent.
		When actions are added via addItem(QVariant,QString),
		they are QObjectPropertyActions which are connected to
		obj, using the given property name. When triggered,
		those actions will call obj->setProperty(propName,selectedValue),
		where selectedValue is the value passed to addItem().  
	*/
	QObjectPropertyMenu( const QString & title,
		QObject * obj,
		QString const & propName,
		QWidget * parent = 0 );
	QObjectPropertyMenu( const QString & title,
		QList<QObject *> objs,
		QString const & propName,
		QWidget * parent = 0 );
	virtual ~QObjectPropertyMenu();
	/**
		Creates a new QObjectPropertyAction which, when triggered, call
		qobj->setProperty(propName,val), using the QObject and propName
		set in this object's ctor. If lbl is empty then val.toString()
		is used to set the item label. It returns the action, which is
		owned by this object.
	*/
	QAction * addItem( QVariant const & val, QString const & lbl = QString() );

	/**
		Creates a menu of alpha level settings. When triggered,
		obj->setProperty(propertyName,selectedValue) is called.

		If hint is a valid color, each item in the menu has a color splotch
		of that color, with the appropriate alpha level applied.
		

		The caller owns the returned menu.
	*/
	static QObjectPropertyMenu * makeAlphaMenu( QObject * obj, char const * propertyName, QColor const & hint = QColor() );

	static QObjectPropertyMenu * makeAlphaMenu( QList<QObject *> objs, char const * propertyName, QColor const & hint = QColor() );

	/**
		Creates a menu of common colors. When triggered,
		obj->setProperty(propertyName,selectedValue) is called.
		If alphaName is non-null, a sub-menu is creates with alpha settings, and when triggered
		obj->setProperty(alphaName,selectedValue) is called.

		The caller owns the returned menu.
	*/
	static QObjectPropertyMenu * makeColorMenu( QObject * obj, char const * propertyName, char const * alphaName = 0 );

	static QObjectPropertyMenu * makeColorMenu( QList<QObject *> objs, char const * propertyName, char const * alphaName = 0 );

	/**
		Creates a menu of Qt::PenStyle entries. When triggered,
		obj->PieceProperty(propertyName,selectedValue) is called.

		The caller owns the returned menu.
	*/
	static QObjectPropertyMenu * makePenStyleMenu( QObject * obj, char const * propertyName );
	static QObjectPropertyMenu * makePenStyleMenu( QList<QObject *> objs, char const * propertyName );

	/**
		Creates a menu of integer valures, in the range (from,to), in intervales
		of the given step. e.g. (from=1,to=10,step=2) is a list of (1,3,5,7,9).
		When triggered,
		pv->piece()->setPieceProperty(propertyName,selectedValue) is called.
		
		lbl is the name used for the menu. It is required because this function can provide
		no sensible default.

		The caller owns the returned menu.
	*/
	static QObjectPropertyMenu * makeNumberListMenu( char const * lbl, QObject * obj, char const * propertyName, qreal from, qreal to, qreal step = 1 );

	static QObjectPropertyMenu * makeNumberListMenu( char const * lbl, QList<QObject *> objs, char const * propertyName, qreal from, qreal to, qreal step = 1 );

private:
    struct Impl;
    Impl * impl;
};

#endif // __MENUHANDLERGENERIC_H__
