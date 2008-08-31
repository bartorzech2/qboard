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

#include "JSGameState.h"
#include "GameState.h"
#include <QDebug>
#include <QScriptValue>
#include <QScriptEngine>

struct JSGameState::Impl
{
    Impl()
    {
    }
    ~Impl()
    {
    }
};

JSGameState::JSGameState( QObject * parent )
    : QObject(parent),
      QScriptable(),
      impl(new Impl)
{
}

JSGameState::~JSGameState()
{
    delete impl;
}

void JSGameState::bogo()
{
    qDebug() << "JSGameState::bogo(): self =="<<this->self();
}


GameState * JSGameState::self()
{
    return qscriptvalue_cast<GameState*>(thisObject());
}
