#ifndef QTSCRIPTSHELL_QSQLRELATIONALTABLEMODEL_H
#define QTSCRIPTSHELL_QSQLRELATIONALTABLEMODEL_H

#include <qsqlrelationaltablemodel.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_QSqlRelationalTableModel : public QSqlRelationalTableModel
{
public:
    QtScriptShell_QSqlRelationalTableModel(QObject*  parent = 0, QSqlDatabase  db = QSqlDatabase());
    ~QtScriptShell_QSqlRelationalTableModel();

    void clear();
    bool  deleteRowFromTable(int  row);
    bool  insertRowIntoTable(const QSqlRecord&  values);
    QString  orderByClause() const;
    void queryChange();
    QSqlTableModel*  relationModel(int  column) const;
    void revertRow(int  row);
    bool  select();
    QString  selectStatement() const;
    void setEditStrategy(QSqlTableModel::EditStrategy  strategy);
    void setFilter(const QString&  filter);
    void setRelation(int  column, const QSqlRelation&  relation);
    void setSort(int  column, Qt::SortOrder  order);
    void setTable(const QString&  tableName);
    bool  updateRowInTable(int  row, const QSqlRecord&  values);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_QSQLRELATIONALTABLEMODEL_H
