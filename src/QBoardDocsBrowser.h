#ifndef QBOARDDOCSBROWSER_H
#define QBOARDDOCSBROWSER_H
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
#include <QString>
#include <QUrl>
#include "ui_QBoardDocsBrowser.h"

class QBoardDocsBrowser : public QMainWindow, public Ui::QBoardDocsBrowser
{
Q_OBJECT
public:
	QBoardDocsBrowser();
	explicit QBoardDocsBrowser(QString const & home);
	virtual ~QBoardDocsBrowser();
	QString homePage();
public Q_SLOTS:
	void linkClicked(QUrl const &);
	void goHome();
	void setHome(QString const &);
private:
	QString m_home;
};
#endif
