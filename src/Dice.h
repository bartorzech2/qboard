#ifndef QBOARD_Die_H_INCLUDED
#define QBOARD_Die_H_INCLUDED 1
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
#include "Serializable.h"
#include "QGI.h"
#include <QGraphicsTextItem>
#include <QObject>

namespace qboard {
class Die
{
public:
    Die( int sides = 6, int lowDigit = 1);
    ~Die();

    int roll();
    void sides( int );
    int sides() const;
    int low() const;
    void low( int );
private:
    struct Impl;
    Impl * impl;
};

struct Die_s11n
{
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, Die const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, Die & dest ) const;
};


class QGIDie : public QGraphicsTextItem,
	       public Serializable
{
Q_OBJECT
public:
	QGIDie();
	virtual ~QGIDie();
	/** Serializes this object to dest.
	*/
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
	virtual int type() const { return QGITypes::QGIDie; }
	QRectF boundingRect() const;
    QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool event( QEvent * e );
public Q_SLOTS:
    int roll();
protected:
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private Q_SLOTS:
    /** Handles property changes. */
    void propertySet( char const * key, QVariant const & val );

private:
    void updateText();
    struct Impl;
    Impl * impl;
};

class MenuHandlerDie : public QObject
{
Q_OBJECT
public:
	MenuHandlerDie();
	virtual ~MenuHandlerDie();
public Q_SLOTS:
	void doMenu( QGIDie *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
    /*
     */
    void showHelp();
private:
	struct Impl;
	Impl * impl;
};

} // namespace

// Register QGIDie with s11n:
#define S11N_TYPE qboard::QGIDie
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QGIDie"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE qboard::QGIDie
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#define S11N_TYPE_NAME "QGIDie"
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#define S11N_TYPE qboard::Die
#define S11N_TYPE_NAME "Die"
#define S11N_SERIALIZE_FUNCTOR qboard::Die_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>


#endif // QBOARD_Die_H_INCLUDED
