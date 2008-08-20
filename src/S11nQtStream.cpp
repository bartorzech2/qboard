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

#include "S11nQtStream.h"
#include <stdexcept>
struct StdToQtOBuf::Impl
{
    // FIXME? add a buffer?
    std::ostream & in;
    QIODevice & out;
    std::streambuf * oldBuf;
    Impl(std::ostream & i, QIODevice & o ) :
	in(i),
	out(o),
	oldBuf(i.rdbuf())
    {
	if( ! o.isOpen() )
	{
	    o.open(QIODevice::WriteOnly);
	}
	if( o.openMode() != QIODevice::WriteOnly )
	{
	    throw std::runtime_error("StdToQtOBuf::StdToQtOBuf() requires that the QIODevice be open WriteOnly");
	}
    }
    ~Impl()
    {
    }
};


StdToQtOBuf::StdToQtOBuf( std::ostream & in,
			  QIODevice & out )
    : impl(new Impl(in,out))
{
    this->setp(0,0);
    this->setg(0,0,0);
    in.rdbuf( this );
}

StdToQtOBuf::~StdToQtOBuf()
{
    std::streambuf * rb = impl->in.rdbuf();
    if( rb == this )
    {
	impl->in.rdbuf( impl->oldBuf );
    }
    delete impl;
}


int StdToQtOBuf::overflow(int c)
{
    typedef traits_type CT;
    if (!CT::eq_int_type(c, CT::eof()))
    {
        return impl->out.putChar(c)
	    ? CT::not_eof(c)
	    : CT::eof();
    }
    return CT::eof();
}


struct StdToQtIBuf::Impl
{
    /**
       FIXME: add a reasonbly-sized buffer!
    */
    std::istream & sstr;
    QIODevice & qstr;
    std::streambuf * oldBuf;
    QByteArray bufa;
    static const int bufsize = 1024;
    Impl(std::istream & i, QIODevice & o ) :
	sstr(i),
	qstr(o),
	oldBuf(i.rdbuf()),
	bufa(bufsize,0)
    {
	if( ! o.isOpen() )
	{
	    o.open(QIODevice::ReadOnly);
	}
	if( o.openMode() != QIODevice::ReadOnly )
	{
	    throw std::runtime_error("StdToQtIBuf::StdToQtIBuf() requires that the QIODevice be open ReadOnly");
	}
    }
    ~Impl()
    {
    }
};


StdToQtIBuf::StdToQtIBuf( std::istream & in,
			  QIODevice & out )
    : impl(new Impl(in,out))
{
    this->setp(0,0);
    this->setg(0,0,0);
    in.rdbuf( this );
}

StdToQtIBuf::~StdToQtIBuf()
{
    std::streambuf * rb = impl->sstr.rdbuf();
    if( rb == this )
    {
	impl->sstr.rdbuf( impl->oldBuf );
    }
    delete impl;
}

int StdToQtIBuf::underflow()
{
    char * dest = impl->bufa.data();
    qint64 rd = impl->qstr.read( dest, Impl::bufsize );
    if( rd < 1 )
    {
	return traits_type::eof();
    }
    this->setg(dest,dest,dest+rd);
    return traits_type::not_eof(*dest);
}
