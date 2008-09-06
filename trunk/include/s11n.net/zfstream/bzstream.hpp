// ============================================================================
// bzstream, C++ iostream classes wrapping the bz2lib compression library.
// Copyright (C) 2001  Deepak Bandyopadhyay, Lutz Kettner
// Copyright (C) 2003 stephan beal <stephan@s11n.net>
//
// This code is basically a copy/paste of gzstream, adapted for libbz2
// by stephan@s11n.net (2 Oct 2003)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ============================================================================
//
// File          : bzstream.h
// Revision      : $Revision: 1.1 $
// Revision_date : $Date: 2005/05/25 18:20:56 $
// Author(s)     : Deepak Bandyopadhyay, Lutz Kettner
// 
// Standard streambuf implementation following Nicolai Josuttis, "The 
// Standard C++ Library".
// ============================================================================

//    Changes by stephan@s11n.net:
//    October, 2003:
//    - added zlevel() to gzstreambase and gzstreambuf. Only works for ostreams.
//    - replaced gzip code with bzip code.

#ifndef zfstream_BZSTREAM_H_INCLUDED
#define zfstream_BZSTREAM_H_INCLUDED 1


// standard C++ with new header file names and std:: namespace
#include <iostream>
#include <fstream>

#include <bzlib.h> // libbz2 header


namespace zfstream
{

// ----------------------------------------------------------------------------
// Internal classes to implement bzstream. See below for user classes.
// ----------------------------------------------------------------------------
	class bzstreambuf:public std::streambuf
	{
	      private:
		static const int bufferSize = 47 + 256;	// size of data buff
		// totals 512 bytes under g++ for ibzstream at the end.

		BZFILE * file;	// file handle for compressed file
		char buffer[bufferSize];	// data buffer
		char opened;	// open/close state of stream
		int mode;	// I/O mode

		int flush_buffer(  );
	      public:
                bzstreambuf(  ):opened( 0 ), m_zlevel(-1)
		{
			setp( buffer, buffer + ( bufferSize - 1 ) );
			setg( buffer + 4,	// beginning of putback area
			      buffer + 4,	// read position
			      buffer + 4 );	// end position      
			// ASSERT: both input & output capabilities will not be used together
		}
		int is_open(  )
		{
			return opened;
		}
		bzstreambuf *open( const char *name, int open_mode );
		bzstreambuf *close(  );
		~bzstreambuf(  )
		{
			close(  );
		}

		virtual int overflow( int c = EOF );
		virtual int underflow(  );
		virtual int sync(  );

                /**
                   Defines the compression level, 0 - 9. A value of -1
                   means use the system-wide zlib default (normally 3).
                 */
                void zlevel( int z ) { this->m_zlevel = ( z<0 ? -1 : (z>9?9:z) ) ; }
                /**
                   Returns the compression level for this stream.
                 */
                int zlevel() const  { return this->m_zlevel; }
        private:
                int m_zlevel;
	};

        /**
           Base stream class inherited by obzstream/igzstream. Any
           given instance may not be used for both input and output.

           Uses bzip2 compression, and is compatible with files compressed
           using bzip2, or any number of other tools which support bz2lib.

        */
	class bzstreambase:virtual public std::ios
	{
        private:
                int m_zlevel;
	      protected:
		bzstreambuf buf;
	      public:
		bzstreambase(  )
		{
			init( &buf );
		}
		bzstreambase( const char *name, int open_mode );
		~bzstreambase(  );
		void open( const char *name, int open_mode );
		void close(  );

                void zlevel( int z ) { buf.zlevel(z); }
                int zlevel() const  { return buf.zlevel(); }

		bzstreambuf *rdbuf(  )
		{
			return &buf;
		}
	};

        /**
           An input stream which decompresses it's input. Used
           identically to a std::ifstream.

<pre>
igzstream ifs( "myfile.bz2" );
</pre>
        */
	class ibzstream:public bzstreambase, public std::istream
	{
	      public:
		ibzstream(  ):std::istream( &buf )
		{
		}
	      ibzstream( const char *name, int open_mode = std::ios::in ):bzstreambase( name, open_mode ), std::istream( &buf )
		{
		}
		bzstreambuf *rdbuf(  )
		{
			return bzstreambase::rdbuf(  );
		}
		void open( const char *name, int open_mode = std::ios::in )
		{
			bzstreambase::open( name, open_mode );
		}
	};

        /**
           An output stream which compresses it's output. Used identically to a std::ofstream:

<pre>
ogzstream of( "myfile.bz2" );
of << "hello, world!" << std::endl;
</pre>
        */

	class obzstream:public bzstreambase, public std::ostream
	{
	      public:
		obzstream(  ):std::ostream( &buf )
		{
		}
	      obzstream( const char *name, int mode = std::ios::out ):bzstreambase( name, mode ), std::ostream( &buf )
		{
		}
		bzstreambuf *rdbuf(  )
		{
			return bzstreambase::rdbuf(  );
		}
		void open( const char *name, int open_mode = std::ios::out )
		{
			bzstreambase::open( name, open_mode );
		}
	};

}				// namespace

#endif // zfstream_BZSTREAM_H_INCLUDED
// ============================================================================
// EOF //
