#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
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
protected:
	//bool eventFilter(QObject *obj, QEvent *ev);	
private Q_SLOTS:
	void chdir(const QDir & dir);
private:
	struct Impl;
	Impl * impl;
};
#endif
