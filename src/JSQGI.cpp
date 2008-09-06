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

#include <qboard/JSQGI.h>
#include <QGraphicsItem>
#include <QScriptEngine>
#include <QScriptValue>

#define SELF(RV) QGraphicsItem *self = this->self(); \
    QScriptEngine * js = this->engine(); \
    if(!self || !js) return RV;

namespace qboard {
struct JSQGI::Impl
{
    Impl()
    {
    }
    ~Impl()
    {
    }
};

JSQGI::JSQGI( QObject * parent )
    : QObject( parent ),
      QScriptable(),
      impl(new Impl)
{
}

JSQGI::~JSQGI()
{
    delete impl;
}

QPointF JSQGI::pos()
{
    SELF(QPointF());
    return self->pos();
}


void JSQGI::move( QPointF const & p )
{
    SELF();
    self->setPos(p);
}

void JSQGI::move( qreal x, qreal y )
{
    this->move( QPointF(x,y) );
}

qreal JSQGI::posX()
{
    SELF(0.0);
    return self->pos().x();
}
qreal JSQGI::posY()
{
    SELF(0.0);
    return self->pos().y();
}


QGraphicsItem * JSQGI::self()
{
    QGraphicsItem * s = qscriptvalue_cast<QGraphicsItem*>(thisObject());
    if( ! s )
    {
	this->context()->throwError("JSQGI::self() got a null pointer.");
    }
    return s;
}

} // namespace
#undef SELF
