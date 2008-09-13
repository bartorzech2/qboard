#ifndef QTSCRIPTSHELL_QSQLTABLEMODEL_H
#define QTSCRIPTSHELL_QSQLTABLEMODEL_H

#include <qsqltablemodel.h>

#include <QtScript/qscriptvalue.h>

class QtScriptShell_QSqlTableModel : public QSqlTableModel
{
public:
    QtScriptShell_QSqlTableModel(QObject*  parent = 0, QSqlDatabase  db = QSqlDatabase());
    ~QtScriptShell_QSqlTableModel();

    void clear();
    bool  deleteRowFromTable(int  row);
    bool  insertRowIntoTable(const QSqlRecord&  values);
    QString  orderByClause() const;
    void queryChange();
    void revertRow(int  row);
    bool  select();
    QString  selectStatement() const;
    void setEditStrategy(QSqlTableModel::EditStrategy  strategy);
    void setFilter(const QString&  filter);
    void setSort(int  column, Qt::SortOrder  order);
    void setTable(const QString&  tableName);
    bool  updateRowInTable(int  row, const QSqlRecord&  values);

    QScriptValue __qtscript_self;
};

#endif // QTSCRIPTSHELL_QSQLTABLEMODEL_H
