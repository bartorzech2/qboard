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

#include "QBoardDocsBrowser.h"
#include "utility.h"
#include <QDebug>

QBoardDocsBrowser::QBoardDocsBrowser(QString const & home)
	: QMainWindow(),
	Ui::QBoardDocsBrowser(),
	m_home(home)
{
	this->setupUi(this);
	this->goHome();
}
QBoardDocsBrowser::QBoardDocsBrowser() 
	: QMainWindow(),
	Ui::QBoardDocsBrowser()
{
	this->setupUi(this);
	m_home = qboard::home().canonicalPath() + QDir::toNativeSeparators("/QBoard/manual/index.html");
	this->goHome();
}

void QBoardDocsBrowser::linkClicked(QUrl const &u)
{
	qDebug() << "linkClicked("<<u<<")";
	this->statusBar()->showMessage(u.toString());
}

void QBoardDocsBrowser::goHome()
{
	QString index = this->homePage();
	QFileInfo fi(index);
	if( ! fi.exists() )
	{
		QString msg("Error: page not found:<br/>");
		msg += index;
		this->webView->setHtml(msg);
	}
	else
	{
		this->webView->setUrl( index );
	}
}
void QBoardDocsBrowser::setHome(QString const &h)
{
	m_home = h;
	this->goHome();
}
QString QBoardDocsBrowser::homePage()
{
	return m_home;
}

QBoardDocsBrowser::~QBoardDocsBrowser()
{
}
