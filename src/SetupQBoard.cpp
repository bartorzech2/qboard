#include "SetupQBoard.h"
//
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
#include "utility.h"

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
