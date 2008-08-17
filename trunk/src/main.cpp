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

#include <QApplication>
#include <QDebug>

#include "MainWindowImpl.h"
#include "SetupQBoard.h"
#include <s11n.net/s11n/s11nlite.hpp>

#define TEST_QS11N 0
#if TEST_QS11N
#include "GamePiece.h"
#include <iostream>
#include "Serializable.h"
#include "S11nQt.h"
#include "S11nQtList.h"
#include <s11n.net/s11n/proxy/pod/int.hpp>
#endif

#include <s11n.net/s11n/s11n_debuggering_macros.hpp>

void try_s11n()
{
#if TEST_QS11N
	// test out our s11n-qt routines...
	QString qs("Hello, world.");
	S11nNode node;
	typedef s11nlite::node_traits NT;

	if(1)
	{
	    s11nlite::serialize( node, qs );
	    s11nlite::save( node, std::cout );
	    qs = QString();
	    s11nlite::deserialize( node, qs );
	    char const * cp = qs.toAscii();
	    COUT << "deserialized QString=["<<cp<<"]\n";
	    s11nlite::save( qs, std::cout );
	}

	NT::clear(node);
	if(0)
	{
	    QRect qr(1,3,4,5);
	    NT::clear(node);
	    s11nlite::serialize( node, qr );
	    s11nlite::save( node, std::cout );
	    qr = QRect();
	    s11nlite::deserialize( node, qr );
	    s11nlite::save( qr, std::cout );
	}

	NT::clear(node);	
	GamePiece gp;
	if(1)
	{
	    gp.setProperty("geom", QRect(0,0,40,45) );
	    gp.setProperty("bgcolor", QColor(127,0,20,13) );
	    gp.setProperty("phrase", QString("Hi, world") );
	    gp.setProperty("size", QSize(100,200) );
	    gp.setProperty("point", QPointF(10,20) );
	    QPixmap pm(35,40);
	    pm.fill(QColor(255,255,0));
	    gp.setProperty( "inlinePixmap", QVariant(pm) );
	    NT::clear(node);
	    bool worked = s11n::serialize( node, gp );
	    COUT << "serialize gp worked ? == "<<worked<<'\n';
	    s11nlite::save( node, std::cout );
	    GamePiece gpde;
	    worked = s11n::deserialize( node, gpde );
	    COUT << "deserialize gp worked ? == "<<worked<<'\n';
	    s11nlite::save( gpde, std::cout );
	}
	
	NT::clear(node);
	if(1)
	{
	    QStringList li;
	    li << "Item 1" << "Item 2" << "Item 3";
	    NT::clear(node);
	    s11n::serialize( node, li );
	    s11nlite::save( node, std::cout );
	    li.clear();
	    s11n::deserialize( node, li );
	    s11nlite::save( node, std::cout );
	}
	
	NT::clear(node);
	if(1)
	{
	    typedef QVector<QRect> QV;
	    QV vec;
	    for( int i = 0; i < 5; ++i ) vec.push_back(QRect(i,i+1,i+2,i+3));
	    NT::clear(node);
	    CERR << "serialized QV:\n";
	    s11n::serialize( node, vec );
	    s11nlite::save( node, std::cout );
	    vec.clear();
	    s11n::deserialize( node, vec );
	    CERR << "re-deserialized QV:\n";
	    s11nlite::save( vec, std::cout );
	    vec.clear();
	}

	NT::clear(node);
	if(1)
	{
	    //QVector<QPoint> poly;
	    QPolygon poly;
	    for( int i =0 ; i < 5; ++i ) poly.push_back( QPoint(i,1) );
	    s11nlite::save( poly, std::cout );
	}

	NT::clear(node);
	if(1)
	{
	    GamePiece gp2;
	    gp2.setProperty("pos",QPoint(1,2));
	    gp2.setProperty("size",QSize(10,10));
	    qDebug() << "gp.geom() == "<<gp2.geom();
	    s11nlite::save( gp2, std::cout );
	}

	NT::clear(node);
	if(0)
	{
	    QString ucstr("Hällo, world.");
	    qDebug() << "ucstr="<<ucstr<<'='<<"H\u00e4llo, world.";
	    s11nlite::save( ucstr, std::cout );
	}

	NT::clear(node);
	if(1)
	{
	    QByteArray qb;
	    if( 0 )
	    {
		qb = QByteArray("Qt is great!");
	    }
	    else
	    {
		const int count = 150;
		qb.reserve(150);
		int at = 0;
		for( int i = 0; i < count; ++i )
		{
		    qb.append( (at == 0) ? '+' : char('0'+at) );
		    if( ++at > 9 ) at = 0;
		}
	    }
	    s11nlite::serialize( node, qb );
	    COUT << "QByteArray:\n";
	    s11nlite::save( node, std::cout );
	    QByteArray qbd;
	    s11n::deserialize( node, qbd );
	    COUT << "QByteArray:\n";
	    s11nlite::save( qbd, std::cout );
	    COUT << "QByteArray: ["<<qbd.constData()<<"]\n";
	}

	NT::clear(node);
	if(1)
	{
	    QPixmap pm(500,500);
	    pm.fill(QColor(255,0,0));
	    qDebug() << "QPixmap:"<<QVariant(pm);
	    if( s11nlite::serialize( node, pm ) )
	    {
		s11nlite::save( node, std::cout );
		QPixmap p2;
		s11nlite::deserialize( node, p2 );
		NT::clear(node);
		s11nlite::save( p2, std::cout );
		//p2.save("000.png","PNG");
	    }
	}
#endif // TEST_QS11N
}

#include <QDir>
#include "utility.h"
#include <QMessageBox>
#include <sstream>
int do_setup()
{
#if (QT_VERSION < 0x040400)
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
#if TEST_QS11N
		try_s11n();
		return 0;
#endif
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
