#ifndef QGIPIECEPLACEMARKER_H_INCLUDED
#define QGIPIECEPLACEMARKER_H_INCLUDED
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
#include <QVariant>
#include <QPointF>
#include <QPair>
#include <QList>
#include <QPainterPath>
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QGraphicsSceneMouseEvent;
class QMoveEvent;

#include "Serializable.h"
#include "QGI.h"
class GameState;

#define QGIPiecePlacemarker_IS_SERIALIZABLE 0
/**
	QGIPiecePlacemarker
*/
class QGIPiecePlacemarker : public QObject, public QGraphicsItem
#if QGIPiecePlacemarker_IS_SERIALIZABLE
, public Serializable
#endif
{
Q_OBJECT
public:
	QGIPiecePlacemarker();
	virtual ~QGIPiecePlacemarker();
#if QGIPiecePlacemarker_IS_SERIALIZABLE
	/** Serializes this object to dest. */
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
#endif
	QRectF boundingRect() const;
// 	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  	virtual bool event( QEvent * e );

//     void setGameState( GameState & gs );

    virtual int type() const { return QGITypes::QGIPiecePlacemarker; }

protected:
	//virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
// 	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
// 	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
 	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    struct Impl;
    Impl * impl;
};

#if QGIPiecePlacemarker_IS_SERIALIZABLE
// Register QGIPiecePlacemarker with s11n:
#define S11N_TYPE QGIPiecePlacemarker
#define S11N_TYPE_NAME "QGIPiecePlacemarker"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIPiecePlacemarker
#define S11N_TYPE_NAME "QGIPiecePlacemarker"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // QGIPiecePlacemarker_IS_SERIALIZABLE

#endif
