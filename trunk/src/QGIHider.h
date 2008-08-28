#ifndef QBOARD_QGIHider_H_INCLUDED
#define QBOARD_QGIHider_H_INCLUDED 1

#include <QObject>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QRectF>

class QStyleOptionGraphicsItem;
class QGraphicsSceneContextMenuEvent;
class QWidget;
class QEvent;

#include "Serializable.h"
#include "QGI.h"

class QGIHider : public QObject,
		 public QGraphicsPathItem,
		 public Serializable
{
Q_OBJECT
public:
    QGIHider();
    virtual ~QGIHider();

    /**
       Serializes this object to dest.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

    virtual int type() const { return QGITypes::QGIHider; }
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool event( QEvent * e );

    /**
       Creates a new hider for the given item. Ownership of qgi is
       passed on to the returned item. Ownership of the returned
       item is as follows:

       If qgi had (before calling this routine) either a parent QGI
       or a QGraphicsScene (i.e. is a top-level item), then that
       parent or scene now owns the returned object. If qgi has
       neither parent nor scene, then the caller owns the returned
       object. In any case, the returned object will have the same
       scene and parent as qgi had before calling this function.
    */
    static QGIHider * createHider( QGraphicsItem * qgi );

public Q_SLOTS:

    /**
       Transfers ownership of toHide to this object, which will remove
       it from the scene and hide it from view. In toHide's place,
       this object will move itself to toHide's parent item (or the
       scene). Calling unhideItem() will reverse the process.
    */
    void hideItem( QGraphicsItem * toHide );

    /**
       Transfers ownership of the hidden object (if any) as follows:

       If this object is part of a QGraphicsScene or a parent object
       then the item is made visible in that scene/parent at this
       object's current position.  If there is no scene, the caller
       owns the returned object.

       This routine will leave this object invisible. This object
       should normally be deleted after calling this.
    */
    QGraphicsItem * unhideItem();

    /**
       Identical to unhideItem(), but calls this->deleteLater()
       to free this object.
    */
    QGraphicsItem * replaceItem();

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *ev);
private:
    struct Impl;
    Impl * impl;
    void setup();
};

// Register QGIHider with s11n:
#define S11N_TYPE QGIHider
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QGIHider"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIHider
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#define S11N_TYPE_NAME "QGIHider"
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // QBOARD_QGIHider_H_INCLUDED
