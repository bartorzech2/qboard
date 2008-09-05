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

#include "SetupQBoard.h"

SetupQBoard::SetupQBoard(QWidget * parent) 
	: QDialog(parent),
	Ui::SetupQBoardDialog()
{
	this->setupUi(this);
}
//
SetupQBoard::~SetupQBoard()
{
}

#include <QString>
#include <QDir>
#include <QDebug>
#include <qboard/utility.h>

void SetupQBoard::accept()
{
	QString dirname = qboard::home().absolutePath();
	QDir home(dirname);
	if( ! home.exists() )
	{
		if( ! home.mkpath(dirname) )
		{
			qDebug() << "Could not create QBoard home dir:" <<dirname;
			this->QDialog::reject(); 
			return;
		}
	}
	QDir::setCurrent(dirname);
	this->QDialog::accept(); 
}
void SetupQBoard::reject()
{
	this->QDialog::reject();
}
