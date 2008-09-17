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

#include <qboard/QBoard.h>

#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QString.h>
#include <QDebug>
#include <QVariant>


QBoard::QBoard() 
    : QObject(),
      Serializable("QBoard")
{
}

QBoard::~QBoard()
{
    //qDebug() << "~QBoard()";
}
#include <QDynamicPropertyChangeEvent>
bool QBoard::event( QEvent * e )
{
	if( e->type() == QEvent::DynamicPropertyChange )
	{
	    e->accept();
	    QDynamicPropertyChangeEvent * dev = dynamic_cast<QDynamicPropertyChangeEvent*>(e);
	    QString key(dev->propertyName());
	    if( QString("pixmap") == key )
	    {
		QVariant var( this->property("pixmap") );
		if( var.canConvert<QPixmap>() )
		{
		    this->m_file.clear();
		    this->m_px = var.value<QPixmap>();
		}
		else if( var.canConvert<QString>() )
		{
		    this->m_file = var.value<QString>();
		    this->m_px = QPixmap(m_file);
		}
		Q_EMIT loadedBoard();
	    }
	    return true;
	}
	return QObject::event(e);
}


bool QBoard::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    return s11n::serialize_subnode( dest, "pixmap", this->m_file );
    //&& s11n::serialize_subnode( dest, "size", this->m_px.size() );
}

bool QBoard::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    this->m_px = QPixmap();
    // FIXME? ensure we have a relative path for m_file?
    S11nNode const * ch = s11n::find_child_by_name( src, "pixmap" );
    if( ! ch )
    { // try older name for this property...
	ch = s11n::find_child_by_name( src, "file" );
    }
    if( ! ch || ! s11n::deserialize( *ch, this->m_file ) )
    {
	return false;
    }
    if( m_file.isEmpty() ) return true; // that's okay.
#if 0
    QSize szchk(0,0);
    if( ! s11n::deserialize_subnode( src, "size", szchk ) ) return false;
    if( ! this->load(this->m_file) ) return false;
    QSize psz( this->m_px.size() );
    if( szchk != psz )
    {
	throw s11n::s11n_exception(
				   "QBoard: image file [%s]: resolution changed since serialization. "
				   "Expected (%d x %d) but got (%d x %d).",
				   m_file.toStdString().c_str(),
				   szchk.width(), szchk.height(),
				   psz.width(), psz.height() );
    }
#else
    if( ! this->s11nLoad(this->m_file) ) return false;
#endif
    return true;
}
QPixmap & QBoard::pixmap()
{
    return this->m_px;
}
QPixmap const & QBoard::pixmap() const
{
    return this->m_px;
}
QSize QBoard::size() const
{
    return m_px.size();
}
void QBoard::clear()
{
#if 1
    this->setProperty("pixmap",QVariant());
#else
    m_px = QPixmap();
    m_file = "";
#endif
#if 0
    // FIXME: clear all dynamic properties?
    typedef QList<QByteArray> QL;
    QL ql( src.dynamicPropertyNames() );
    QL::const_iterator it( ql.begin() );
    QL::const_iterator et( ql.end() );
    for( ; et != it; ++it )
    {
	char const * key = it->constData();
	if( !key || (*key == '_') ) continue; // Qt reserves the "_q_" prefix, so we'll elaborate on that.
	this->setProperty(key,QVariant());
	// ^^ setProperty() ends up triggering a call to this->event() on each change.
    }
#endif
}

bool QBoard::s11nSave( QString const & fn) const
{
    return this->Serializable::s11nSave(fn, true);
}

bool QBoard::loadPixmap( QString const & fn )
{
    this->setProperty("pixmap",QVariant(fn));
    return !m_px.isNull();
}

bool QBoard::s11nLoad( QString const & fn )
{
    if( fn.isEmpty() ) return false;
    this->m_file = fn;
    if( this->fileNameMatches(fn) )
    {
	bool ret = this->Serializable::s11nLoad( fn );
	if( ret ) Q_EMIT loadedBoard();
	return ret;
    }
    else
    {
	return this->loadPixmap(fn);
    }
}
