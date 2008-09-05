#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
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

#include <QMainWindow>
#include <QFileInfo>
#include "ui_MainWindow.h"

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~MainWindowImpl();
public Q_SLOTS:
	bool loadGame( QString const & );
	bool loadFile( QFileInfo const & );
	bool loadPiece( QFileInfo const & );
	bool loadGame();
	bool saveGame( QString const & );
	bool saveGame();
	void launchNewBoardView();
	void doSomethingExperimental();
	void rotate90();
	void goHome();
	void aboutQBoard();
	void printGame();
	void launchHelp();
	void clearBoard();
	void launchNewWindow();
	void addQGIHtml();
	void addLine();
	void toggleSidebarVisible(bool);
    void quickSave();
    void quickLoad();
    //virtual bool eventFilter( QObject * watched, QEvent * event );
protected:
	//bool eventFilter(QObject *obj, QEvent *ev);	
private Q_SLOTS:
	void chdir(const QDir & dir);
    void clipboardUpdated();
    void clearClipboard();
private:
	struct Impl;
	Impl * impl;
};
#endif
