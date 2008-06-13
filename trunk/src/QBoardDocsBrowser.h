#ifndef QBOARDDOCSBROWSER_H
#define QBOARDDOCSBROWSER_H

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
