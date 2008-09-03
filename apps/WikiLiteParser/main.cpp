#include <iostream>
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "

#include "WikiLiteParser.h"

#include <QFile>
#include <QByteArray>

int main( int, char ** )
{
    COUT << "main()\n";
    QFile inf("/dev/stdin");
    inf.open(QIODevice::ReadOnly);
    QByteArray src( inf.readAll() );
    if( ! src.size() )
    {
	CERR << "Source empty!";
	return 1;
    }
    QFile out("/dev/stdout");
    out.open(QIODevice::WriteOnly);
    CERR << "START PARSING:::\n";
    qboard::WikiLiteParser gp;
    bool worked = gp.parse( QString(src), &out );
    out.close();
    CERR << ":::END PARSING\n";
    CERR << "Worked? == "<<worked<<'\n';
    COUT << "bye!\n";
    return 0;
}

