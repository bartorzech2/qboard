#ifndef SETUPQBOARD_H
#define SETUPQBOARD_H
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

#include <QDialog>
#include "ui_SetupQBoard.h"

class SetupQBoard : public QDialog, public Ui::SetupQBoardDialog
{
Q_OBJECT
public:
	SetupQBoard(QWidget * parent);
	virtual ~SetupQBoard();
public Q_SLOTS:
	virtual void accept();
	virtual void reject();
};
#endif
