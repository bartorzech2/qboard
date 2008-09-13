#ifndef QTSCRIPTSHELL_QSQLQUERYMODEL_H
#define QTSCRIPTSHELL_QSQLQUERYMODEL_H

#include <qsqlquerymodel.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_QSqlQueryModel : public QSqlQueryModel
{
public:
    QtScriptShell_QSqlQueryModel(QObject*  parent = 0);
    ~QtScriptShell_QSqlQueryModel();

    void clear();
    void queryChange();

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_QSQLQUERYMODEL_H
