#include <iostream>
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "

#include <qboard/WikiLiteParser.h>
#include <QFile>
#include <QByteArray>

int main( int argc, char ** argv )
{
    CERR << "main()\n";

    QString fn("/dev/stdin");
    if( argc > 1 )
    {
	fn = argv[1];
    }
    QFile inf(fn);
    inf.open(QIODevice::ReadOnly);
    QByteArray src( inf.readAll() );
    if( ! src.size() )
    {
	CERR << "Source empty!\n";
	return 1;
    }
    if( argc > 2 )
    {
	fn = argv[2];
    }
    else
    {
	fn = "/dev/stdout";
    }
    QFile out(fn);
    out.open(QIODevice::WriteOnly);
    CERR << "START PARSING:::\n";
    qboard::WikiLiteParser gp;
    bool worked = gp.parse( QString(src), &out );
    out.close();
    CERR << ":::END PARSING\n";
    CERR << "Worked? == "<<worked<<'\n';
    CERR << "bye!\n";
    return 0;
}

