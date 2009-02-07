
#include <QDebug>
#include <QGraphicsItem>
#include <QStringList>
#include <QMetaType>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>


#include <qboard/QGI.h>
#include <qboard/utility.h>
#include <qboard/QGIPiece.h>
#include <qboard/QGIDot.h>
#include <qboard/QGIHtml.h>

#include <ctime>
#include <cstdlib>

Q_DECLARE_METATYPE(QGIPiece*)
Q_DECLARE_METATYPE(QGIDot*)
Q_DECLARE_METATYPE(QGIHtml*)
Q_DECLARE_METATYPE(QGraphicsPixmapItem*)
Q_DECLARE_METATYPE(QGraphicsEllipseItem*)
Q_DECLARE_METATYPE(QGraphicsTextItem*)

void QGITypes::setupJsEngine( QScriptEngine * eng )
{
    QScriptValue vo;
//     vo = eng->defaultPrototype( QMetaType::type("QGraphicsPixmapItem*") );
//     if( vo.isObject() )
//     {
// 	eng->setDefaultPrototype( QMetaType::type("QGIPiece*"), vo );
//     }

#if 1
#define DO(T,B) \
    vo = eng->defaultPrototype( QMetaType::type(# B) ); \
    if( vo.isObject() ) eng->setDefaultPrototype( QMetaType::type(# T), vo );

    DO(QGIPiece*,QGraphicsPixmapItem*);
    DO(QGIDot*,QGraphicsEllipseItem*);
    DO(QGIHtml*,QGraphicsTextItem*);
#undef DO
#endif
}

bool QGITypes::handleClickRaise( QGraphicsItem * it,
				 QGraphicsSceneMouseEvent * ev )
{
    if( (ev->buttons() & Qt::LeftButton)
	|| (ev->buttons() & Qt::MidButton)
	)
    {
	bool high = (ev->buttons() & Qt::LeftButton);
	qreal zV = qboard::nextZLevel(it,high);
	if( zV != it->zValue() )
	{
	    it->setZValue( zV );
	}
	return true;
    }
    return false;
}
static int shuffleRand(int n)
{
    return std::rand() % n ;
}
void QGITypes::shuffleQGIList( QList<QGraphicsItem*> list, bool skipParentedItems )
{
    if( list.isEmpty() ) return;
    typedef QList<QGraphicsItem*> LI;
    static unsigned int seed = 0;
    if( 0 == seed )
    {
	std::srand(seed = std::time(0));
    }
    typedef std::vector<QGraphicsItem*> VT;
    typedef std::vector<QPointF> VPT;
    typedef std::vector<qreal> ZPT;
    const int lsz = list.size();
    VT vec(lsz, (QGraphicsItem*)0 );
    VPT pts(lsz, QPointF());
    ZPT zvals(lsz,0.0);
    unsigned int i = 0;
    for( LI::iterator it = list.begin();
	 list.end() != it; ++it )
    {
	if( skipParentedItems && (*it)->parentItem() ) continue;
	vec[i] = *it;
	pts[i] = (*it)->pos();
	zvals[i] = (*it)->zValue();
	++i;
    }
    if( ! i ) return;
    std::random_shuffle( &vec[0], &vec[i], shuffleRand );
    //std::random_shuffle( &pts[0], &pts[i], shuffleRand );
    std::random_shuffle( &zvals[0], &zvals[i], shuffleRand );
    for( unsigned int x = 0; x < i; ++x )
    {
	QGraphicsItem * gi = vec[x];
	gi->setPos( pts[x] );
	gi->setZValue( zvals[x] );
    }
    return;
}
