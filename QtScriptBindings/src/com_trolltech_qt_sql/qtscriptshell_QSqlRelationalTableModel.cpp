#include "qtscriptshell_QSqlRelationalTableModel.h"

#include <QtScript/QScriptEngine>
#include <QSize>
#include <QStringList>
#include <QVariant>
#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlindex.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlrelationaltablemodel.h>
#include <qsqltablemodel.h>

#define QTSCRIPT_IS_GENERATED_FUNCTION(fun) ((fun.data().toUInt32() & 0xFFFF0000) == 0xBABE0000)

Q_DECLARE_METATYPE(QSqlRecord)
Q_DECLARE_METATYPE(QSqlTableModel*)
Q_DECLARE_METATYPE(QSqlTableModel::EditStrategy)
Q_DECLARE_METATYPE(QSqlRelation)
Q_DECLARE_METATYPE(Qt::SortOrder)

QtScriptShell_QSqlRelationalTableModel::QtScriptShell_QSqlRelationalTableModel(QObject*  parent, QSqlDatabase  db)
    : QSqlRelationalTableModel(parent, db) {}

QtScriptShell_QSqlRelationalTableModel::~QtScriptShell_QSqlRelationalTableModel() {}

void QtScriptShell_QSqlRelationalTableModel::clear()
{
    QScriptValue _q_function = __qtscript_self.property("clear");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("clear") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::clear();
    } else {
        _q_function.call(__qtscript_self);
    }
}

bool  QtScriptShell_QSqlRelationalTableModel::deleteRowFromTable(int  row)
{
    QScriptValue _q_function = __qtscript_self.property("deleteRowFromTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("deleteRowFromTable") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::deleteRowFromTable(row);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row)));
    }
}

bool  QtScriptShell_QSqlRelationalTableModel::insertRowIntoTable(const QSqlRecord&  values)
{
    QScriptValue _q_function = __qtscript_self.property("insertRowIntoTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("insertRowIntoTable") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::insertRowIntoTable(values);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, values)));
    }
}

QString  QtScriptShell_QSqlRelationalTableModel::orderByClause() const
{
    QScriptValue _q_function = __qtscript_self.property("orderByClause");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("orderByClause") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::orderByClause();
    } else {
        return qscriptvalue_cast<QString >(_q_function.call(__qtscript_self));
    }
}

void QtScriptShell_QSqlRelationalTableModel::queryChange()
{
    QScriptValue _q_function = __qtscript_self.property("queryChange");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("queryChange") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::queryChange();
    } else {
        _q_function.call(__qtscript_self);
    }
}

QSqlTableModel*  QtScriptShell_QSqlRelationalTableModel::relationModel(int  column) const
{
    QScriptValue _q_function = __qtscript_self.property("relationModel");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("relationModel") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::relationModel(column);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<QSqlTableModel* >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, column)));
    }
}

void QtScriptShell_QSqlRelationalTableModel::revertRow(int  row)
{
    QScriptValue _q_function = __qtscript_self.property("revertRow");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("revertRow") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::revertRow(row);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row));
    }
}

bool  QtScriptShell_QSqlRelationalTableModel::select()
{
    QScriptValue _q_function = __qtscript_self.property("select");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("select") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::select();
    } else {
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self));
    }
}

QString  QtScriptShell_QSqlRelationalTableModel::selectStatement() const
{
    QScriptValue _q_function = __qtscript_self.property("selectStatement");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("selectStatement") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::selectStatement();
    } else {
        return qscriptvalue_cast<QString >(_q_function.call(__qtscript_self));
    }
}

void QtScriptShell_QSqlRelationalTableModel::setEditStrategy(QSqlTableModel::EditStrategy  strategy)
{
    QScriptValue _q_function = __qtscript_self.property("setEditStrategy");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setEditStrategy") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::setEditStrategy(strategy);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, strategy));
    }
}

void QtScriptShell_QSqlRelationalTableModel::setFilter(const QString&  filter)
{
    QScriptValue _q_function = __qtscript_self.property("setFilter");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setFilter") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::setFilter(filter);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, filter));
    }
}

void QtScriptShell_QSqlRelationalTableModel::setRelation(int  column, const QSqlRelation&  relation)
{
    QScriptValue _q_function = __qtscript_self.property("setRelation");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setRelation") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::setRelation(column, relation);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, column)
            << qScriptValueFromValue(_q_engine, relation));
    }
}

void QtScriptShell_QSqlRelationalTableModel::setSort(int  column, Qt::SortOrder  order)
{
    QScriptValue _q_function = __qtscript_self.property("setSort");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setSort") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::setSort(column, order);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, column)
            << qScriptValueFromValue(_q_engine, order));
    }
}

void QtScriptShell_QSqlRelationalTableModel::setTable(const QString&  tableName)
{
    QScriptValue _q_function = __qtscript_self.property("setTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("setTable") & QScriptValue::QObjectMember)) {
        QSqlRelationalTableModel::setTable(tableName);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        _q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, tableName));
    }
}

bool  QtScriptShell_QSqlRelationalTableModel::updateRowInTable(int  row, const QSqlRecord&  values)
{
    QScriptValue _q_function = __qtscript_self.property("updateRowInTable");
    if (!_q_function.isFunction() || QTSCRIPT_IS_GENERATED_FUNCTION(_q_function)
        || (__qtscript_self.propertyFlags("updateRowInTable") & QScriptValue::QObjectMember)) {
        return QSqlRelationalTableModel::updateRowInTable(row, values);
    } else {
        QScriptEngine *_q_engine = __qtscript_self.engine();
        return qscriptvalue_cast<bool >(_q_function.call(__qtscript_self,
            QScriptValueList()
            << qScriptValueFromValue(_q_engine, row)
            << qScriptValueFromValue(_q_engine, values)));
    }
}

