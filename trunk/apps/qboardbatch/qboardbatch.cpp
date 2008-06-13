#include <QApplication>
#include <QDebug>
#include <QString>

#include <functional>
#include <algorithm>
#include <iterator>
//#include <cctype>
#include <iostream>

#include <s11n.net/s11n/s11nlite.hpp>
#include "QBBatch.h"
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#ifndef COUT
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif




int main(int argc, char ** argv)
{
#if 0
    std::string inp("012345\n012345\n\n");
    Ps::parse_iterator it(inp);
    for( ; !it.eof(); ++it )
    {
	COUT << "CHAR=["<<*it<<"] L"<<it.line()<<" C"<<it.col()<<"\n";
    }
    return 0;
#endif

    s11nlite::serializer_class("parens");
    if( 1 == argc )
    {
	std::cerr << "Usage: " << argv[0] << " [flags] FILE1 [...FILE_N]\n"
		  << "Where each file is a qboard batch script and the flags are:\n"
		  << "\t-v VERBOSE mode\n"
	    ;
	return 1;
    }
    QApplication app( argc, argv );

    try
    {
	QBBatch::process_scripts(argc-1,argv+1);
    }
    catch( std::exception const & ex )
    {
	std::cerr << argv[0] << ": EXCEPTION: " << ex.what() << '\n';
	return 1;
    }
    catch( ... )
    {
	std::cerr << argv[0] << ": UNKNOWN EXCEPTION\n";
	return 2;
    }
    return 0;
}

#undef COUT
#undef CERR
#undef VERBOSE
#undef QDEBUG
#undef PERR
