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
#include <QGraphicsView>
#include <QCursor>
#include <QApplication>

#include <stdexcept>

#include <qboard/utility.h>
#include <qboard/Serializable.h>
#include <qboard/S11nQt/S11nClipboard.h>
#include <qboard/S11nQt.h>
#include <qboard/S11nQt/QList.h>
#include <qboard/GameState.h>

#include <qboard/S11nQt/QPointF.h>
#include <qboard/S11nQt/QPoint.h>

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

// #if defined(QBOARD_VERSION)
// #  if ! QBOARD_VERSION
// #  undef QBOARD_VERSION
// #  define QBOARD_VERSION "SVN [" __DATE__ " " __TIME__ "]"
// #  endif
// #endif

// #if !defined(QBOARD_VERSION)
// #  define QBOARD_VERSION "Unknown"
// #endif

namespace qboard {


    void ScopedChdir::chdir( QString const & newDir )
    {
	if( ! QDir::setCurrent( newDir ) )
	{
	    QString msg = QString("Could not change to dir [%1]!").
		arg(newDir);
	    throw std::runtime_error(msg.toAscii().constData());
	}

    }
    ScopedChdir::ScopedChdir( QString const & newDir )
	: old(QDir::current())
    {
	this->chdir(newDir);
    }
    ScopedChdir::ScopedChdir( QDir const & newDir )
	: old(QDir::current())
    {
	this->chdir( newDir.absolutePath() );
    }
    ScopedChdir::~ScopedChdir()
    {
	QDir::setCurrent( this->old.absolutePath() );
    }

    const QString versionString()
    {
	// Reminder: we do this .arg() bit so we can use a numeric or string QBOARD_VERSION
	// 	return QString("%1").arg(QBOARD_VERSION);
	return
#if defined(QBOARD_VERSION)
#  if ! QBOARD_VERSION
	    QString("SVN [" __DATE__ " " __TIME__ "]")
#  else
	    QString("%1").arg(QBOARD_VERSION)
#  endif
#else
	    QString("Unknown")
#endif
	    ;
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

    qreal nextZLevel( QGraphicsItem const * gi, bool high )
    {
	if( ! gi ) return 0.0;
	typedef QList<QGraphicsItem *> QL;
	QL li = gi->collidingItems(Qt::IntersectsItemBoundingRect);
	QL::iterator it = li.begin();
	if( it == li.end() ) return gi->zValue();
	qreal zmax = gi->zValue();
	const qreal myZ = zmax;
	const qreal stride = high ? 0.01 : -0.01;
	/**
	   FIXME: the Qt docs state that the items returned from the
	   various items lists are in zLevel order - highest to
	   lowest. We can use that to short-circuit the for-loop.
	*/
	for( ; it != li.end(); ++it )
	{
	    qreal tmp = (*it)->zValue();
	    if( high )
	    {
		if( zmax < tmp ) zmax = tmp;
	    }
	    else
	    {
		if( zmax > tmp ) zmax = tmp;
	    }
	    if( tmp == myZ )
	    { // we can't know if we're at the same level here, so we'll advance.
		zmax += stride;
	    }
	}
	if( myZ != zmax )
	{
	    zmax += stride;
	}
	return zmax;
    } // nextZLevel()
	
    void destroyToplevelItems( QList<QGraphicsItem *> & tops )
    {
	typedef QList<QGraphicsItem *> QL;
	QGraphicsScene * sc = 0;
	for( QL::iterator it = tops.begin(); tops.end() != it; ++it )
	{
	    if( (*it)->parentItem() )
	    {
		//qDebug() << "qboard::destroyToplevelItems(QList) skipping parented item.";
		continue;
	    }
	    sc = (*it)->scene();
	    if( sc )
	    {
		if(0) qDebug() << "qboard::destroyToplevelItems(QList) asking Scene to remove *it:"<<*it;
		sc->removeItem(*it);
	    }
#if 1 // i don't like this, but it avoids some crashes!
	    if( QObject * obj = dynamic_cast<QObject*>(*it) )
	    { // kludge to help avoid stepping on self during Destroy/Delete actions
		if(0) qDebug() << "qboard::destroyToplevelItems(QList) using obj->deleteLater():" <<obj;
		obj->deleteLater(); // huh? not destroying QGILineBinder???
	    }
	    else
	    {
		if(0) qDebug() << "qboard::destroyToplevelItems(QList) delete *it:" <<*it;
		delete *it;
	    }
#else
	    if(0) qDebug() << "qboard::destroyToplevelItems(QList) delete *it:"<<*it;
	    delete *it;
#endif

	}
	tops.clear();
    }
    void destroyToplevelItems( QGraphicsItem * gi, bool alsoSelected )
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
	destroyToplevelItems( tops );
    }

    void destroyQGIList( QList<QGraphicsItem *> const & li )
    {
	Q_FOREACH( QGraphicsItem * it, li )
	{
	    delete it;
	}
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

    void rotateCentered( QGraphicsItem * qgi, qreal angle )
    {
	if( ! qgi ) return;
	QRectF rec(qgi->boundingRect());
	qreal x = rec.width()/2 + rec.left();
	qreal y = rec.height()/2 + rec.top();
	qgi->setTransform(QTransform().translate(x, y).rotate(angle).translate(-x, -y));
    }

    bool clipboardScene( QGraphicsScene * gsc, bool copy, QPointF const & origin )
    {
	if( ! gsc ) return false;
	if(0) qDebug() <<"qboard::clipboardScene("<<gsc<<","<<copy<<","<<origin<<")";
	typedef QList<QGraphicsItem *> QGIL;
	QGIL toCut;
	S11nClipboard & cb( S11nClipboard::instance() );
	QGIL selected( gsc->selectedItems() );
	typedef QList<Serializable *> SerL;
	SerL seritems;
	for( QGIL::iterator it = selected.begin(); selected.end() != it; ++it )
	{
	    if( (*it)->parentItem() ) continue;
	    //if(0) qDebug() <<"qboard::clipboardScene() marking " << *it;
	    Serializable * ser = dynamic_cast<Serializable*>(*it);
	    if( ! ser )
	    {
		if(1) qDebug() << (copy?"copy":"cut")<<"handler cannot handle non-Serializables."
			       << "Skipping object "<<*it;
		continue;
	    }
	    if( !copy ) toCut.push_back(*it); // FIXME? only cut serializables?
	    //if(0) qDebug() <<"qboard::clipboardScene() marking for "<<(copy?"COPY":"CUT") << *it;
	    seritems.push_back(ser);
	}
	S11nNode * parent = S11nNodeTraits::create(GameState::KeyClipboard);
	try
	{
	    bool ret = true;
	    S11nNode & meta( s11n::create_child( *parent, "metadata") );
	    if( 1 ) //! origin.isNull() )
	    {
		ret = s11n::serialize_subnode( meta, "originPoint", origin );
	    }
	    if( ret && ! seritems.isEmpty() )
	    {
		ret = s11n::serialize_subnode( *parent, "graphicsitems", seritems );
	    }
	    if( ! ret )
	    {
		throw s11n::s11n_exception("qboard::clipboardScene(): serialization of one or more entries failed!");
	    }
	}
	catch(std::exception const & ex)
	{
	    delete parent;
	    qDebug() << "qboard::clipboardScene(): serialization threw:"<<ex.what();
	    return false;
	}
	seritems.clear();
	cb.slotCut(parent);
	parent = 0;
	if( copy )
	{
	    if(0) qDebug() << "qboard::clipboardScene() copied data.";
	}
	else
	{
	    if(0) qDebug() << "qboard::clipboardScene() cut data.";
	    qboard::destroyToplevelItems( toCut );
	}
	S11nClipboard::S11nNode * cont = cb.contents();
	if( cont )
	{
	    if(0)
	    {
		qDebug() <<"Clipboard contents:";
		s11nlite::save( *cont, std::cout );
	    }
	}
	return 0 != cont;
    }
    bool clipboardGraphicsItems( QGraphicsItem * gvi, bool copy )
    {
	if(0) qDebug() <<"qboard::clipboardGraphicsItems("<<gvi<<","<<copy<<")";
	if( ! gvi ) return false;
#if 0
	// reference point at top/left:
	return clipboardScene( gvi->scene(), copy, gvi->pos().toPoint() );
#else
	// reference point centered around gvi's bounds:
	return clipboardScene( gvi->scene(), copy, calculateCenter(gvi) );
#endif
    }

    QRectF calculateBounds( QGraphicsItem * qgi, bool skipParented )
    {
	if( ! qgi ) return QRectF();
	typedef QList<QGraphicsItem*> QGIL;
	QGIL li;
	if( qgi->isSelected() )
	{
	    li = qgi->scene()->selectedItems();
	}
	else
	{
	    return QRectF( qgi->pos(), qgi->boundingRect().size() );
	}
	QRectF un;
	for( QGIL::iterator it = li.begin();
	     li.end() != it; ++it )
	{
	    QGraphicsItem * gi = *it;
	    un = un.unite( QRectF( gi->pos(), gi->boundingRect().size() ) );
	    if( gi->parentItem() )
	    {
		if( skipParented ) continue;
		QRectF r(gi->boundingRect());
		QPointF p(gi->mapToParent(r.topLeft()));
		un = un.unite( QRectF( p, r.size() ) );
		continue;
	    }
	    else
	    {
		un = un.unite( QRectF( gi->pos(), gi->boundingRect().size() ) );
	    }
	}
	return un;
    }

    QPointF calculateCenter( QGraphicsItem * qgi, bool skipParented )
    {
	QRectF r( calculateBounds(qgi,skipParented) );
	//qDebug() << "qboard::calculateCenter("<<qgi<<"):"<<r;
	return QPointF( r.left() + (r.width() / 2),
			r.top() + (r.height() / 2) );
    }

    int copyProperties( QObject const * src, QObject * dest )
    {
	if( src == dest ) return 0;
	typedef QList<QByteArray> QL;
	QL ql( src->dynamicPropertyNames() );
	int count = 0;
	for( QL::const_iterator it( ql.begin() );
	     ql.end() != it; ++it, ++count )
	{
	    char const * key = it->constData();
	    if( key && (*key == '_') ) continue; // Qt reserves the "_q_" prefix, so we'll elaborate on that.
	    dest->setProperty( key, src->property(key) );
	}
	return count;
    }

    int clearProperties( QObject * obj )
    {
	if( ! obj ) return 0;
	typedef QList<QByteArray> QL;
	QL ql( obj->dynamicPropertyNames() );
	int count = 0;
	for( QL::const_iterator it( ql.begin() );
	     ql.end() != it; ++it, ++count )
	{
	    char const * key = it->constData();
	    if( key && (*key == '_') ) continue; // Qt reserves the "_q_" prefix, so we'll elaborate on that.
	    obj->setProperty( key, QVariant() );
	}
	return count;
    }

    QTransform rotateAndScale( QRectF const & bounds, qreal angle, qreal scaleX, qreal scaleY, bool center )
    {
	if( 0 == scaleX ) scaleX = 1.0;
	if( 0 == scaleY ) scaleY = scaleX;
	QTransform trans;
	qreal x = bounds.width()/2 + bounds.left();
	qreal y = bounds.height()/2 + bounds.top();
	if( center ) trans.translate(x,y);
	trans.rotate( angle );
	trans.scale( scaleX, scaleY );
	if( center ) trans.translate(-x, -y);
	return trans;
    }

    void rotateAndScale( QGraphicsItem * obj, qreal angle, qreal scaleX, qreal scaleY, bool center )
    {
	if( ! obj ) return;
	if( scaleY == 0.0 ) scaleY = scaleX;
	obj->setTransform( rotateAndScale( obj->boundingRect(), angle, scaleX, scaleY, center ) );
    }

    void rotateAndScale( QGraphicsView * obj, qreal angle, qreal scaleX, qreal scaleY )
    {
	if( ! obj ) return;
	if( scaleY == 0.0 ) scaleY = scaleX;
	obj->setTransform( rotateAndScale( obj->sceneRect(), angle, scaleX, scaleY, false ) );
    }


    QTransform & transformFlip( QTransform & trans,
				QSizeF const & bounds,
				bool horiz )
    {
	if( horiz )
	{
	    trans.translate( -bounds.width(), 0 ).scale(-1,1);
	}
	else
	{
	    trans.translate( 0, -bounds.height() ).scale(1,-1);
	}
	return trans;
    }

    QTransform & transformFlip( QTransform & trans,
				QRectF const & bounds,
				bool horiz )
    {
	return transformFlip( trans, bounds.size(), horiz );
    }

    void transformFlip( QGraphicsItem * gi, bool horiz )
    {
	QTransform t( gi->transform() );
	transformFlip( t, gi->boundingRect().size(), horiz );
	gi->setTransform( t );
    }

    void transformFlip( QGraphicsView * gi, bool horiz )
    {
	QTransform t( gi->transform() );
	transformFlip( t, gi->sceneRect().size(), horiz );
	gi->setTransform( t );
    }


    QList<QGraphicsItem*> childItems( QGraphicsItem const * gi )
    {
	return gi ?
#if QT_VERSION >= 0x040400
	    gi->childItems()
#else // Assume Qt 4.3...
	    gi->QGraphicsItem::children()
#endif
	    : QList<QGraphicsItem*>();

    }

    QPoint findViewMousePoint( QGraphicsView * view )
    {
	if( ! view ) return QPoint();
	QWidget * vp = view->viewport();
	QPoint glpos( QCursor::pos() );
	QWidget * wa = QApplication::widgetAt( glpos );
	if( wa != vp ) return QPoint();
	return view->mapToScene( view->mapFromGlobal(glpos) ).toPoint();
    }


} // namespace
