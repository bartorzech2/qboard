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

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDialog>
#include <QGridLayout>
#include <QTextEdit>
#include <QDebug>
#include <sstream>

#include "utility.h"
#include "QGIGamePiece.h" // a horrible, horrible dependency!
#include "S11nClipboard.h" // another horrible dep!
#include "S11nQt.h"
#include "S11nQtList.h"
#include "GamePiece.h"
#include "GameState.h"

/************************************************************************
The sad, sad story of the QBOARD_VERSION variable...

Ideally, this var would be set by a configure script. However, we can't
do that cross-platform with qmake. We can pass it as a #define, however.
But... it is not possible to properly escape/quote the value of a string
from here. i've seen cases where, e.g. an IDE which saves the .pro
file will mangle the escaping, breaking the build.

So... the current build process sets QBOARD_VERSION to 0 when we are
in an svn checkout tree. We convert that 0 to some usable string. If
QBOARD_VERSION is defined as non-zero then it is used as-is. e.g. in the
source releases config.qmake is filtered to contain the release version.

************************************************************************/

#if defined(QBOARD_VERSION)
#  if ! QBOARD_VERSION
#  undef QBOARD_VERSION
#  define QBOARD_VERSION "SVN [" __DATE__ " " __TIME__ "]"
#  endif
#endif

#if !defined(QBOARD_VERSION)
#  define QBOARD_VERSION "Unknown"
#endif

namespace qboard {

    const QString versionString()
    {
	// Reminder: we do this .arg() bit so we can use a numeric or string QBOARD_VERSION
	return QString("%1").arg(QBOARD_VERSION);
    }

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
		QGraphicsScene * sc = 0;
		for( QL::iterator it = tops.begin(); tops.end() != it; ++it )
		{
			if( (*it)->parentItem() )
			{
			    //qDebug() << "qboard::destroy(QList) skipping parented item.";
				continue;
			}
#if 1 // i don't like this, but it avoids some crashes!
			if( QObject * obj = dynamic_cast<QObject*>(*it) )
			{ // kludge to help avoid stepping on self during Destroy/Delete actions
				qDebug() << "qboard::destroy(QList) using obj->deleteLater():" <<obj;
				obj->deleteLater(); // huh? not destroying QGILineBinder???
			}
			else
			{
			    sc = (*it)->scene();
			    if( sc )
			    {
				qDebug() << "qboard::destroy(QList) asking Scene to remove *it:"<<*it;
				sc->removeItem(*it);
			    }
			    qDebug() << "qboard::destroy(QList) delete *it:" <<*it;
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

    QDir persistenceDir( QString const & className )
    {
	QDir dir( QString("%1/QBoard/persistance/%2").arg(home().canonicalPath()).arg(className) );
	QString dname( dir.absolutePath() );
	// dir.canonicalDir() gives me an empty string?
	//qDebug() << "persistenceDir("<<className<<") =?"<<dname<<"=?"<<dir;
	if( (! dir.exists()) && (! dir.mkpath(dname)) )
	{
	    QString msg = QString("Could not access or create directory [%1]").arg(dname);
	    throw std::runtime_error( msg.toAscii().constData() );
	}
	return dir;
    }

//     QDir helpDir( QString const & className )
//     {
// 	QDir dir( QString("%1/QBoard/help/%2").arg(home().canonicalPath()).arg(className) );
// 	QString dname( dir.absolutePath() );
// 	if( (! dir.exists()) && (! dir.mkpath(dname)) )
// 	{
// 	    QString msg = QString("Could not access or create directory [%1]").arg(dname);
// 	    throw std::runtime_error( msg.toAscii().constData() );
// 	}
// 	return dir;
//     }

    QString loadResourceText( QString const & res )
    {
	QFile inf(res);
	if( ! inf.open(QIODevice::ReadOnly|QIODevice::Text) )
	{
	    qDebug() <<"qboard::loadResourceText() could not load help resource:"<<res;
	    return QString();
	}
	QString buf;
	while (!inf.atEnd())
	{
	    buf.append(inf.readLine());
	}
	return buf;
    }

    void showHelpResource( QString const & title, QString const & res )
    {
#if 1
	QDialog dlg;
	QTextEdit * ed = new QTextEdit();
	ed->setReadOnly(true);
	QGridLayout * lay = new QGridLayout();
	lay->setSpacing(2);
	dlg.setLayout(lay);
	dlg.setWindowTitle( title );
	dlg.setSizeGripEnabled(true);
	lay->addWidget(ed);
	QString txt( qboard::loadResourceText(res) );
	if( txt.isEmpty() )
	{
	    txt = QString("Error loading text resource:<br/><tt>%1</tt>").arg(res);
	}
	ed->setHtml( txt );
	dlg.exec();
#else
	// We can't set a reasonable starting position, or else i'd
	// prefer to use this approach:
	QTextEdit * ed = new QTextEdit();
	ed->setAttribute( Qt::WA_DeleteOnClose, true );
	ed->setReadOnly(true);
	ed->setWindowTitle( title );
	QString txt( qboard::loadResourceText(res) );
	if( txt.isEmpty() )
	{
	    txt = QString("Error loading text resource:<br/><tt>%1</tt>").arg(res);
	}
	ed->setHtml( txt );
	ed->show();
#endif
    }

    static char const * KeyS11nQGI = "GameStateClipboardData";
    bool clipboardGraphicsItems( QGraphicsItem * gvi, bool copy )
    {

	if( ! gvi ) return false;
	//qDebug() <<"qboard::clipboardGraphicsItems(item,"<<copy<<")";
	typedef QList<QGraphicsItem *> QGIL;
	typedef QList<Serializable *> SerL;
	SerL seritems;
	GamePieceList pieces;
	QGIL toCut;
	QPoint origin( gvi->pos().toPoint() );
	if( gvi->isSelected() )
	{ // handle all selected items:
	    QGIL ql( gvi->scene()->selectedItems() );
	    for( QGIL::iterator it = ql.begin(); ql.end() != it; ++it )
	    {
		if( (*it)->parentItem() ) continue;
		//qDebug() <<"qboard::clipboardGraphicsItems() marking " << *it;
		if( !copy ) toCut.push_back(*it); // FIXME? only cut serializables?
		if( (*it)->type() == QGITypes::GamePiece )
		{
		    QGIGamePiece * pcv = dynamic_cast<QGIGamePiece*>(*it);
		    GamePiece * pc = pcv ? pcv->piece() : 0;
		    if( pc )
		    {
			pieces.addPiece(pc);
			if(0) qDebug() <<"qboard::clipboardGraphicsItems() copy/cut object is GamePiece:" << pc;
		    }
		    else
		    {
			if(0) qDebug() << (copy?"copy":"cut")<<"handler found QGIGamePiece with no associated GamePiece!"
				 << "Skipping object "<<*it;
		    }
		    continue;
		}
		Serializable * ser = dynamic_cast<Serializable*>(*it);
		if( ser )
		{
		    //if(0) qDebug() <<"qboard::clipboardGraphicsItems() marking for CUT " << *it;
		    seritems.push_back(ser);
		}
		else
		{
		    if(0) qDebug() << (copy?"copy":"cut")<<"handler cannot handle non-Serializables."
			     << "Skipping object "<<*it;
		}
	    }
	}
	else
	{ // a single non-selected item:
	    //if(0) qDebug() <<"qboard::clipboardGraphicsItems() single object " << gvi;
	    if( ! copy ) toCut.push_back(gvi);
	    if( gvi->type() == QGITypes::GamePiece )
	    {
		QGIGamePiece * pcv = dynamic_cast<QGIGamePiece*>(gvi);
		GamePiece * pc = pcv ? pcv->piece() : 0;
		if( pc )
		{
		    pieces.addPiece(pc);
		    if(0) qDebug() <<"qboard::clipboardGraphicsItems() single object is GamePiece:" << pc;
		}
		else
		{
		    if(0) qDebug() << (copy?"copy":"cut")<<"handler found QGIGamePiece with no associated GamePiece!"
			     << "Skipping object!";
		}
	    }
	    else
	    {
		Serializable * ser = dynamic_cast<Serializable*>(gvi);
		if( ! ser )
		{
		    if(0) qDebug() << (copy?"copy":"cut")<<"handler cannot handle non-Serializables."
			     << "Skipping object "<<gvi;
		}
		else
		{
		    //if(0) qDebug() <<"qboard::clipboardGraphicsItems() single object is Serializable ??? " << ser;
		    seritems.push_back(ser);
		}
	    }
	}
	S11nNode * parent = S11nNodeTraits::create(KeyS11nQGI);
	try
	{
	    bool ret = true;
	    S11nNode & meta( s11n::create_child( *parent, "metadata") );
	    ret = s11n::serialize_subnode( meta, "copyPos", origin );
	    if( ret && ! pieces.empty() )
	    {
		ret = s11n::serialize_subnode( *parent, "pieces", pieces );
	    }
	    if( ret && ! seritems.isEmpty() )
	    {
		ret = s11n::serialize_subnode( *parent, "graphicsitems", seritems );
	    }
	    if( ! ret )
	    {
		delete parent;
		return false;
	    }
	}
	catch(std::exception const & ex)
	{
	    pieces.clearPieces(false);
	    delete parent;
	    if(0) qDebug() << "qboard::clipboardGraphicsItems(): serialization threw:"<<ex.what();
	    return false;
	}
	pieces.clearPieces(false);
	seritems.clear();
	S11nClipboard & cb( S11nClipboard::instance() );
	cb.slotCut(parent);
	if( copy )
	{
	    //if(0) qDebug() << "qboard::clipboardGraphicsItems() copied data.";
	}
	else
	{
	    //if(0) qDebug() << "qboard::clipboardGraphicsItems() cut data.";
	    qboard::destroy( toCut );
	}
	S11nClipboard::S11nNode * cont = cb.contents();
	if( cont )
	{
	    if(0) qDebug() <<"Clipboard contents:";
	    s11nlite::save( *cont, std::cout );
	}
	return 0 != cont;
    }


    bool pasteGraphicsItems( GameState & gs, QPoint const & pos )
    {
	typedef QList<QGraphicsItem *> QGIL;
	typedef QList<Serializable *> SerL;
	S11nNode * root = S11nClipboard::instance().contents();
	typedef S11nNodeTraits TR;
	if( (! root)
	    || (TR::name(*root) != KeyS11nQGI ) )
	{
	    return false;
	}
	S11nNode * node = 0;
	node = s11n::find_child_by_name(*root, "metadata");
	QPoint cpos;
	if( node )
	{
	    s11nlite::deserialize_subnode( *node, "copyPos", cpos );
	}
	QPoint pD;
	if( ! pos.isNull() && ! cpos.isNull() )
	{
	    int xD = cpos.x() - pos.x();
	    int yD = cpos.y() - pos.y();
	    pD = QPoint( xD, yD );
	}
	node = s11n::find_child_by_name(*root, "pieces");
	if( node )
	{
	    GamePieceList list;
	    if( ! s11nlite::deserialize( *node, list ) )
	    {
		return false;
	    }
	    GamePiece * pc = 0;
	    for( GamePieceList::iterator it = list.begin();
		 list.end() != it;
		 ++it )
	    {
		pc = *it;
		QPoint newpos;
		QVariant vpos( pc->property("pos") );
		if( ! vpos.isValid() )
		{
		    newpos = pos;
		}
		else
		{
		    newpos = vpos.toPoint();
		}
		newpos -= pD;
		pc->setPieceProperty("pos",newpos);
		gs.pieces().addPiece( pc );
		if(0) qDebug() << "qboard::pasteGraphicsItems() pasting piece:"<<pc
			 << "\nclickpos ="<<cpos<<", reqpos ="<<pos<<", newpos ="<<newpos<<"delta ="<<pD;
	    }
	    list.clearPieces(false);
	    //causing a crash: gs.pieces().takePieces( list );
	}
	node = s11n::find_child_by_name(*root, "graphicsitems");
	if( node )
	{
	    if(0) qDebug() << "qboard::pasteGraphicsItems() pasting items";
	    SerL list;
	    if( ! s11nlite::deserialize( *node, list ) )
	    {
		if(0) qDebug() << "qboard::pasteGraphicsItems() pasting deserialization failed!";
		return false;
	    }
	    for( SerL::iterator it = list.begin();
		 it != list.end(); ++it )
	    {
		QGraphicsItem * qgi = dynamic_cast<QGraphicsItem*>( *it );
		if( qgi )
		{
		    QPoint newpos( qgi->pos().toPoint() - pD );
		    qgi->setPos( newpos );
		    if(0) qDebug() << "qboard::pasteGraphicsItems() adding Serializable QGI to scene:"
			     << "newpos ="<<newpos
			     << qgi;
		    gs.scene()->addItem(qgi);
		    continue;
		}
		if(0) qDebug() << "qboard::pasteGraphicsItems() warning: skipping non-piece, non-QGI Serializable in input.";
		delete( *it );
	    }
	}
	return true;
    }

} // namespace
