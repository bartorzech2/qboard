#include "QBoardHomeView.h"

#include <QItemSelectionModel>
#include <QDirModel>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QKeyEvent>

#include "utility.h"

// If QBHomeView_USE_DIRICON is a string then: if a dir entry has a file named that
// string (e.g. ".diricon.png") then that icon is used in place of the default one.
// To turn this of, undefine QBHomeView_USE_DIRICON 
#define QBHomeView_USE_DIRICON ".diricon.png"
#ifdef QBHomeView_USE_DIRICON
#include <QRegExp>
#endif

// If QBHomeView_GENERATE_MINIICONS is true then code is enabled which tries to
// generate preview icons for "small" image files. Whe a list entry has the
// suffix 'png' or 'xpm' and that file is under a certain size (e.g. 8k) then
// it is loaded as a pixmap, scaled to 16x16, and used as its own icon.
#define QBHomeView_GENERATE_MINIICONS 1
#if QBHomeView_GENERATE_MINIICONS
#include <QPixmapCache>
#include <QRegExp>
#endif

#define QBHomeView_USE_QBOARD_FILEEXT_RESOURCES 1
#if QBHomeView_USE_QBOARD_FILEEXT_RESOURCES
#include <QRegExp>
#endif



QBoardFileIconProvider::QBoardFileIconProvider() :
    QFileIconProvider()
{
}

QBoardFileIconProvider::~QBoardFileIconProvider()
{}

QIcon QBoardFileIconProvider::icon( const QFileInfo & info ) const
{
#ifdef QBHomeView_USE_DIRICON
    if( info.isDir() )
    {
	//QString check( QDir::toNativeSeparators(info.canonicalFilePath()+"/dir
	QDir dir(info.canonicalFilePath());
	if( dir.exists(QBHomeView_USE_DIRICON) )
	{
	    return QIcon(QDir::toNativeSeparators(dir.filePath(QBHomeView_USE_DIRICON)));
	}
    }
#endif // QBHomeView_USE_DIRICON
#if QBHomeView_GENERATE_MINIICONS
    static const int thresh = 8 * 1024;
    if( (info.size()<thresh)
	&& (0 == QRegExp("(png|xpm)",Qt::CaseInsensitive).indexIn(info.suffix()) )
	)
    {
	QString key( info.canonicalFilePath() );
	QPixmap pix;
	if( ! QPixmapCache::find(key,pix) )
	{
	    if( pix.load( key ) )
	    {
		pix = pix.scaled(16,16);
		QPixmapCache::insert(key,pix);
	    }
	}
	if( ! pix.isNull() )
	{
	    return QIcon(pix);
	}
    }
#endif // QBHomeView_GENERATE_MINIICONS
#if QBHomeView_USE_QBOARD_FILEEXT_RESOURCES
    if( ! info.isDir() )
    {
	QIcon ico( QString( ":/QBoard/icon/fileext/%1.png" ).
		   arg(info.completeSuffix()) );
	if( ! ico.isNull() )
	{
	    return ico;
	}
    }
#endif // QBHomeView_USE_QBOARD_FILEEXT_RESOURCES
    return this->QFileIconProvider::icon(info);
}


struct QBoardHomeView::Impl
{
    QDirModel * model;
    QItemSelectionModel * sel;
    QModelIndex current;
    static QBoardFileIconProvider * iconer;
    Impl() : model( new QDirModel ),
	     sel( new QItemSelectionModel(model) ),
	     current()
    {
	++instCount;
	if( ! iconer )
	{
	    iconer = new QBoardFileIconProvider;
	}
	model->setIconProvider( iconer );
	model->setSorting( QDir::DirsFirst | QDir::IgnoreCase );
	model->setReadOnly( false );
    }
    ~Impl()
    {
	delete sel;
	delete model;
	if( 0 == --instCount )
	{
	    delete iconer;
	    iconer = 0;
	}
    }
private:
    static size_t instCount;
};
size_t QBoardHomeView::Impl::instCount = 0;
QBoardFileIconProvider * QBoardHomeView::Impl::iconer = 0;

QBoardHomeView::QBoardHomeView( QWidget * parent ) :
    QTreeView(parent),
    impl(new Impl)
{
    this->setModel( impl->model );
    for( int i = 1; i < 4; ++i )
    {
	this->setColumnHidden(i, true);
    }
    this->setUniformRowHeights(true);
#if QT_VERSION >= 0x040400
    this->QTreeView::setHeaderHidden(true);
#endif
    connect( impl->sel, SIGNAL(currentChanged( const QModelIndex &, const QModelIndex & )),
	     this, SLOT(currentChanged( const QModelIndex &, const QModelIndex & )));
    //this->setSortingEnabled(true);
    this->setRootIndex( impl->model->index(qboard::home().absolutePath()) );
}

QBoardHomeView::~QBoardHomeView()
{
    delete impl;
}

void QBoardHomeView::mouseDoubleClickEvent( QMouseEvent * event )
{
    /**
       This readOnly flag kludge is to keep a double-click from
       causing the view to go into item-rename mode, but still allow
       renaming via the standard hotkey F2.
    */
    impl->model->setReadOnly( true );
    this->QTreeView::mouseDoubleClickEvent(event);
    impl->model->setReadOnly( false );
    if(0) qDebug() << "QBoardHomeView::mouseDoubleClickEvent(): "
		   << impl->model->filePath(impl->current);
    QFileInfo fi( impl->model->fileInfo(impl->current) );
    if( fi.isFile() )
    {
	emit itemActivated( fi );
    }
}

void QBoardHomeView::keyReleaseEvent( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Delete )
    {
	QModelIndex cur = impl->current;
	QString fn = qboard::homeRelative( impl->model->filePath(cur) );
	QString msg = QString("Please confirm deletion of file:\n%1").
	    arg(fn);
	event->accept();
	int rc = QMessageBox::question( this,
					"Delete file?",
					msg,
					QMessageBox::Ok
					| QMessageBox::Cancel );
	if( QMessageBox::Ok == rc )
	{
	    impl->model->remove( cur );
	}
	return;
    }
    return QTreeView::keyReleaseEvent(event);
}
void QBoardHomeView::refresh()
{
    QModelIndex h = impl->model->index(qboard::home().absolutePath());
    this->setRootIndex( h );
    impl->model->refresh(h); // causes a segfault when called during ctor
}

void QBoardHomeView::currentChanged( const QModelIndex & current,
				     const QModelIndex & /* previous */ )
{
    impl->current = current;
    if(0) qDebug() << "QBoardHomeView::currentChanged(): "
		   << impl->model->filePath(current);
}
