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

#include "S11nQt.h"

#include "S11nQt/QBrush.h"
#include "S11nQt/QBitArray.h"
#include "S11nQt/QByteArray.h"
#include "S11nQt/QColor.h"
#include "S11nQt/QDate.h"
#include "S11nQt/QDateTime.h"
#include "S11nQt/QFont.h"
#include "S11nQt/QLineF.h"
#include "S11nQt/QLine.h"
#include "S11nQt/QList.h"
#include "S11nQt/QMap.h"
#include "S11nQt/QMatrix.h"
#include "S11nQt/QPair.h"
#include "S11nQt/QPen.h"
#include "S11nQt/QPixmap.h"
#include "S11nQt/QPointF.h"
#include "S11nQt/QPoint.h"
#include "S11nQt/QPolygon.h"
#include "S11nQt/QRectF.h"
#include "S11nQt/QRect.h"
#include "S11nQt/QRegExp.h"
#include "S11nQt/QSizeF.h"
#include "S11nQt/QSize.h"
#include "S11nQt/QString.h"
#include "S11nQt/QStringList.h"
#include "S11nQt/QTime.h"
#include "S11nQt/QTransform.h"
#include "S11nQt/QVariant.h"
#include "S11nQt/Stream.h"

#include <s11n.net/s11n/proxy/pod/int.hpp>

void try_s11n()
{
    // test out our s11n-qt routines...
    QString qs("Hello, world.");
    S11nNode node;
    using namespace s11n::qt;
    typedef s11nlite::node_traits NT;
#define THROW(MSG) throw std::runtime_error(MSG);
    if(0)
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
    if(0)
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
    if(0)
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
    if(0)
    {
	//QVector<QPoint> poly;
	QPolygon poly;
	for( int i =0 ; i < 5; ++i ) poly.push_back( QPoint(i,1) );
	s11nlite::save( poly, std::cout );
    }

    NT::clear(node);
    if(0)
    {
	QString ucstr("Hällo, world.");
	qDebug() << "ucstr="<<ucstr<<'='<<"H\u00e4llo, world.";
	s11nlite::save( ucstr, std::cout );
    }

    NT::clear(node);
    if(0)
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
    if(0)
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
	QString const orig("hi, world");
	qv.setValue( VariantS11n(orig) );
	COUT << "VariantS11n::variantType() == " << VariantS11n::variantType()
	     << "\nqv.type() == "<<qv.type()
	     << "\nqv.userType() == "<<qv.userType()<<'\n';
	COUT << "save(qv):\n";
	s11nlite::save( qv, std::cout );
	QVariant vcp( qv );
	COUT << "save(vcp):\n";
	s11nlite::save( vcp, std::cout );
	COUT << "VariantS11n::variantType() == " << VariantS11n::variantType()
	     << " vcp.type() == "<<vcp.type()<<'\n';
	COUT << "vcp.typeName() == "
	     << vcp.typeName() <<'\n';
	COUT << "vcp.userType() == "
	     << vcp.userType() <<'\n';
	COUT << "QVariant::typeToName("<<VariantS11n::variantType()<<") == "
	     << QVariant::typeToName(QVariant::Type(VariantS11n::variantType()))<<"\n";
	COUT << "QVariant::typeToName(vcp.type()=="<<vcp.type()<<") == "
	     << QVariant::typeToName(vcp.type())<<"\n";
	COUT << "QVariant::nameToType("<<vcp.typeName()<<") == "
	     << QVariant::nameToType(vcp.typeName()) << '\n';
	VariantS11n vsn( vcp.value<VariantS11n>() );
	COUT << "save(vsn.node()):\n";
	s11nlite::save( vsn.node(), std::cout );
	QString deser;
	if( ! vsn.deserialize(deser) )
	{
	    throw s11n::s11n_exception("VariantS11n deserialization of QString failed!");
	}
	if( deser != QString(orig) )
	{
	    throw s11n_exception("Deserialized QVariant/QString is not the same as the original!");
	}
	QSize bogus;
	if( vsn.deserialize(bogus) )
	{
	    throw s11n::s11n_exception("VariantS11n deserialization of non-QString should not have worked!");
	}
	COUT << "save(deser):\n";
	s11nlite::save( deser, std::cout );
    }

    if(0)
    {
	QTransform tr(11,12,13,21,22,23,31,32,33);
	COUT << "QTransform:\n";
	S11nNode n;
	s11nlite::serialize( n, tr );
	s11nlite::save( n, std::cout );
	QTransform tr2;
	s11nlite::deserialize( n, tr2 );
	s11nlite::save( tr2, std::cout );
    }

    if(0)
    {
	QMatrix tr(11,12,21,22,3,4);
	COUT << "QMatrix:\n";
	S11nNode n;
	s11nlite::serialize( n, tr );
	s11nlite::save( n, std::cout );
	QMatrix tr2;
	s11nlite::deserialize( n, tr2 );
	s11nlite::save( tr2, std::cout );
    }

    if(0)
    {
	QDateTime tr = QDateTime::currentDateTime();
	COUT << "QDateTime:\n";
	S11nNode n;
	s11nlite::serialize( n, tr );
	s11nlite::save( n, std::cout );
	QDateTime tr2;
	s11nlite::deserialize( n, tr2 );
	s11nlite::save( tr2, std::cout );
    }

    if(1)
    {

	int count = 64;
	QBitArray ba(count);
	for( int i = 33; i < count; i += 3 )
	{
	    ba.setBit( i, true );
	}
	ba.setBit(31,true);
	ba.setBit(0,true);
	COUT << "original QBitArray:\n";

#define OUTBIT(BA) for( int i = 0; i < BA.count(); ++i ) {\
	    std::cout << (ba.testBit(i) ? 1 : 0); }	  \
	std::cout << '\n';

	OUTBIT(ba);
	S11nNode n;
	if( ! s11nlite::serialize(n, ba) )
	{
	    throw s11n::s11n_exception("QBitArray serialization failed!");
	}
	s11nlite::save(n, std::cout);
	QBitArray dba;
	if( ! s11nlite::deserialize(n, dba) )
	{
	    throw s11n::s11n_exception("QBitArray deserialization failed!");
	}
	s11nlite::save( dba, std::cout);
	COUT << "original QBitArray:\n";
	OUTBIT(ba);
	COUT << "deser'd QBitArray:\n";
	OUTBIT(dba);
#undef OUTBIT
	std::cout <<'\n';

    }

    if(1)
    {
	QPen p1(QColor(255,0,0,120));
	{
	    QVector<qreal> dp;
	    dp << 4.0 << 2 << 2 << 4;
	    p1.setDashPattern(dp);
	}
	p1.setWidth(4);
	S11nNode n;
	s11n::serialize( n, p1 );
	QPen p2;
	s11n::deserialize( n, p2 );
	s11nlite::save(p1,std::cout);
	s11nlite::save(p2,std::cout);
    }

}

int main(int argc, char ** argv)
{
    s11nlite::serializer_class("parens");
    QApplication app( argc, argv );
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
