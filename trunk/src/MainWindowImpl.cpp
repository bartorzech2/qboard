#include "MainWindowImpl.h"
//#include <QtScript>
#include <QGridLayout>
#include <QSplitter>
#include <QDockWidget>
#include <QDebug>
#include <QFileDialog>
#include <QSplitter>
#include <QScrollArea>
#include <QRegExp>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <s11n.net/s11n/s11nlite.hpp>
#if QT_VERSION >= 0x040400
#include "QBoardDocsBrowser.h"
#endif
#include "GamePiece.h"
#include "QBoardView.h"
#include "QGIGamePiece.h"
#include <cmath>
#include "QGIHtml.h"
#include "GameState.h"
#include "FileBrowser.h"
#include "QGILine.h"
#include "utility.h"
#include "S11nClipboard.h"
#include "QBBatch.h"
struct MainWindowImpl::Impl
{
	GameState gstate;
	QGraphicsView * gv;
	FileBrowser * fb;
	Impl() : gstate(),
		gv(0),
		fb(0)
	{
	}
	~Impl()
	{
		delete gv;
	}
};

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
    : QMainWindow(parent, f),
    impl(new Impl)
{
	setupUi(this);
	this->setWindowTitle( QString("QBoard v. %1").arg(qboard::versionString()) );
	connect( this->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveGame()) );
	connect( this->actionLoad, SIGNAL(triggered(bool)), this, SLOT(loadGame()) );
	connect( this->actionNewBoardView, SIGNAL(triggered(bool)), this, SLOT(launchNewBoardView()) );
	connect( this->actionHome, SIGNAL(triggered(bool)), this, SLOT(goHome()) );
	connect( this->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()) );
	connect( this->actionAboutQBoard, SIGNAL(triggered(bool)), this, SLOT(aboutQBoard()) );
	connect( this->actionPrint, SIGNAL(triggered(bool)), this, SLOT(printGame()) );
	connect( this->actionHelp, SIGNAL(triggered(bool)), this, SLOT(launchHelp()) );
	connect( this->actionClearBoard, SIGNAL(triggered(bool)), this, SLOT(clearBoard()) );
	connect( this->actionExperiment, SIGNAL(triggered(bool)), this, SLOT(doSomethingExperimental()) );
	connect( &S11nClipboard::instance(), SIGNAL(signalUpdated()), this, SLOT(clipboardUpdated()) );

	this->actionClearClipboard->setEnabled( 0 != S11nClipboard::instance().contents() );
	connect( this->actionClearClipboard, SIGNAL(triggered(bool)), this, SLOT( clearClipboard() ) );

#if 1
	QWidget * cli = this->clientArea;
	QLayout * lay = new QGridLayout( cli );
	lay->setSpacing(0);
	lay->setContentsMargins(2,2,2,2);
	QSplitter * splitter = new QSplitter( Qt::Horizontal, cli );
	lay->addWidget( splitter );
	splitter->setHandleWidth(8);
#else
	QSplitter * sp = splitter = this->clientArea;
#endif

	FileBrowser * fb = this->impl->fb = new FileBrowser( "*", 0 );
	splitter->addWidget(fb);
	connect( this->actionRefreshFileList, SIGNAL(triggered(bool)), fb, SLOT(reloadDir()) );
	connect( fb, SIGNAL(pickedFile(QFileInfo const &)), this, SLOT(loadFile(QFileInfo const &)) );
	//connect( fb, SIGNAL(pickedDir(QDir const &)), this, SLOT(chdir(QDir const &)) );
	connect( this->actionToggleBrowserView, SIGNAL(toggled(bool)), fb, SLOT(setVisible(bool)) );
	impl->gv = new QBoardView( impl->gstate.board(), impl->gstate.scene() );
	connect( this->actionToggleBoardDragMode, SIGNAL(toggled(bool)),
		impl->gv, SLOT(setHandDragMode(bool)) );
	this->actionToggleBoardDragMode->setChecked(false);
	connect( this->actionSelectAll, SIGNAL(triggered(bool)), impl->gv, SLOT(selectAll()) );

	connect( this->actionZoomIn, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomIn()) );
	connect( this->actionZoomOut, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomOut()) );	
	connect( this->actionZoomReset, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomReset()) );


	{
		QWidget * frenchy = new QWidget();
		QGridLayout * gl = new QGridLayout(frenchy);
		lay->setSpacing(0);
		lay->setContentsMargins(2,2,2,2);
		gl->addWidget(impl->gv);
		splitter->addWidget(frenchy);
	}

	splitter->setStretchFactor(0,1);
	splitter->setStretchFactor(1,3);
}
MainWindowImpl::~MainWindowImpl()
{
	delete impl;
}

void MainWindowImpl::clearClipboard()
{
    delete( S11nClipboard::instance().take() );
}

void MainWindowImpl::clipboardUpdated()
{
    QString msg("Clipboard updated: ");
    S11nNode const * c = S11nClipboard::instance().contents();
    if( !c )
    {
	msg += "cleared";
	this->actionClearClipboard->setEnabled( false );
    }
    else
    {
	msg += QString("type = ")+QString(S11nNodeTraits::class_name(*c).c_str());
	this->actionClearClipboard->setEnabled( true );
    }
    this->statusBar()->showMessage( msg );
}


#include "utility.h"
void MainWindowImpl::goHome()
{
	this->chdir( qboard::home() );
	impl->fb->setDir(".");
}
#include "AboutQBoardImpl.h"
void MainWindowImpl::aboutQBoard()
{
	AboutQBoardImpl dlg(this);
	dlg.exec();
}
void MainWindowImpl::chdir(const QDir & dir)
{
	QDir::setCurrent( dir.absolutePath() );
	// don't call impl->fb->setDir() from here. Endless loop.
}

void MainWindowImpl::launchHelp()
{
#if QT_VERSION >= 0x040400
	(new QBoardDocsBrowser)->show();
#else
	QMessageBox::warning( this, "Not implemented!",
			      "MainWindowImpl::launchHelp() not implemented for Qt < 4.4",
			      QMessageBox::Ok, QMessageBox::Ok );
#endif
}
bool MainWindowImpl::saveGame( QString const & fn )
{
    bool b = false;
    std::ostringstream os;
    try
    {
	b = impl->gstate.save( fn, true );
	os << "failed for unknown reason.";
    }
    catch( std::exception const & ex )
    {
	b = false;
	os << "Save failed! The error text is:\n"
	   << ex.what();
    }

    if( b )
    {
	    this->statusBar()->showMessage("Saved: "+fn);
    }
    else
    {
	this->statusBar()->showMessage("Save FAILED: "+fn);
	QMessageBox::warning( this, "Save failed!", os.str().c_str(),
			      QMessageBox::Ok, QMessageBox::Ok );
    }
    this->actionRefreshFileList->activate( QAction::Trigger );
    return b;
	
}
bool MainWindowImpl::saveGame()
{
	QString lbl = QString("QBoard Files (*%1)").arg(impl->gstate.s11nFileExtension());
	QString fn = QFileDialog::getSaveFileName(this,
		tr("Save Game"),
		"",
		lbl.toAscii() );
	return fn.isEmpty() ? false : this->saveGame(fn);
}

bool MainWindowImpl::loadFile( QFileInfo const & fi )
{
	// FIXME: this dispatch needs to be generalized.
	QString fn( fi.filePath() ); // fi.absoluteFilePath() );
	bool worked = false;
	GamePiece pc;
	GamePieceList gpl;
	if( pc.fileNameMatches(fn) )
	{
		worked = this->loadPiece(fi);
	}
	if( gpl.fileNameMatches(fn) )
	{
		worked = gpl.load(fn);
		if( worked )
		{
		    impl->gstate.pieces().takePieces(gpl);
		}
	}
	else if( impl->gstate.board().fileNameMatches(fn) )
	{
		worked = impl->gstate.board().load(fn);
	}
	else if( impl->gstate.fileNameMatches(fn) )
	{
		worked = this->loadGame(fn);
	}
	else if( QRegExp("\\.(html|txt)$",Qt::CaseInsensitive).indexIn(fn) > 0 )
	{
#if QT_VERSION >= 0x040400
		QBoardDocsBrowser * b = new QBoardDocsBrowser;
		b->setHome(fn);
		b->setWindowTitle(fn);
		b->show();
		worked = true;
#else
		QMessageBox::warning( this, "Not implemented!",
				      "MainWindowImpl::loadFile(*.{txt,html}) not implemented for Qt < 4.4",
				      QMessageBox::Ok, QMessageBox::Ok );
		worked = false;
#endif
	}
	else if( QRegExp("\\.(png|jpg|gif|xpm|svg|bmp)$",Qt::CaseInsensitive).indexIn(fn) > 0 )
	{ // ^^^ FIXME: get the list of image formats from somewhere dynamic
		const size_t threshold = 10 * 1024;
		// ^^^ fixme: ^^^^ do this based on image resolution, not file size, cuz many
		// large-sized PNGs are quite small.
		if( fi.size() < threshold )
		{
			worked = this->loadPiece(fi);
		}
		else
		{
			worked = this->impl->gstate.board().load(fn); // fi.absoluteFilePath());
		}
	}
	else if ( -1 != QRegExp("\\.QBBatch$", Qt::CaseInsensitive).indexIn(fn) )
	{
	    try
	    {
		QStringList li;
		li << "-v"
		   << fn;
		/**
		   FIXME: when we pass impl->gstate, it WORKS in
		   principal, but the pieces do not appear. They are
		   there - they can be serialized - but they are not visible until
		   the same is saved and loaded again. So we will avoid that problem
		   for now by not allowing QBBatch to modify our state...
		*/
		GameState bogo;
		QBBatch::process_scripts( &bogo, li );
		impl->fb->reloadDir();// in case script generated anything.
		worked = true;
	    }
	    catch(std::exception const & ex)
	    {
		worked = false;
		std::ostringstream os;
		os << "Script threw an exception! The error text is:\n"
		   << ex.what();
		QMessageBox::warning( this, "Script failed!", os.str().c_str(),
				      QMessageBox::Ok, QMessageBox::Ok );
	    }
	}
	if( worked )
	{
		this->statusBar()->showMessage("Loaded: "+fn); // fi.absoluteFilePath());
	}
	else
	{
		this->statusBar()->showMessage("Load FAILED: "+fn); // fi.absoluteFilePath());
	}
	return worked;
}
bool MainWindowImpl::loadPiece( QFileInfo const & fi )
{
	QString fn( fi.filePath() ); // absoluteFilePath() );
	GamePiece * pc = new GamePiece;
	impl->gstate.pieces().addPiece(pc);
	if( pc->fileNameMatches( fn ) )
	{
		qDebug() << "NYI: loadPiece(*"<<pc->s11nFileExtension()<<")";
		delete pc;
		return false;
	}
	else
	{
		qDebug() << "loadPiece("<<fn<<")";
		pc->setPieceProperty("pixmap", fn );
		return true;
	}
	return false;
}

bool MainWindowImpl::loadGame( QString const & fn )
{
	QApplication::setOverrideCursor(Qt::BusyCursor);
	bool b = impl->gstate.load(fn); 
	QApplication::restoreOverrideCursor();
	if( ! b )
	{
		this->statusBar()->showMessage("Load FAILED: "+fn);
		return false;
	}
	this->statusBar()->showMessage("Loaded board: "+fn);
	return true;
}
bool MainWindowImpl::loadGame()
{
	QString fn = QFileDialog::getOpenFileName(this,
		tr("Load file..."),
		"",
		tr("*"));
		// FIXME ^^^ get the list of supported image formats from somewhere dynamic.
	return fn.isEmpty() ? false : this->loadGame(fn);
}

void MainWindowImpl::launchNewBoardView()
{
    const qreal zm(0.20);
	QBoardView * v = new QBoardView( this->impl->gstate.board(), impl->gstate.scene() );
	v->zoom(zm);
	QDockWidget * win = new QDockWidget( "QBoard View", this );
	win->setAttribute(Qt::WA_DeleteOnClose);
#if 1
	QFrame * frame = new QFrame();
	QGridLayout * lay = new QGridLayout(frame);
	lay->setSpacing(1);
	lay->setContentsMargins(2,2,2,2);
	lay->addWidget( v );
	win->setWidget(frame);
	this->addDockWidget(Qt::RightDockWidgetArea, win );
	//QSizeF sz(v->sizeHint());
	//sz.scale( sz.width()+10, sz.height()+20, Qt::IgnoreAspectRatio);
	//win->resize( sz.toSize() );
	//win->show();
#else
	win->setWidget( v );
	this->addDockWidget(Qt::RightDockWidgetArea, win );
#endif
}
#include <QPrinter>
#include <QPrintDialog>
#include <QDialog>
void MainWindowImpl::printGame()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);
	if (QPrintDialog(&printer).exec() == QDialog::Accepted)
	{
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		// arrggghhh! impl->gv->render( &painter, QRect(), QRect(QPoint(0,0),impl->gv->size()) );
		// arrggghhh! impl->gv->render( &painter, QRect(), impl->gstate.scene()->sceneRect().toRect() );
		impl->gv->render( &painter, QRect(), impl->gstate.board().pixmap().rect() );
		// fixme: print properly when there's no background pixmap.
		// fixme: offer scale-to-page option, but also allow multi-page print
		// fixme: offer print scaling
	}
}
void MainWindowImpl::launchNewWindow()
{
	(new MainWindowImpl)->show();
}

void MainWindowImpl::addLine()
{
	QGILineNode * ln = new QGILineNode;
	QGILineNode * rn = new QGILineNode;
	ln->setProperty("color", "#ffff00");
	rn->setProperty("color", "#ff0000");
	rn->setPos(50,50);
	ln->setPos(50,100);
	QGILineBinder * ed = new QGILineBinder(ln,rn);
#if 0
	ed->setProperty("lineColor", "#00ffff");
#endif
	ed->setProperty("lineWidth", 2 );
	ed->setProperty("lineStyle", Qt::DotLine );
	impl->gstate.scene()->addItem(ed);

}

void MainWindowImpl::doSomethingExperimental()
{
	qDebug() << "MainWindowImpl::doSomethingExperimental()";
	//new QGraphicsLineItem( 0,0, 200, 200 )
	//impl->gstate.scene()->addItem(  );
	//impl->gstate.scene()->addWidget( new QFrame );
	if(1)
	{
		QGILineNode * ln = new QGILineNode;
		QGILineNode * rn = new QGILineNode;
		//QGILineNode * rrn = new QGILineNode;
		//impl->gstate.scene()->addItem(ln);
		//impl->gstate.scene()->addItem(rn);
		//impl->gstate.scene()->addItem(rrn);
		rn->setPos(50,50);
		ln->setPos(50,100);
		QGILineBinder * ed = new QGILineBinder(ln,rn);
		ed->setProperty("lineColor", "#00ffff");
		ed->first()->setProperty("ballColor", "#00ff00");
		ed->second()->setProperty("ballColor", "#ff0000");
		ed->setProperty("lineStyle", Qt::DotLine );
		ed->setProperty("lineWidth", 5 );
		impl->gstate.scene()->addItem(ed);
		//ed = new QGILineBinder(rn,rrn);
		//impl->gstate.scene()->addItem(ln);
		//impl->gstate.scene()->addItem(rn);
	}
}

#include <QMessageBox>
void MainWindowImpl::clearBoard()
{
	QString msg("Really clear the board? This will delete all pieces!");
	if( QMessageBox::Ok == QMessageBox::question( this, "Clear board?", msg, QMessageBox::Ok | QMessageBox::Cancel ) )
	{
		impl->gstate.clear();
		this->statusBar()->showMessage("Board cleared.");
	}
	else
	{
		this->statusBar()->showMessage("Disaster avoided.",2000);
	}
}

void MainWindowImpl::addQGIHtml()
{
	QGIHtml * ti = new QGIHtml;
#if 0
	/* weird bug: when i use this dialog HERE,
	the following isEmpty check always returns false if the user
	enters anything in the HTML area of the text, but not if he
	deletes the HTML and enters only plain text.
	*/
	QGIHtmlEditor ed(ti,this);
	if(ed.exec() != QDialog::Accepted )
	{
		delete ti;
		this->statusBar()->showMessage("Cancelled");
		return;
	}
	if( ti->toPlainText().isEmpty() )
	{
		ti->setPlainText("[Double-click to edit.]");
	}
#else
	//ti->setHtml("<html><body style='background-color:white'>Double-click to edit</body></html>");
#endif
	impl->gstate.scene()->addItem(ti);
	this->statusBar()->showMessage("Added text item. Double-click to edit it.");
}
void MainWindowImpl::rotate90()
{
	impl->gv->rotate( 90 );
}
