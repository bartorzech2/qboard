#ifndef QBOARD_ScriptQt_H_INCLUDED
#define QBOARD_ScriptQt_H_INCLUDED 1

#include <QObject>
#include "Serializable.h"
#include <QScriptEngine>
#include <QScriptValue>
namespace qboard {

    QScriptEngine * createScriptEngine( QObject * parent = 0 );

}

#endif // QBOARD_ScriptQt_H_INCLUDED
