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

/**
   QGIHider is a special type of QGraphicsItem which "hides"
   other QGIs. When it does so, it takes on the "hidden"
   item as a child and replaces the child with this item
   in the parent's coordinates. This item takes on the overall
   shape of the covered item.

   Intended usage:

   \code
   QGIHider * h = new QGIHider;
   h->hideItem( myQGI );
   ...
   h->unhideItem();
   delete h;
   \code

   Alternately, unhideItems() (plural) will not only
   unhide the item (and all selected items, if the item
   is selected), but will also:

   - unhide all selected QGIHider items IFF this object
   is selected.
   - destroy this object (and all selected QGIHider objects
   IFF this object is selected).
*/
class QGIHider : public QObject,
		 public QGraphicsPathItem,
		 public Serializable
{
Q_OBJECT
public:
    QGIHider();
    virtual ~QGIHider();

    /**
       Serializes this object to dest. The currently hidden
       object gets serialized as well.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

    virtual int type() const { return QGITypes::QGIHider; }

    /**
       Reimplemented to return the rect for the hidden piece.
    */
    virtual QRectF boundingRect() const;
    /**
       Reimplemented to return the shape of the hidden piece.
    */
    virtual QPainterPath shape() const;
    /**

    */
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

    /**
       Hides toHide, as per createHider(). If toHide->isSelected()
       then all selected items which are not themselves of type
       QGIHider are also hidden.
    */
    static void hideItems( QGraphicsItem * toHide );
    virtual QPainterPath opaqueArea() const;
public Q_SLOTS:

    /**
       Transfers ownership of toHide to this object, which will remove
       it from the scene and hide it from view. In toHide's place,
       this object will move itself to toHide's parent item (or the
       scene). Calling unhideItem() will reverse the process.

       As a special case, if toHide is-a QObject and has a property
       named 'color', this object will set its color to that color.
    */
    void hideItem( QGraphicsItem * toHide );
    /**
       Transfers ownership of the hidden object (if any) as follows:

       If this object is part of a QGraphicsScene or a parent object
       then the item is made visible in that scene/parent at this
       object's current position.  If there is no scene, the caller
       owns the returned object.

       This routine will leave this object invisible, and thus not
       reachable via a GUI. This object should normally be deleted
       after calling unhideItem(). Alternately, use unhideItems(),
       which destroys this object after unhiding the hidden item.
    */
    QGraphicsItem * unhideItem();

    /**
       If this object is selected, this unhides all selected items,
       otherwise it just unhides this object.

       This object (or all selected QGIHiders, if this object is
       selected) is/are destroyed via this->deleteLater() by this
       routine!
    */
    void unhideItems();

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    /**
       Reimplemented to handle raise-on-click.
    */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *ev);

private:
    /**
       Calls h->unhideItem() and calls h->deleteLater().
    */
    static void unhideItem( QGIHider * h );
    /**
       Unhides h. If h is selected then all selected QGIHiders
       are processed.

       At the end of this routine, h is deleted.
    */
    static void unhideItems( QGIHider * h );
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
