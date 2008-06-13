#include "utility.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include "QGIGamePiece.h" // a horrible, horrible dependency!

namespace qboard {
	QDir home()
	{
		
		return QDir( QDir::home().absolutePath() + "/.QBoard" );
	}
	QString homeRelative( QString const & fn )
	{
		QDir hm( home() );
		QString habs( hm.absolutePath() );
		QFileInfo fi(fn);
		if( fn.isEmpty() || (!fi.isAbsolute()) 
			|| (!fn.startsWith(habs) ) )
		{
			return fn;
		}
		QString rel( fn.mid( habs.length() ) );
		while( (!rel.isEmpty()) && (rel[0] == '/') ) rel.remove(0,1);
		return rel;
	}

	qreal nextZLevel( QGraphicsItem const * gi )
	{
		if( ! gi ) return 0.0;
		typedef QList<QGraphicsItem *> QL;
		QL li = gi->collidingItems(Qt::IntersectsItemBoundingRect);
		QL::iterator it = li.begin();
		if( it == li.end() ) return gi->zValue();
		qreal zmax = gi->zValue();
		const qreal myZ = zmax;
		const qreal stride = 0.01;
		for( ; it != li.end(); ++it )
		{
			qreal tmp = (*it)->zValue();
			if( zmax < tmp ) zmax = tmp;
			else if( tmp == myZ )
			{ // we can't know if we're at the same level here, so we'll advance.
				zmax += stride;
			}
		}
		if( myZ < zmax )
		{
			zmax += stride;
		}
		return zmax;
	} // nextZLevel()
	
	int stringToPenStyle( QString const &val )
	{
		if( val == "SolidLine" ) return Qt::SolidLine;
		else if( val == "DashLine" ) return Qt::DashLine;
		else if( val == "DotLine" ) return Qt::DotLine;
		else if( val == "DashDotLine" ) return Qt::DashDotLine;
		else if( val == "DashDotDotLine" ) return Qt::DashDotDotLine;
		return Qt::NoPen;
	}
	QString penStyleToString( int i )
	{
		switch( i )
		{
		case Qt::SolidLine: return "SolidLine";
		case Qt::DashLine: return "DashLine";
		case Qt::DotLine: return "DotLine";
		case Qt::DashDotLine: return "DashDotLine" ;
		case Qt::DashDotDotLine: return "DashDotDotLine";
		default:
			break;
		}
		return "NoPen";
	}

	void destroy( QList<QGraphicsItem *> & tops )
	{
		typedef QList<QGraphicsItem *> QL;
		for( QL::iterator it = tops.begin(); tops.end() != it; ++it )
		{
			if( (*it)->parentItem() )
			{
				qDebug() << "qboard::destroy(QList) skipping parented item.";
				continue;
			}
#if 0 // unfortunate klude for now
			if( QGIGamePiece * pcv = dynamic_cast<QGIGamePiece*>(*it) )
			{ // this needs to go away!!!!
				pcv->destroyWithPiece();
				continue;
			}
#endif
#if 1 // i don't like this, but it avoids some crashes!
			if( QObject * obj = dynamic_cast<QObject*>(*it) )
			{ // kludge to help avoid stepping on self during Destroy/Delete actions
				qDebug() << "qboard::destroy(QList) using obj->deleteLater():" <<obj;
				obj->deleteLater(); // huh? not destroying QGILineBinder???
			}
			else
			{
				qDebug() << "qboard::destroy(QList) delete *it:"<<*it;
				delete *it;
			}
#else
			qDebug() << "qboard::destroy(QList) delete *it:"<<*it;
			delete *it;
#endif

		}
		tops.clear();
	}
	void destroy( QGraphicsItem * gi, bool alsoSelected )
	{
		if( ! gi ) return;
		typedef QList<QGraphicsItem *> QL;
		QL tops;
		if( alsoSelected )
		{
			tops = gi->scene()->selectedItems();
		}
		else
		{
			tops.push_back(gi);
		}
		destroy( tops );
	}
	
	
	QList<QColor> colorList()
	{
		typedef QList<QColor> QL; 
		static QL colors;
		if( colors.isEmpty() )
		{
			colors.push_back( Qt::white );
			colors.push_back( Qt::black );
			colors.push_back( Qt::red );
			colors.push_back( Qt::darkRed );
			colors.push_back( Qt::green );
			colors.push_back( Qt::darkGreen );
			colors.push_back( Qt::blue );
			colors.push_back( Qt::darkBlue );
			colors.push_back( Qt::cyan );
			colors.push_back( Qt::darkCyan );
			colors.push_back( Qt::magenta );
			colors.push_back( Qt::darkMagenta );
			colors.push_back( Qt::yellow );
			colors.push_back( Qt::darkYellow );
			colors.push_back( Qt::lightGray );
			colors.push_back( Qt::gray );
			colors.push_back( Qt::darkGray );
		}
		return colors;
	}
} // namespace
