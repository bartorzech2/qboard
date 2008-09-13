#include "qtscriptshell_QSqlQueryModel.h"

#include <QtScript/QScriptEngine>
#include <QSize>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>
#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#define QTSCRIPT_IS_GENERATED_FUNCTION(fun) ((fun.data().toUInt32() & 0xFFFF0000) == 0xBABE0000)


QtScriptShell_QSqlQueryModel::QtScriptShell_QSqlQueryModel(QObject*  parent)
    : QSqlQueryModel(parent) {}

QtScriptShell_QSqlQueryModel::~QtScriptShell_QSqlQueryModel() {}

void QtScriptShell_QSqlQueryModel::clear()
{
    QScriptValue _q_function = __qtscript_self.property("clear");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("clear") & QScriptValue::QObjectMember)) {
        QSqlQueryModel::clear();
    } else {
        _q_function.call(__qtscript_self);
    }
}

void QtScriptShell_QSqlQueryModel::queryChange()
{
    QScriptValue _q_function = __qtscript_self.property("queryChange");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("queryChange") & QScriptValue::QObjectMember)) {
        QSqlQueryModel::queryChange();
    } else {
        _q_function.call(__qtscript_self);
    }
}

