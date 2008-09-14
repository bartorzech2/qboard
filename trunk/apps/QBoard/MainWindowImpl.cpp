/*
 * This file is (or was, at some point) part of the QBoard project
 * (http://code.google.com/p/qboard)
 *
 * Copyright (c) 2008 Stephan Beal (http://wanderinghorse.net/home/stephan/)
 *
 * This file may be used under the terms of the GNU General Public
 * License versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 * included in the packaging of this file.
 *
 */

#include "MainWindowImpl.h"
//#include <QtScript>
#include <QCursor>
#include <QDebug>
#include <QDialog>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QRegExp>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QScriptEngine>

#include <cmath>
#include <s11n.net/s11n/s11nlite.hpp>

#include <qboard/QBoardView.h>
#include <qboard/QGIPiece.h>
#include <qboard/QGIHtml.h>
#include <qboard/GameState.h>
#include <qboard/QBoardHomeView.h>
#include <qboard/QGILine.h>
#include <qboard/utility.h>
#include <qboard/S11nQt/S11nClipboard.h>
#include <qboard/QBoard.h>
#include <qboard/PieceAppearanceWidget.h>
#include "AboutQBoardImpl.h"
#include <qboard/WikiLiteView.h>

#if QT_VERSION >= 0x040400
#include <qboard/QBoardDocsBrowser.h>
#endif

#include <qboard/ScriptQt.h>

#define QBOARD_MAINWINDOW_PERSISTENCE_CLASS "MainWindow"

struct MainWindowImpl::Impl
{
    GameState gstate;
    QBoardView * gv;
    QBoardHomeView * tree;
    PieceAppearanceWidget *paw;
    QWidget * sidebar;
    static char const * fileName;
    static char const * persistanceClass;
    Impl() : gstate(),
	     gv(0),
	     tree(0),
	     paw(new PieceAppearanceWidget),
	     sidebar(0)
    {
	QDir pdir = qboard::persistenceDir( persistanceClass );
	QString fn = pdir.canonicalPath() + "/" + fileName;
	try
	{
	    if( ! paw->s11nLoad(fn) )
	    {
		paw->setupDefaultTemplates();
	    }
	}
	catch(...)
	{
	    paw->setupDefaultTemplates();
	}
	gstate.board().loadPixmap(":/QBoard/images/SplashScreen.jpg");
    }
    ~Impl()
    {
	QDir pdir = qboard::persistenceDir( persistanceClass );
	QString fn = pdir.canonicalPath() + "/" + fileName;
	try
	{
	    paw->s11nSave(fn,false);
	}
	catch(...){}
	/** Reminder: we could rely on the fact that the member QWidgets
	    will be assigned to parent widgets, and we don't need delete
	    them from here. Call me old fashioned. */
	delete paw;
	delete gv;
	delete sidebar;
    }
};
char const * MainWindowImpl::Impl::fileName = "default-templates";
char const * MainWindowImpl::Impl::persistanceClass = "MainWindow";

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
    : QMainWindow(parent, f),
      Ui::MainWindow(),
      impl(new Impl)
{

	setupUi(this);
	this->setWindowTitle( QString("QBoard v. %1").arg(qboard::versionString()) );
	connect( this->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveGame()) );
	connect( this->actionLoad, SIGNAL(triggered(bool)), this, SLOT(loadGame()) );
	connect( this->actionNewBoardView, SIGNAL(triggered(bool)), this, SLOT(launchNewBoardView()) );
	connect( this->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()) );
	connect( this->actionAboutQBoard, SIGNAL(triggered(bool)), this, SLOT(aboutQBoard()) );
	connect( this->actionPrint, SIGNAL(triggered(bool)), this, SLOT(printGame()) );
	connect( this->actionHelp, SIGNAL(triggered(bool)), this, SLOT(launchHelp()) );
	connect( this->actionClearBoard, SIGNAL(triggered(bool)), this, SLOT(clearBoard()) );
	connect( this->actionQuickSave, SIGNAL(triggered(bool)), this, SLOT(quickSave()) );
	connect( this->actionQuickLoad, SIGNAL(triggered(bool)), this, SLOT(quickLoad()) );


	impl->gstate.enablePlacemarker(true);

#if ! QBOARD_VERSION
	// For "end user builds" we won't show this action.
	connect( this->actionExperiment, SIGNAL(triggered(bool)), this, SLOT(doSomethingExperimental()) );
	this->actionExperiment->setEnabled(true);
#else
	this->actionExperiment->setEnabled(false);
#endif
	connect( &S11nClipboard::instance(), SIGNAL(signalUpdated()), this, SLOT(clipboardUpdated()) );

	this->actionClearClipboard->setEnabled( 0 != S11nClipboard::instance().contents() );
	connect( this->actionClearClipboard, SIGNAL(triggered(bool)), this, SLOT( clearClipboard() ) );

	QWidget * cli = this->clientArea;
	QLayout * lay = new QGridLayout( cli );
	lay->setSpacing(0);
	lay->setContentsMargins(2,2,2,2);
	QSplitter * splitter = new QSplitter( Qt::Horizontal, cli );
	lay->addWidget( splitter );
	splitter->setHandleWidth(4);

	QSplitter * vsplit = new QSplitter( Qt::Vertical, cli );
	impl->sidebar = vsplit;
	splitter->addWidget( vsplit );
	vsplit->setHandleWidth(4);

	impl->tree = new QBoardHomeView();
	vsplit->addWidget(impl->tree);
	connect( this->actionRefreshFileList, SIGNAL(triggered(bool)), impl->tree, SLOT(refresh()) );
	connect( impl->tree, SIGNAL(itemActivated(QFileInfo const &)), this, SLOT(loadFile(QFileInfo const &)) );
	connect( this->actionToggleBrowserView, SIGNAL(toggled(bool)), this, SLOT(toggleSidebarVisible(bool)) );
	impl->gv = new QBoardView( impl->gstate );
	//impl->gv->enablePlacemarker(true);
	connect( this->actionToggleBoardDragMode, SIGNAL(toggled(bool)),
		impl->gv, SLOT(setHandDragMode(bool)) );
	this->actionToggleBoardDragMode->setChecked(false);

	connect( this->actionCopy, SIGNAL(triggered(bool)), impl->gv, SLOT(clipCopySelected()) );
	connect( this->actionCut, SIGNAL(triggered(bool)), impl->gv, SLOT(clipCutSelected()) );
	connect( this->actionPaste, SIGNAL(triggered(bool)), impl->gv, SLOT(clipPaste()) );

	connect( this->actionSelectAll, SIGNAL(triggered(bool)), impl->gv, SLOT(selectAll()) );
	connect( this->actionZoomIn, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomIn()) );
	connect( this->actionZoomOut, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomOut()) );	
	connect( this->actionZoomReset, SIGNAL(triggered(bool)), impl->gv, SLOT(zoomReset()) );

#define BOGO(A)
	// this->action ## A->setParent(impl->gv);
	BOGO(Copy);
	BOGO(Cut);
	BOGO(Paste);
	BOGO(ZoomIn);
	BOGO(ZoomOut);
	BOGO(ZoomReset);
#undef BOGO

	splitter->addWidget( impl->gv );

	splitter->setStretchFactor(0,2);
	splitter->setStretchFactor(1,3);

	vsplit->addWidget( impl->paw );

	vsplit->setStretchFactor(0,3);
	vsplit->setStretchFactor(1,1);

	if(1)
	{
	    impl->gv->setObjectName("view");
	    QScriptEngine & js( impl->gstate.jsEngine() );
	    QScriptValue jo = js.newQObject( impl->gv, QScriptEngine::QtOwnership );
	    QScriptValue qb = js.globalObject().property("qboard");
	    qb.setProperty("view",jo);
	    qb.setProperty("window",js.newQObject(this, QScriptEngine::QtOwnership ) );
	}

	this->resize(760,600);
}

MainWindowImpl::~MainWindowImpl()
{
	delete impl;
}

// bool MainWindowImpl::eventFilter( QObject * obj, QEvent * ev )
// {
//     return QMainWindow::eventFilter(obj,ev);
// }

void MainWindowImpl::clearClipboard()
{
    delete( S11nClipboard::instance().take() );
}

void MainWindowImpl::clipboardUpdated()
{
    QString msg;
    S11nClipboard & cb( S11nClipboard::instance() );
    if( ! cb.contents() )
    {
	msg += "cleared";
	this->actionClearClipboard->setEnabled( false );
	this->actionPaste->setEnabled( false );
    }
    else
    {
	msg += cb.contentLabel();
	this->actionClearClipboard->setEnabled( true );
	this->actionPaste->setEnabled( true );
#if 0
	qDebug() << "MainWindowImpl::clipboardUpdated(): content:";
	s11nlite::save( *cb.contents(), std::cout );
#endif
    }
    this->statusBar()->showMessage( msg );
}


void MainWindowImpl::goHome()
{
	this->chdir( qboard::home() );
	//impl->fb->setDir(".");
}
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
	QMessageBox::warning( this, tr("Not implemented!"),
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
	b = impl->gstate.s11nSave( fn, true );
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
    if( ! fi.isFile() ) return false;
	// FIXME: this dispatch needs to be generalized.
	QString fn( fi.filePath() ); // fi.absoluteFilePath() );
	bool worked = false;
	if( impl->gstate.board().fileNameMatches(fn) )
	{
		worked = impl->gstate.board().s11nLoad(fn);
	}
	else if( impl->gstate.fileNameMatches(fn) )
	{
		worked = this->loadGame(fn);
	}
	else if( fn.endsWith(".wiki") )
	{
	    worked = true;
	    QString lbl = QString("Wiki: %1").arg(fi.fileName());
	    QDockWidget * win = new QDockWidget( lbl, this );
	    qboard::WikiLiteView * v = new qboard::WikiLiteView;
	    win->setAttribute(Qt::WA_DeleteOnClose);
	    win->setWidget( v );
	    win->move( impl->gv->viewport()->mapToGlobal(QPoint(0,0)) );
	    win->resize( impl->gv->viewport()->size() );
	    win->setFloating(true);
	    this->addDockWidget(Qt::RightDockWidgetArea, win );
	    v->parseFile(fn);
	    win->setWindowTitle( v->windowTitle() );
	}
	else if( QRegExp("\\.[jq]s$",Qt::CaseInsensitive).indexIn(fn) > 0 )
	{
	    QScriptEngine & js( impl->gstate.jsEngine() );
	    qDebug() << "[ running script"<<fn<<"]";
	    QScriptValue rv = qboard::jsInclude( &js, fn );
	    if( rv.isError() )
	    {
		QStringList bt( js.uncaughtExceptionBacktrace() );
		QScriptValue exv = rv.isError() ? rv : js.uncaughtException();
		QString msg("Script threw or returned an exception:\n");
		msg += exv.toString()
		    + "\nBacktrace:\n"
		    + bt.join("\n");
		QMessageBox::warning( this, "JavaScript Exception",
				      msg,
				      QMessageBox::Ok, QMessageBox::Ok );
		worked = false;
	    }
	    else
	    {
		worked = true;
	    }
	    qDebug() << "[ done running script"<<fn<<"]";
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
			worked = this->impl->gstate.board().s11nLoad(fn); // fi.absoluteFilePath());
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
	QGIPiece * pc = new QGIPiece;
	if( pc->fileNameMatches( fn ) )
	{
	    try
	    {
		if( ! pc->s11nLoad( fn ) )
		{
		    delete pc;
		    QMessageBox::warning( this, "Load failed!",
					  QString("Failed: MainWindowImpl::loadPiece(%1)").arg(fn),
					  QMessageBox::Ok, QMessageBox::Ok );
		    return false;
		}
	    }
	    catch(...)
	    {
		delete pc;
		throw;
	    }
	}
	else
	{
	    impl->paw->applyCurrentTemplate( pc );
	    pc->setProperty( "pixmap", qboard::homeRelative(fn) );
	}
	impl->gstate.addItem(pc, true);
	return true;
}

bool MainWindowImpl::loadGame( QString const & fn )
{
	QApplication::setOverrideCursor(Qt::BusyCursor);
	bool b = impl->gstate.s11nLoad(fn); 
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
    const qreal zm(0.25);
    QBoardView * v = new QBoardView( this->impl->gstate );
    v->zoom(zm);
	QDockWidget * win = new QDockWidget( "QBoard View", this );
	win->setAttribute(Qt::WA_DeleteOnClose);
#if 0
	QWidget * frame = new QWidget();
	QGridLayout * lay = new QGridLayout(frame);
	lay->setSpacing(1);
	lay->setContentsMargins(2,2,2,2);
	lay->addWidget( v );
	win->setWidget(frame);
	this->addDockWidget(Qt::RightDockWidgetArea, win );
#else
	win->setWidget( v );
	this->addDockWidget(Qt::RightDockWidgetArea, win );
#endif
}
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
	impl->gstate.addItem(ed,true);

}

#include <qboard/QBoardPlugin.h>
#include <qboard/QGIDot.h>
#include <QPluginLoader>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QLineF>
#include <QScriptEngine>
void MainWindowImpl::doSomethingExperimental()
{
	qDebug() << "MainWindowImpl::doSomethingExperimental()";
	//new QGraphicsLineItem( 0,0, 200, 200 )
	//impl->gstate.scene()->addItem(  );
	//impl->gstate.scene()->addWidget( new QFrame );

	if(0)
	{
	    QString fileName("eval.js");
	    QFile scriptFile(fileName);
	    if (!scriptFile.open(QIODevice::ReadOnly)) return;
	    qDebug() << "[ running script"<<fileName<<"]";
	    QScriptEngine & eng( impl->gstate.jsEngine() );
	    QTextStream stream(&scriptFile);
	    QString contents = stream.readAll();
	    scriptFile.close();
	    eng.evaluate(contents, fileName);
	    qDebug() << "[ done running script"<<fileName<<"]";
	}

	if(0)
	{
	    QBoardHomeView * v = new QBoardHomeView(0);
	    v->show();
	    connect( v, SIGNAL(itemActivated(QFileInfo const &)),
		     this, SLOT(loadFile(QFileInfo const &)) );

#if 0
	    QDirModel *model = new QDirModel;
	    model->setIconProvider( impl->fb->iconProvider() );
	    QTreeView *tree = new QTreeView(0);
	    tree->setModel(model);
	    for( int i = 1; i < 4; ++i )
	    {
		tree->setColumnHidden(i, true);
	    }
	    tree->setRootIndex(model->index(QDir::currentPath()));
	    tree->show();
	    QString fn("QBoard/manual/index.html");
	    QModelIndex sel;
#define FP sel = model->index(fn); \
	    qDebug() << fn << "sel.isValid() =="<<sel.isValid() \
		     << "filePath =="<<model->filePath(sel);
	    FP;
	    fn = QString("%1/QBoard/manual/index.html").arg(qboard::home().absolutePath());
	    FP;
	    fn = "/foo";
	    FP;
#undef FP
#endif
	}

	if(0)
	{
	    QGraphicsLineItem * li =
		new QGraphicsLineItem;
	    QLineF co(20,20,100,100);
	    li->setLine( co );
	    li->setFlag(QGraphicsItem::ItemIsMovable, true);
	    li->setFlag(QGraphicsItem::ItemIsSelectable, false);
	    impl->gstate.scene()->addItem( li );

	    QGIDot * dot = new QGIDot;
	    dot->setFlag(QGraphicsItem::ItemIsMovable, false);
	    //dot->setFlag(QGraphicsItem::ItemIsSelectable, false);
	    dot->setParentItem(li);
	    dot->setPos( dot->mapFromParent( co.p2() ) );
	    dot->setProperty("color","white");
	}

	if(0)
	{
	    QGraphicsItem * dot = new QGIDot;
	    dot->setPos( QPointF(40, 40) );
	    impl->gstate.scene()->addItem( dot );
	}

	if(0)
	{
	    QGIPiece * pc = new QGIPiece;
	    pc->setProperty("color",QColor("#ffff00") );
	    pc->setProperty("borderColor",QColor("#ff0000") );
	    pc->setProperty("borderSize", 1);
	    pc->setProperty("pos",QPoint(20,20) );
	    pc->setProperty("size",QSize(50,50) );
	    impl->gstate.scene()->addItem(pc);

	    QGIHtml * i = 
		new QGIHtml;
	    i->setPlainText("Hi, world");
	    i->setFlag(QGraphicsItem::ItemIsMovable, true);
	    i->setFlag(QGraphicsItem::ItemIsSelectable, false);
	    i->setPos( QPointF(20, 20) );
	    i->setParentItem(pc);
#if 0
	    QGraphicsSimpleTextItem * ch = 
		new QGraphicsSimpleTextItem;
	    ch->setFlag(QGraphicsItem::ItemIsMovable, true);
	    ch->setFlag(QGraphicsItem::ItemIsSelectable, false);
	    ch->setParentItem( i );
	    ch->setPos( 4, 8 );
	    ch->setText("I'm a child item");
#endif
	}

	if(0)
	{
	    QGraphicsTextItem * ti = new QGraphicsTextItem("Hi, world.");
	    impl->gstate.scene()->addItem( ti );
	    ti->setFlag(QGraphicsItem::ItemIsMovable, true);
	    ti->setFlag(QGraphicsItem::ItemIsSelectable, true);
	}
	if(0)
	{
	    qboard::transformFlip( impl->gv, true );
#if 0
	    // Flip board view...
	    QGraphicsView * v = impl->gv;
	    QTransform t( v->transform() );
	    QRectF r(v->sceneRect());
	    //t.translate( -r.width(), 0 ).scale(-1,1);
	    t.translate( 0, -r.height() ).scale(1,-1);
	    v->setTransform( t );
#endif
	}

#if 1
	if(1)
	{
	    QBoardPlugin * plug = 0;
	    QDir pluginsDir(qApp->applicationDirPath());
	    QString fname = pluginsDir.absoluteFilePath("plugins/libJSEditor.so");
	    qDebug() << "fname =="<<fname;
#if QT_VERSION >= 0x040400
	    qDebug() << "pluginsDir =="<<pluginsDir;
#endif
	    QPluginLoader loader(fname);
	    loader.load();
	    qDebug() << "loader.errorString() =="<<loader.errorString();
	    qDebug() << "isLoaded =="<<loader.isLoaded();
	    QObject *inst = loader.instance();
	    if (inst) {
		plug = qobject_cast<QBoardPlugin *>(inst);
	    }
	    qDebug() << "inst =="<<inst;
	    qDebug() << "plugin =="<<plug;
 	    if( plug )
 	    {
		plug->setGameState( impl->gstate );
		QWidget * v = plug->widget();
		qDebug() << "plugin widget =="<<v;
		if( v )
		{
		    QDockWidget * win = new QDockWidget( "Plugin", this );
		    win->setAttribute(Qt::WA_DeleteOnClose);
		    win->setWidget( v );
		    this->addDockWidget(Qt::RightDockWidgetArea, win );
		}
 	    }
	    //loader.unload();
         }
#endif
	if(0)
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
	impl->gstate.addItem(ti,true);
	this->statusBar()->showMessage("Added text item. Double-click to edit it.");
}
void MainWindowImpl::rotate90()
{
    QVariant var( impl->gv->property("angle") );
    qreal ang = (var.toInt() + 90) % 360;
    impl->gv->setProperty("angle", ang );
}

void MainWindowImpl::toggleSidebarVisible(bool b)
{
    impl->sidebar->setVisible( b );
}

static QString quickSaveFileName( GameState & gs )
{
    static QString bob;
    if( bob.isEmpty() )
    {
	QDir d( qboard::persistenceDir(QBOARD_MAINWINDOW_PERSISTENCE_CLASS) );
	bob = QString("%1%2").
	    arg(d.absoluteFilePath( "quicksave" )).
	    arg(gs.s11nFileExtension());
    }
    return bob;
}

void MainWindowImpl::quickLoad()
{
    this->loadGame( quickSaveFileName(impl->gstate) );
}

void MainWindowImpl::quickSave()
{
    this->saveGame( quickSaveFileName(impl->gstate) );
}


