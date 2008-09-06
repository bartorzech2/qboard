#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h> //fopen()/fread()
#include <fstream>


#include <s11n.net/zfstream/zfstream.hpp>
#include <s11n.net/zfstream/debuggering_macros.hpp> // COUT macro

#include <s11n.net/zfstream/zfstream_config.hpp> // expected: HAVE_ZLIB, HAVE_BZLIB

#if HAVE_ZLIB
#  include <s11n.net/zfstream/gzstream.hpp>
#endif
#if HAVE_BZLIB
#  include <s11n.net/zfstream/bzstream.hpp>
#endif

namespace zfstream {


        // don't use this... i dunno what i want it for yet...
        std::string name_for_policy( CompressionPolicy c )
        {
                switch( c )
                {
                  case( NoCompression ): return "NoCompression";  break;
                  case( ZLibCompression ): return "ZLibCompression"; break;
                  case( BZipCompression ): return "BZipCompression"; break;
                  default: return ""; break;
                }
        }

        bool supports_compression_policy( CompressionPolicy c )
        {
                CompressionPolicy p = compression_policy();
                if( NoCompression == p ) return true;
#if HAVE_BZLIB
                if( BZipCompression == p ) return true;
#endif                
#if HAVE_ZLIB
                if( ZLibCompression == p ) return true;
#endif            
                return false;
        }



        static CompressionPolicy m_comp_policy = zfstream::NoCompression;
        void compression_policy( CompressionPolicy c )
        {
                //CERR << "compression_policy = " << c << std::endl;
                m_comp_policy = c;
        }
        CompressionPolicy compression_policy()
        {
                return m_comp_policy;
        }


        std::istream * get_istream( const std::string & src, bool AsFile )
        {
                // if( "-" == src ) return &std::cin; // i'd like
                // that, but clients will then delete it :/.

                if( ! AsFile )
                {
                        return new std::istringstream( src );
                }


                {
                        std::ifstream check( src.c_str() );
                        if( ! check.good() ) return 0;
                        // let check go out of scope, closing it
                }

                
                FILE * inf = fopen( src.c_str(), "rb" );
                if( ! inf )
                {
                        CERR << "fopen("<<src<<") failed!" << std::endl;
                        return 0;
                }
                unsigned char buff[4];
                fread( buff, sizeof(buff), 1, inf );
                fclose( inf );
#if HAVE_BZLIB
                // bzip=5a42 3968 (dec: 90 66 57 104) ascii: ZB9h
                if( 'B' == buff[0] && 'Z' == buff[1] )
                {
                        //COUT << "bzip!"<<std::endl;
                        return new zfstream::ibzstream( src.c_str() );
                }
#endif                
#if HAVE_ZLIB
                // gzip=8b1f 0808 (dec: 139 31 8 8)
                if( 0x1f == buff[0] && 0x8b == buff[1] ) // maybe a bug here? 
                                                         // is this ordering endian dependent?
                {
                        //COUT << "gzip!"<<std::endl;
                        return new zfstream::igzstream( src.c_str() );
                }
#endif            
                return new std::ifstream( src.c_str() );
        }

        std::ostream *
        get_ostream( const std::string & fname )
        {
                switch( zfstream::compression_policy() )
                {
#if HAVE_ZLIB
                  // case GZipCompression:
                  // REMINDER: when/if GZip/ZLib mean different things, this will break!
                  // C++ won't let me put both in here when they have the same value.
                  case ZLibCompression: return new zfstream::ogzstream( fname.c_str() );
#endif
#if HAVE_BZLIB
                  case BZipCompression: return new zfstream::obzstream( fname.c_str() );
#endif
                  default:
                          return new std::ofstream( fname.c_str() );
                }
        }


} // namespace zfstream
