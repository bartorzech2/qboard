#ifndef ABOUTQBOARDIMPL_H
#define ABOUTQBOARDIMPL_H
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
