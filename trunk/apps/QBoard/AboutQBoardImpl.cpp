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

#include "AboutQBoardImpl.h"
#include <qboard/utility.h>
AboutQBoardImpl::AboutQBoardImpl( QWidget * parent ) 
	: QDialog(parent), Ui::AboutQBoard()
{
	this->setupUi(this);
	this->textBrowser->setOpenExternalLinks(true);
	QString msg = QString("About QBoard v. %1").arg(qboard::versionString());
	this->labelHeader->setText(msg);
}

AboutQBoardImpl::~AboutQBoardImpl()
{
}


void AboutQBoardImpl::accept()
{
	this->QDialog::accept(); 
}
