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

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QStringList.h>
#include <qboard/PathFinder.h>

namespace qboard {

struct PathFinder::Impl
{
    QStringList prefix;
    QStringList suffix;
    Impl()
    {
    }
    ~Impl()
    {
    }
};

PathFinder::PathFinder( QObject * parent ) :
    QObject(parent),
    Serializable("PathFinder"),
    impl(new Impl)
{
}

PathFinder::~PathFinder()
{
    delete impl;
}

bool PathFinder::empty() const
{
    return impl->prefix.isEmpty()
	&& impl->suffix.isEmpty();
}
void PathFinder::clear()
{
    impl->prefix.clear();
    impl->suffix.clear();
}
QStringList PathFinder::prefixes() const
{
    return impl->prefix;
}
QStringList PathFinder::suffixes() const
{
    return impl->suffix;
}

QString PathFinder::addPrefix( QString const & p, bool autoAppendSlash )
{
    QString n(p);
    if( autoAppendSlash )
    {
	QChar sep('/'); // QDir::separator()???
	if( ! n.endsWith( sep ) )
	{
	    n.append(sep);
	}
    }
    impl->prefix << n;
    return n;
}
void PathFinder::addSuffix( QString const & p )
{
    impl->suffix << p;
}

//! internal impl of PathFinder::removePrefix/Suffix()
static void removePathItem( QStringList & li,
			QString const & s,
			bool all )
{
    if( all )
    {
	li.removeAll(s);
    }
    else
    {
	int i = li.lastIndexOf(s);
	if( -1 != i ) li.removeAt(i);
    }
}
void PathFinder::removePrefix( QString const & s, bool all )
{
    removePathItem( impl->prefix, s, all );
}
void PathFinder::removeSuffix( QString const & s, bool all )
{
    removePathItem( impl->suffix, s, all );
}

QString
PathFinder::find( QString const & baseName,
		  PathFinder::SearchFlags flags ) const
{
    if( ! flags ) flags = Files;
    QStringList prefix( impl->prefix );
    QStringList suffix( impl->suffix );
    prefix.push_front("");
    suffix.push_front("");
    Q_FOREACH( QString pre, prefix ){
	Q_FOREACH( QString suf, suffix )
	{
	    QString fn = pre + baseName + suf;
	    //qDebug() << "PathFinder::find() trying:"<<fn;
	    QFileInfo fi(fn);
	    if( ! fi.exists() ) continue;
	    if( fi.isDir() )
	    {
		if(flags&Dirs) return fn;
		else continue;
	    }
	    return fn;
	}
    }
    return QString();
}



bool PathFinder::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    return s11n::serialize_subnode( dest, "prefix", impl->prefix )
	&& s11n::serialize_subnode( dest, "suffix", impl->suffix );
}

bool PathFinder::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    this->clear();
    return s11n::deserialize_subnode( src, "prefix", impl->prefix )
	&& s11n::deserialize_subnode( src, "suffix", impl->suffix );
}


} // namespace
