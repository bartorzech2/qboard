#ifndef SETUPQBOARD_H
#define SETUPQBOARD_H

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
