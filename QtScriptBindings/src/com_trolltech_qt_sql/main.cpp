#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include <qsqlrecord.h>
#include <qsqlrelationaltablemodel.h>
#include <qsql.h>
#include <qsqlerror.h>
#include <qsqlresult.h>
#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qsqlfield.h>
#include <qsqldatabase.h>
#include <qsqlquerymodel.h>
#include <qsqldriver.h>
#include <qsqlindex.h>
#include <qsqltablemodel.h>
#include <qsqlrelationaltablemodel.h>

QScriptValue qtscript_create_QSqlRecord_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlRelation_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSql_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlError_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlResult_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlQuery_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlDriverCreatorBase_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlField_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlDatabase_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlQueryModel_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlDriver_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlIndex_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlTableModel_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSqlRelationalTableModel_class(QScriptEngine *engine);

static const char * const qtscript_com_trolltech_qt_sql_class_names[] = {
    "QSqlRecord"
    , "QSqlRelation"
    , "QSql"
    , "QSqlError"
    , "QSqlResult"
    , "QSqlQuery"
    , "QSqlDriverCreatorBase"
    , "QSqlField"
    , "QSqlDatabase"
    , "QSqlQueryModel"
    , "QSqlDriver"
    , "QSqlIndex"
    , "QSqlTableModel"
    , "QSqlRelationalTableModel"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator qtscript_com_trolltech_qt_sql_class_functions[] = {
    qtscript_create_QSqlRecord_class
    , qtscript_create_QSqlRelation_class
    , qtscript_create_QSql_class
    , qtscript_create_QSqlError_class
    , qtscript_create_QSqlResult_class
    , qtscript_create_QSqlQuery_class
    , qtscript_create_QSqlDriverCreatorBase_class
    , qtscript_create_QSqlField_class
    , qtscript_create_QSqlDatabase_class
    , qtscript_create_QSqlQueryModel_class
    , qtscript_create_QSqlDriver_class
    , qtscript_create_QSqlIndex_class
    , qtscript_create_QSqlTableModel_class
    , qtscript_create_QSqlRelationalTableModel_class
};

class com_trolltech_qt_sql_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList com_trolltech_qt_sql_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qt");
    list << QLatin1String("qt.sql");
    return list;
}

void com_trolltech_qt_sql_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qt")) {
    } else if (key == QLatin1String("qt.sql")) {
        QScriptValue extensionObject = engine->globalObject();
        for (int i = 0; i < 14; ++i) {
            extensionObject.setProperty(qtscript_com_trolltech_qt_sql_class_names[i],
                qtscript_com_trolltech_qt_sql_class_functions[i](engine),
                QScriptValue::SkipInEnumeration);
        }
    } else {
        Q_ASSERT_X(false, "com_trolltech_qt_sql::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(com_trolltech_qt_sql_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_com_trolltech_qt_sql, com_trolltech_qt_sql_ScriptPlugin)

