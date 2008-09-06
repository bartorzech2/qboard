#ifndef zfstream_FILE_UTIL_H_INCLUDED
#define zfstream_FILE_UTIL_H_INCLUDED 1

#include <string>

/**
   The zfstream namespace contains several functions and classes for working
   with files compressed with libz or libbz2 compression.
*/
namespace zfstream {

        /**
           CompressionPolicy describes the framework-wide policy
           regarding compression when writing to files. (Sorry, pure
           stream-based de/compression isn't yet supported.)

           bzip compresses much better than gzip, but is notably
           slower. The speed should not make much of a difference
           unless you are working with very large data sets, and then
           you will definately notice that gzip is up to almost twice
           as fast. For many large data sets where the compression
           ratio is very high (95%+) and using GZipCompression can
           actually speed up data transfers, compared non-compressed
           streams.

           Tip: you can use get_istream() on an arbitrary filename to
           get an appropriate input stream. Likewise, use
           get_ostream() to open writable a file with the
           policy-mandated stream type (if possible, defaulting to
           std::ofstream).
         */
        enum CompressionPolicy {
	        /** NoCompression implies just what it says - do not use compressed output streams. */
	        NoCompression = 0,
                /** ZLibCompression means to use the zfstream::ogzstream class for output streams. */
                ZLibCompression = 1,
                /** GZipCompression is currently the same as ZLibCompression.

                    In the future GZipCompression MAY be used to
                    distinguish between zlib's deflate() algorith vs
                    zlib'z gzXXX() functions (which are used by
                    ZLibCompression). This depends on whether we get
                    streams which support both types, in which case it
                    makes sense to split these.
                 */
        	GZipCompression = ZLibCompression,
                /** BZipCompression means to use the zfstream::obzstream class for output streams. */
	        BZipCompression = 2
        };

        /**
           Sets the framework-wide CompressionPolicy preference.
           See the zfstream::CompressionPolicy enum.

           Setting compression_policy() to ZLib/BZipCompression only
           enables compression if HAVE_ZLIB or HAVE_BZLIB ars set
           during library compilation, otherwise non-compressed
           streams are used in all cases.
         */
        void compression_policy( CompressionPolicy c );

        /**
           Returns the framework-wide CompressionPolicy preference.
           See the zfstream::CompressionPolicy enum.
         */
        CompressionPolicy compression_policy();

        /**
           Returns true if this copy of zfstream supports the given
           compression policy. This is determined at library
           build-time.
        */
        bool supports_compression_policy( CompressionPolicy c );


        /**
           Looks at the given file and tries to figure out what type
           of decompression stream, if any, should be used.
           It will return one of the following types:

           - std::ifstream
           - zfstream::gzstream
           - zfstream::bzstream

           It will return NULL if it cannot open filename, or a
           std::ifstream if it cannot figure out a decompression
           scheme.

           It will only return one of the compressed stream types if
           this code is built with the appropriate macros: HAVE_ZLIB
           and HAVE_BZLIB, and then linked against -lz and/or -lbz2.
           Thus, if this code is not built with compression support it
           is possible that it returns a std::ifstream reading from a
           compressed file.

           If AsFilename is true, input is treated as a file,
           otherwise it is treated as a string, for which an
           istringstream is returned. This feature has subtle uses in
           writing more generic client code for handling arbitrary
           input streams. Compressor streams are currently only
           supported when AsFile is true.
        */
        std::istream * get_istream( const std::string & src, bool AsFile = true );


        /**
           Returns one of the following types of ostreams, depending
           on compression_policy() and compile-time library settings
           of HAVE_ZLIB and HAVE_BZLIB.

           - zfstream::ogzstream
           - zfstream::obzstream
           - std::ofstream

           Note that this function only uses filename to pass to the
           stream's constructor, and does no checking of the file.

           The caller is responsible for deleting the pointer.

           For determining the type of input stream for a file, 
           see zfstream::get_istream().
        */
        std::ostream * get_ostream( const std::string & filename );

} // namespace zfstream

#endif // zfstream_FILE_UTIL_H_INCLUDED
