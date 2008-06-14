#ifndef ABOUTQBOARDIMPL_H
#define ABOUTQBOARDIMPL_H
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

#include "ui_AboutQBoard.h"
#include <QDialog>
class AboutQBoardImpl : public QDialog, public Ui::AboutQBoard
{
Q_OBJECT
public:
	AboutQBoardImpl(QWidget * parent);
	virtual ~AboutQBoardImpl();
public Q_SLOTS:
	virtual void accept();
};
#endif
