#ifndef S11NQT_QGRAPHICSITEM_H_INCLUDED
#define S11NQT_QGRAPHICSITEM_H_INCLUDED 1

#include <QGraphicsItem>
#include "QList.h"

namespace s11n { namespace qt {
    /**
       For each item in src, if it can be cast to a (SerializableT*)
       then it is serialized, otherwise it is skipped.

       Returns the number of objects serialized, or -1 if
       serialization fails. It may propogate an exception on error.
    */
    template <typename SerializableT>
    int serializeQGIList( S11nNode & dest,
			  QList<QGraphicsItem *> const & src,
			  bool skipParentedItems )
    {
	typedef QList<QGraphicsItem *> QGIL;
	typedef QList<SerializableT *> QSL;
	QSL serlist;
	for( QGIL::const_iterator it = src.begin();
	     src.end() != it; ++it )
	{
	    if( skipParentedItems && (*it)->parentItem() ) continue;
	    SerializableT * ser = dynamic_cast<SerializableT *>(*it);
	    if( ! ser ) continue;
	    serlist.push_back(ser);
	}
	return s11n::serialize( dest, serlist )
	    ? serlist.size()
	    : -1;
    }

    /**
       The converse of serializeQGIList(), it extracts
       (SerializableT*) objects from src and tries to cast
       them to QGraphicsItems. Dest is populated with all
       items for which the conversion succeeds.

       Returns the number of deserialized items, or -1 if
       deserialization fails. It may propogate an exception on error.
    */
    template <typename SerializableT>
    int deserializeQGIList( S11nNode const & src,
			    QList<QGraphicsItem*> & dest,
			    QGraphicsItem * parent = 0,
			    QGraphicsItem::GraphicsItemFlags flags = 0 )
    {
	typedef QList<QGraphicsItem*> QGIL;
	typedef QList<SerializableT *> QSL;
	QSL serlist;
	if( ! s11n::deserialize( src, serlist ) ) return -1;
	int ret = 0;
	for( typename QSL::iterator it = serlist.begin();
	     serlist.end() != it; ++it )
	{
	    QGraphicsItem * qgi = dynamic_cast<QGraphicsItem *>(*it);
	    if( ! qgi )
	    {
		s11n::cleanup_serializable( (*it) );
		continue;
	    }
	    if( parent ) qgi->setParentItem( parent );
	    if( flags ) qgi->setFlags( flags );
	    dest.push_back(qgi);
	    ++ret;
	}
	return ret;
    }

}}

#endif
