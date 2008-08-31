#ifndef QBOARD_JSGameState_H_INCLUDED
#define QBOARD_JSGameState_H_INCLUDED 1
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
#include <QScriptable>
class GameState;
class JSGameState : public QObject,
		    public QScriptable
{
Q_OBJECT
public:
    explicit JSGameState( QObject * parent = 0 );
    virtual ~JSGameState();

public Q_SLOTS:
    void bogo();

private:
    struct Impl;
    Impl * impl;
    GameState * self();
};

#endif // QBOARD_JSGameState_H_INCLUDED
