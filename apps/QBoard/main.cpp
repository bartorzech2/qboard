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

#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QFile>

#include <sstream>

#include <s11n.net/s11n/s11nlite.hpp>

#include "MainWindowImpl.h"
#include "SetupQBoard.h"
#include <qboard/utility.h>

int do_setup()
{
#if 0 && (QT_VERSION < 0x040400) && QBOARD_VERSION
    std::ostringstream os;
    os << "Warning: your QBoard was built against Qt 4.3.x. While it will be basically functional, "
       << "some features are missing and there may be weird behaviours under Qt 4.3 "
       << "which don't show up in Qt 4.4.";
    QMessageBox::warning( 0, "Qt 4.3.x warning", os.str().c_str(),
			  QMessageBox::Ok, QMessageBox::Ok );
			  
#endif
	QDir home(qboard::home());
	QString dirname = home.absolutePath();
#if 1
	if( home.exists() )
	{
		QDir::setCurrent(dirname);
		return 0;
	}
#endif
	SetupQBoard dlg(0);
	return (QDialog::Accepted == dlg.exec())
		? 0
		: 1;
}

int main(int argc, char ** argv)
{
	s11nlite::serializer_class("parens");
	QApplication app( argc, argv );
	try
	{
		int setrc = do_setup(); 
		if( setrc != 0 )
		{
			return setrc;
		}
		MainWindowImpl win;
		win.show(); 
		app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
		return app.exec();
	}
	catch( std::exception const & ex )
	{
		std::cerr << argv[0] << ": EXCEPTION: " << ex.what() << '\n';
	}
	return 1;
}
