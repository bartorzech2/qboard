#ifndef QGILINE_H_INCLUDED
#define QGILINE_H_INCLUDED
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

#include <QGraphicsTextItem>
#include "Serializable.h"
#include "QGI.h"
#include <QVariant>
#include <QPointF>
#include <QPair>
#include <QList>
#include <QPainterPath>
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QGraphicsSceneMouseEvent;

class QGILineNode;
class QGILineBinder : public QObject, public QGraphicsItem, public Serializable
{
Q_OBJECT
public:
	QGILineBinder();
	QGILineBinder( QGILineNode * left, QGILineNode * right );
	virtual ~QGILineBinder();
	/** Serializes this object to dest. */
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
	virtual int type() const { return QGITypes::LineNodeBinder; }
	QGILineNode * first();
	QGILineNode * second();
	void first(QGILineNode * );
	void second(QGILineNode * );
	void adjust();
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual QPainterPath shape() const;
	virtual bool event( QEvent * e );
public Q_SLOTS:
	void destroyLine();
	void toggleArrows();
protected:
	virtual QRectF boundingRect() const;
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	//virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
	//virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	//virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
private:
	bool isValid();
	void setup();
	struct Impl;
	Impl * impl;
	friend class QGILineNode;
};
#define S11N_TYPE QGILineBinder
#define S11N_TYPE_NAME "QGILineBinder"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGILineNode
#define S11N_TYPE_NAME "QGILineNode"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>



#define QGILineNode_IS_SERIALIZABLE 0
/**
	QGILineNode implements an endpoint for an on-board line.
	It cannot be used standalone - it must be used in pairs
	along with a QGILineBinder.
*/
class QGILineNode : public QObject, public QGraphicsItem
#if QGILineNode_IS_SERIALIZABLE
, public Serializable
#endif
{
Q_OBJECT
public:
	QGILineNode();
	virtual ~QGILineNode();
#if QGILineNode_IS_SERIALIZABLE
	/** Serializes this object to dest. The following properties are saved:
		position, text (as HTML), QObject "dynamic" properties.
	*/
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
#endif
	virtual int type() const { return QGITypes::LineNode; }
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void setLine( QGILineBinder * );
	QGILineBinder * edge();
	virtual bool event( QEvent * e );
protected:
	//virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
	explicit QGILineNode( QGILineNode * buddy );
	bool m_active;
	//typedef QList<QGILineBinder*> EdgeList;
	//EdgeList m_ed;
	QGILineBinder * m_line;
	friend class QGILineBinder;
};

#if QGILineNode_IS_SERIALIZABLE
// Register QGILineNode with s11n:
#define S11N_TYPE QGILineNode
#define S11N_TYPE_NAME "QGILineNode"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGILineNode
#define S11N_TYPE_NAME "QGILineNode"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // QGILineNode_IS_SERIALIZABLE

/**
	This is the popup menu handler for QGILineNode objects.
	It is important that this object's doMenu() never be called
	more than once per popup iteration. The reason is because
	this object has to jump through some hoops to ensure proper
	destruction order for the 3 object related to a QGILineBinder,
	and that workaround requires that THIS object take over ownership
	of a QGILineNode if a Destroy menu operation is requested.
*/
class MenuHandlerLineNode : public QObject
{
Q_OBJECT
public:
	MenuHandlerLineNode();
	virtual ~MenuHandlerLineNode();
public Q_SLOTS:
	void doMenu( QGILineNode *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
	/*
	*/
       void showHelp();
	void scheduleLineDestruction();
private:
	struct Impl;
	Impl * impl;
};

#endif
