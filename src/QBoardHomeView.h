#ifndef QBOARD_HOMEVIEW_H_INCLUDED
#define QBOARD_HOMEVIEW_H_INCLUDED 1

#include <QTreeView>
#include <QWidget>
#include <QObject>
#include <QFileIconProvider>
class QFileInfo;
class QFileIconProvider;

class QBoardFileIconProvider : public QFileIconProvider
{
public:
    QBoardFileIconProvider();
    virtual ~QBoardFileIconProvider();
    virtual QIcon icon( const QFileInfo & info ) const;
};

class QBoardHomeView : public QTreeView
{
Q_OBJECT
public:
    QBoardHomeView( QWidget * parent = 0 );
    virtual ~QBoardHomeView();
    QFileIconProvider * iconProvider();

Q_SIGNALS:
    void itemActivated( QFileInfo const & );
protected:
    virtual void mouseDoubleClickEvent( QMouseEvent * event );
private Q_SLOTS:
    void currentChanged( const QModelIndex & current, const QModelIndex & previous );
private:
    struct Impl;
    Impl * impl;
};

#endif // QBOARD_HOMEVIEW_H_INCLUDED
