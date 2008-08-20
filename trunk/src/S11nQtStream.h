#ifndef QBOARD_S11nQtStream_H_INCLUDED
#define QBOARD_S11nQtStream_H_INCLUDED
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

#include <iostream>
#include <QIODevice>

/**
   StdToQtOBuf is a helper to divert output intended for a
   std::ostream to a QIODevice instead. The intention is to allow Qt
   applications which make heavy use of the STL interact more
   naturally with Qt. Another use is to get the use of Unicode file
   names (supported by Qt) with STL-based stream APIs by opening a
   QFile to open the file and then use the STL-based stream API to
   read/write from/to it.

   This object is intended to be used as a proxy, like:

\code
  QFile outfile("my.file");
  if( outfile.open(QIODevice::WriteOnly) )
  {
    std::ostringstream dummy;
    StdToQtOBuf sentry( dummy, out );
    ... output to dummy. It goes to outfile instead ...
    // For example:
    s11nlite::save( dummy, myS11nableObject );
    ...
  }
\endcode

   Limitations: only write is supported. No lookback, no re-get,
   etc.
*/
class StdToQtOBuf : public std::streambuf
{
private:
    struct Impl;
    Impl * impl;
public:
    typedef std::streambuf::char_type char_type;
    typedef std::streambuf::traits_type traits_type;

    /**
       Sets in.rdbuf(this) and sets up to divert
       all output to qio_out.

       If !qio_out.isOpen() then qio_out.open(QIODevice::WriteOnly)
       is called. If the device is not open and cannot be opened
       then an std::exception is thrown.

       The caller is responsible for ensuring that both
       streams outlive this object.
    */
    StdToQtOBuf( std::ostream & std_out,
		    QIODevice & qio_out );

    /**
       If the ostream still has this object as its
       rdbuf() then this function re-sets the rdbuf
       to its original state.

       This does not close the associated streams.
    */
    virtual ~StdToQtOBuf();

    /**
       If !traits_type::not_eof(c) then it writes c to the QIODevice
       and returns true, else is does nothing and returns
       false. traits_type::eof().
    */
    virtual int overflow(int c);
private:
    //! Copying not allowed.
    StdToQtOBuf( const StdToQtOBuf & );
    //! Copying not allowed.
    StdToQtOBuf & operator=( const StdToQtOBuf & );

};

/**
   StdToQtIBuf is a helper to redirect input aimed at std::istreams to
   a QIODevice instead. This allows us to plug in QIODevices to
   various types of std::istream. The intention is to allow Qt
   applications to interact more naturally with libraries making heavy
   use of the STL streams.

   This object is intended to be used as a proxy, like:

\code
  QFile infile("my.file");
  if( infile.open(QIODevice::ReadOnly) )
  {
    std::istreamstream dummy;
    StdToQtOBuf sentry( dummy, infile );
    ... input from dummy, but it will actually come from infile ...
    // For example
    s11nlite::node_type * node = s11nlite::load_node( dummy);
    ...
  }
\endcode

   Limitations: only readahead is supported. No lookahead, no putback,
   etc.
*/
class StdToQtIBuf : public std::streambuf
{
private:
    struct Impl;
    Impl * impl;
public:
    typedef std::streambuf::char_type char_type;
    typedef std::streambuf::traits_type traits_type;
    /**
       Sets std_in.rdbuf(this) and sets up to divert
       all output to qio_in.

       If !qio_in.isOpen() then qio_in.open(QIODevice::ReadOnly)
       is called. If the device is not open and cannot be opened
       then an std::exception is thrown.

       The caller is responsible for ensuring that both
       streams outlive this object.
    */
    StdToQtIBuf( std::istream & std_in,
		 QIODevice & qio_in );
    /**
       If the istream still has this object as its
       rdbuf() then this function re-sets the rdbuf
       to its original state.

       This does not close the associated streams.
    */
    virtual ~StdToQtIBuf();

protected:
    /**
       Fetches the next character(s) from the QIODevice.
    */
    virtual int_type underflow();
private:
    //! Copying not allowed.
    StdToQtIBuf( const StdToQtIBuf & );
    //! Copying not allowed.
    StdToQtIBuf & operator=( const StdToQtIBuf & );
};



#endif // QBOARD_S11nQtStream_H_INCLUDED