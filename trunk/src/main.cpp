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

void try_s11n()
{
#if TEST_QS11N
	// test out our s11n-qt routines...
	QString qs("Hello, world.");
	S11nNode node;
	s11nlite::serialize( node, qs );
	s11nlite::save( node, std::cout );
	qs = QString();
	s11nlite::deserialize( node, qs );
	char const * cp = qs.toAscii();
	COUT << "deserialized QString=["<<cp<<"]\n";
	s11nlite::save( qs, std::cout );

	QRect qr(1,3,4,5);
	typedef s11nlite::node_traits NT;
	NT::clear(node);
	s11nlite::serialize( node, qr );
	s11nlite::save( node, std::cout );
	qr = QRect();
	s11nlite::deserialize( node, qr );
	s11nlite::save( qr, std::cout );
	
	GamePiece gp;
	gp.setProperty("geom", QRect(0,0,40,45) );
	gp.setProperty("bgcolor", QColor(127,0,20,13) );
	gp.setProperty("phrase", QString("Hi, world") );
	gp.setProperty("size", QSize(100,200) );
	gp.setProperty("point", QPointF(10,20) );
	NT::clear(node);
	s11n::serialize( node, gp );
	s11nlite::save( node, std::cout );
	GamePiece gpde;
	s11n::deserialize( node, gpde );
	s11nlite::save( gpde, std::cout );
	
	QStringList li;
	li << "Item 1" << "Item 2" << "Item 3";
	NT::clear(node);
	s11n::serialize( node, li );
	s11nlite::save( node, std::cout );
	li.clear();
	s11n::deserialize( node, li );
	s11nlite::save( node, std::cout );
	
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
	NT::clear(node);
#if 1
	//QVector<QPoint> poly;
	QPolygon poly;
	for( int i =0 ; i < 5; ++i ) poly.push_back( QPoint(i,1) );
	s11nlite::save( poly, std::cout );
#endif
	GamePiece gp2;
	gp2.setProperty("pos",QPoint(1,2));
	gp2.setProperty("size",QSize(10,10));
	qDebug() << "gp.geom() == "<<gp2.geom();
	s11nlite::save( gp2, std::cout );
#if 0
	QString ucstr("Hällo, world.");
	qDebug() << "ucstr="<<ucstr<<'='<<"H\u00e4llo, world.";
	s11nlite::save( ucstr, std::cout );
#endif

#endif // TEST_QS11N
}

#include <QDir>
#include "utility.h"
int do_setup()
{
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
	s11nlite::serializer_class("funxml");
	QApplication app( argc, argv );
	try
	{
#if TEST_QS11N
		try_s11n();
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