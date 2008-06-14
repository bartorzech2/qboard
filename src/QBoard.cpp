#include "QBoard.h"

#include <s11n.net/s11n/s11nlite.hpp>
#include <QDebug>

#include "S11nQt.h"
QBoard::QBoard() 
    : QObject(), Serializable("QBoard")
{
    this->s11nFileExtension( ".QBoard" );
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
		if( var.isValid() )
		{
		    this->load( var.toString() );
		}
		else
		{
		    this->m_file = QString();
		    this->m_px = QPixmap();
		    emit loaded();
		}
	    }
	    return true;
	}
	return QObject::event(e);
}


bool QBoard::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    return s11n::serialize_subnode( dest, "pixmap", this->m_file );
    //&& s11n::serialize_subnode( dest, "size", this->m_px.size() );
}

bool QBoard::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    this->m_px = QPixmap();
    // FIXME: save relative path to m_file.
    S11nNode const * ch = s11n::find_child_by_name( src, "file" );
    if( ! ch )
    { // newer name
	ch = s11n::find_child_by_name( src, "pixmap" );
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
    if( ! this->load(this->m_file) ) return false;
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
    m_px = QPixmap();
    m_file = "";
}

bool QBoard::save( QString const & fn) const
{
    return this->Serializable::save(fn);
}
bool QBoard::load( QString const & fn)
{
    this->m_file = fn;
    if( this->fileNameMatches(fn) )
    {
	if( this->Serializable::load( fn ) )
	{
	    emit loaded();
	    return true;
	}
    }
    else
    {
	if( this->m_px.load(fn) )
	{
	    emit loaded();
	    return true;
	}
    }
    return false;
}
