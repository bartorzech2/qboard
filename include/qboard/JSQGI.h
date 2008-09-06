#ifndef QBOARD_JSQGI_H_INCLUDED
#define QBOARD_JSQGI_H_INCLUDED 1
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
#include <QPointF>
#include <QScriptable>
#include <QScriptValue>
class QGraphicsItem;

namespace qboard {
class JSQGI : public QObject,
	      public QScriptable
{
Q_OBJECT
public:
    JSQGI( QObject * parent = 0 );
    virtual ~JSQGI();

    Q_INVOKABLE QList<QGraphicsItem*> childItems();


public Q_SLOTS:
    QPointF pos(); // why is this not seen script-side?
    /**
       Moves this object to the given coords.
    */
    void move( qreal x, qreal y );
    void move( QPointF const & p );
    qreal posX();
    qreal posY();
    //QPointF pos();
    QScriptValue prop(QString const & key);
    bool prop(QString const & key, QScriptValue const & val );
private:
    QGraphicsItem * self();
    struct Impl;
    Impl * impl;
};

} // namespace
#endif // QBOARD_JSQGI_H_INCLUDED
