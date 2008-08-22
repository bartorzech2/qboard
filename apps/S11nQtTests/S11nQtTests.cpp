#include <QApplication>
#include <QDebug>
#include <QString>
#include <QFile>

#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <s11n.net/s11n/s11nlite.hpp>
#include <s11n.net/s11n/s11n_debuggering_macros.hpp>

#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#ifndef COUT
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include "GamePiece.h"
#include "Serializable.h"
#include "S11nQt.h"
#include "S11nQtList.h"
#include "S11nQtStream.h"
#include <s11n.net/s11n/proxy/pod/int.hpp>

void try_s11n()
{
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
	    QString fname("foo.s11n");
	    {
		QFile ofile(fname);
		ofile.remove();
		QtStdOStream str(ofile);
		s11nlite::save( p2, str );
		qDebug() << "Take a look for file "<<fname;
	    }
	    {
		QFile ifile(fname);
		//ifile.remove();
		// ifile.open(QIODevice::WriteOnly); // test exception tossing
		//StdToQtIBuf sentry( std::cin, ifile );
		QtStdIStream str(ifile);
		S11nNode * cp = s11nlite::load_node( str );
		if( ! cp )
		{
		    throw std::runtime_error("ERROR loading S11nNode from QIODevice!");
		}
		else
		{
		    COUT << "Read from QIODevice:\n";
		    s11nlite::save( *cp, std::cout );
		    delete cp;
		}
	    }
	}
    }

    if(1)
    {
	using namespace s11n::qt;
	QVariant qv;
	qv.setValue( VariantS11n(QString("hi, world")) );
	COUT << "VariantS11n::variantType() == " << VariantS11n::variantType()
	     << " qv.type() == "<<qv.type()<<'\n';
	QVariant vcp( qv );
	COUT << "VariantS11n::variantType() == " << VariantS11n::variantType()
	     << " vcp.type() == "<<vcp.type()<<'\n';
	COUT << "vcp.typeName() == "
	     << vcp.typeName() <<'\n';
	COUT << "vcp.userType() == "
	     << vcp.userType() <<'\n';
	COUT << "typeToName("<<VariantS11n::variantType()<<") == "
	     << QVariant::typeToName(QVariant::Type(VariantS11n::variantType()))<<"\n";
	COUT << "QVariant::typeToName(vcp.type()=="<<vcp.type()<<") == "
	     << QVariant::typeToName(vcp.type())<<"\n";
	COUT << "QVariant::nameToType("<<vcp.typeName()<<") == "
	     << QVariant::nameToType(vcp.typeName()) << '\n';
	COUT << "save(qv):\n";
	s11nlite::save( qv, std::cout );
	VariantS11n vsn( vcp.value<VariantS11n>() );
	COUT << "save(vsn.node()):\n";
	s11nlite::save( vsn.node(), std::cout );
	QString deser;
	if( ! vsn.deserialize(deser) )
	{
	    throw s11n::s11n_exception("VariantS11n deserialization of QString failed!");
	}
	QSize bogus;
	if( vsn.deserialize(bogus) )
	{
	    throw s11n::s11n_exception("VariantS11n deserialization of non-QString should not have worked!");
	}
	COUT << "save(deser):\n";
	s11nlite::save( deser, std::cout );
    }

}

int main(int argc, char ** argv)
{
    s11nlite::serializer_class("parens");
    QApplication app( argc, argv );
    qRegisterMetaType<s11n::qt::VariantS11n>("VariantS11n");
    try
    {
	try_s11n();
	//app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	//return app.exec();
	return 0;
    }
    catch( std::exception const & ex )
    {
	std::cerr << argv[0] << ": EXCEPTION: " << ex.what() << '\n';
    }
    return 1;

}
