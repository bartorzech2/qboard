#include "AboutQBoardImpl.h"
#include "utility.h"
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
