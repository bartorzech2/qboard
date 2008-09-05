#ifndef QBOARD_HOMEVIEW_H_INCLUDED
#define QBOARD_HOMEVIEW_H_INCLUDED 1

#include <QTreeView>
#include <QWidget>
#include <QObject>
#include <QFileIconProvider>
class QFileInfo;
class QFileIconProvider;

/**
   A QFileIconProvider implementation for QBoard widgets.
*/
class QBoardFileIconProvider : public QFileIconProvider
{
public:
    QBoardFileIconProvider();
    virtual ~QBoardFileIconProvider();
    virtual QIcon icon( const QFileInfo & info ) const;
};

/**
   A filesystem view which restricts the user to browsing
   QBoard home directory. It uses QBoardFileIconProvider
   to determine what icons to use.
*/
class QBoardHomeView : public QTreeView
{
Q_OBJECT
public:
    QBoardHomeView( QWidget * parent = 0 );
    virtual ~QBoardHomeView();
    QFileIconProvider * iconProvider();

public Q_SLOTS:
    /**
       Refreshes the view. Call this when your app has created new
       files.

       TODO?: add a path argument, to restrict the refresh to a
       specific dir.
     */
    void refresh();
Q_SIGNALS:
   /**
      Emited when an item is double-clicked.

      TODO: also emit when Enter is pressed.
    */
    void itemActivated( QFileInfo const & );
protected:
    /**
       Reimplemented to send the selected file's info to listeners. As
       a side-effect, double-clicking does not go into item-rename
       mode - use F2 for that.
     */
    virtual void mouseDoubleClickEvent( QMouseEvent * event );
    virtual void keyReleaseEvent( QKeyEvent * event );
private Q_SLOTS:
    void currentChanged( const QModelIndex & current, const QModelIndex & previous );
private:
    struct Impl;
    Impl * impl;
};

#endif // QBOARD_HOMEVIEW_H_INCLUDED
