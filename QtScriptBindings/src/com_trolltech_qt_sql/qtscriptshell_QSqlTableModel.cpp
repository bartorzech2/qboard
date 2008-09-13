#include "qtscriptshell_QSqlTableModel.h"

#include <QtScript/QScriptEngine>
#include <QSize>
#include <QSqlIndex>
#include <QStringList>
#include <QVariant>
#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlindex.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#define QTSCRIPT_IS_GENERATED_FUNCTION(fun) ((fun.data().toUInt32() & 0xFFFF0000) == 0xBABE0000)

Q_DECLARE_METATYPE(QSqlRecord)
Q_DECLARE_METATYPE(QSqlTableModel::EditStrategy)
Q_DECLARE_METATYPE(Qt::SortOrder)

QtScriptShell_QSqlTableModel::QtScriptShell_QSqlTableModel(QObject*  parent, QSqlDatabase  db)
    : QSqlTableModel(parent, db) {}

QtScriptShell_QSqlTableModel::~QtScriptShell_QSqlTableModel() {}

void QtScriptShell_QSqlTableModel::clear()
{
    QScriptValue _q_function = __qtscript_self.property("clear");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("clear") & QScriptValue::QObjectMember)) {
        QSqlTableModel::clear();
    } else {
        _q_function.call(__qtscript_self);
    }
}

bool  QtScriptShell_QSqlTableModel::deleteRowFromTable(int  row)
{
    QScriptValue _q_function = __qtscript_self.property("deleteRowFromTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("deleteRowFromTable") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::deleteRowFromTable(row);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row)));
    }
}

bool  QtScriptShell_QSqlTableModel::insertRowIntoTable(const QSqlRecord&  values)
{
    QScriptValue _q_function = __qtscript_self.property("insertRowIntoTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("insertRowIntoTable") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::insertRowIntoTable(values);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, values)));
    }
}

QString  QtScriptShell_QSqlTableModel::orderByClause() const
{
    QScriptValue _q_function = __qtscript_self.property("orderByClause");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("orderByClause") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::orderByClause();
    } else {
        return qscriptvalue_cast<QString >(_q_function.call(__qtscript_self));
    }
}

void QtScriptShell_QSqlTableModel::queryChange()
{
    QScriptValue _q_function = __qtscript_self.property("queryChange");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("queryChange") & QScriptValue::QObjectMember)) {
        QSqlTableModel::queryChange();
    } else {
        _q_function.call(__qtscript_self);
    }
}

void QtScriptShell_QSqlTableModel::revertRow(int  row)
{
    QScriptValue _q_function = __qtscript_self.property("revertRow");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("revertRow") & QScriptValue::QObjectMember)) {
        QSqlTableModel::revertRow(row);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row));
    }
}

bool  QtScriptShell_QSqlTableModel::select()
{
    QScriptValue _q_function = __qtscript_self.property("select");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("select") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::select();
    } else {
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self));
    }
}

QString  QtScriptShell_QSqlTableModel::selectStatement() const
{
    QScriptValue _q_function = __qtscript_self.property("selectStatement");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("selectStatement") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::selectStatement();
    } else {
        return qscriptvalue_cast<QString >(_q_function.call(__qtscript_self));
    }
}

void QtScriptShell_QSqlTableModel::setEditStrategy(QSqlTableModel::EditStrategy  strategy)
{
    QScriptValue _q_function = __qtscript_self.property("setEditStrategy");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setEditStrategy") & QScriptValue::QObjectMember)) {
        QSqlTableModel::setEditStrategy(strategy);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, strategy));
    }
}

void QtScriptShell_QSqlTableModel::setFilter(const QString&  filter)
{
    QScriptValue _q_function = __qtscript_self.property("setFilter");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setFilter") & QScriptValue::QObjectMember)) {
        QSqlTableModel::setFilter(filter);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, filter));
    }
}

void QtScriptShell_QSqlTableModel::setSort(int  column, Qt::SortOrder  order)
{
    QScriptValue _q_function = __qtscript_self.property("setSort");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setSort") & QScriptValue::QObjectMember)) {
        QSqlTableModel::setSort(column, order);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, column)
            << qScriptValueFromValue(_q_engine, order));
    }
}

void QtScriptShell_QSqlTableModel::setTable(const QString&  tableName)
{
    QScriptValue _q_function = __qtscript_self.property("setTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setTable") & QScriptValue::QObjectMember)) {
        QSqlTableModel::setTable(tableName);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, tableName));
    }
}

bool  QtScriptShell_QSqlTableModel::updateRowInTable(int  row, const QSqlRecord&  values)
{
    QScriptValue _q_function = __qtscript_self.property("updateRowInTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("updateRowInTable") & QScriptValue::QObjectMember)) {
        return QSqlTableModel::updateRowInTable(row, values);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row)
            << qScriptValueFromValue(_q_engine, values)));
    }
}

