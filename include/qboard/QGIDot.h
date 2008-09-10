#ifndef QBOARD_QGI_DOT_H_INCLUDED
#define QBOARD_QGI_DOT_H_INCLUDED 1

#include "Serializable.h"
#include "QGI.h"

#include <QGraphicsItem>
#include <QVariant>
#include <QPointF>
#include <QPair>
#include <QList>
#include <QPainterPath>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
class QMenu;
class QGIDotLine;
/**
   QGIDot represent a simple dot with configurable colors.
*/
class QGIDot : public QObject,
	       public QGraphicsEllipseItem,
	       public Serializable
{
Q_OBJECT
public:
QGIDot();
    virtual ~QGIDot();
    /** Serializes this object to dest. The following properties are saved:
	position, text (as HTML), QObject "dynamic" properties.
    */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );
    virtual int type() const { return QGITypes::QGIDot; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool event( QEvent * e );
    virtual Serializable * clone() const;
Q_SIGNALS:
    void dotDestructing( QGIDot * );
    void doubleClicked( QGraphicsItem * );
public Q_SLOTS:
   /**
      See QGIHider::hideItems(). Returns object is this item's new
      parent (which may technically be 0 but is "likely" to be a
      QGIHider).
   */
   QGraphicsItem * hideItems();

    /**
       "Splits" this dot into two dots and an adjoining line.
       The line becomes a child of this object and the new dot
       becomes a child of the line.
    */
    void split();
protected:
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private Q_SLOTS:
    /** Handles property changes. */
    void propertySet( char const * key, QVariant const & val );
    //void lineDestroyed( QGIDotLine * );
    void addEdge( QGIDotLine * );
    void removeEdge( QGIDotLine * );
    void showHelp();
    void cloneWithLine();

private:
    friend class QGIDotLine;
    typedef QList<QGIDotLine*> EdgeList;
    EdgeList edges() const;
    void refreshTransformation();

    void updatePainter();
    struct Impl;
    Impl * impl;
};

// Register QGIDot with s11n:
#define S11N_TYPE QGIDot
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QGIDot"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIDot
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#define S11N_TYPE_NAME "QGIDot"
#include <s11n.net/s11n/reg_s11n_traits.hpp>

/**
   QGIDotLine represents a line between two QGIDots. Do not
   use this class from client code. It has an embarassingly
   intimate relationship with QGIDot, and is intended to
   be used via that class' interface.
*/
class QGIDotLine : public QObject,
		   public QGraphicsLineItem,
		   public Serializable
{
Q_OBJECT
public:
    QGIDotLine();
    virtual ~QGIDotLine();

    virtual int type() const { return QGITypes::QGIDotLine; }

    /**
       Serializes this object to dest.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
protected:
    virtual bool event( QEvent * e );
    QMenu * createLineMenu();
Q_SIGNALS:
    void lineDestructing( QGIDotLine * );
private Q_SLOTS:
    void destDestroyed( QGIDot * );
    void propertySet( char const *pname, QVariant const & var );
    void setNodes( QGIDot * src, QGIDot * dest );
    void setSourceNode( QGIDot * d );
    void setDestNode( QGIDot * d );
    QGIDot * srcNode();
    QGIDot * destNode();
    void adjust();
private:
    friend class QGIDot;
    struct Impl;
    Impl * impl;
};

// Register QGIDotLine with s11n:
#define S11N_TYPE QGIDotLine
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QGIDotLine"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIDotLine
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#define S11N_TYPE_NAME "QGIDotLine"
#include <s11n.net/s11n/reg_s11n_traits.hpp>


class MenuHandlerDot : public QObject
{
Q_OBJECT
public:
	MenuHandlerDot();
	virtual ~MenuHandlerDot();
public Q_SLOTS:
	void doMenu( QGIDot *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
    /*
     */
    void showHelp();
private:
	struct Impl;
	Impl * impl;
};


#endif // QBOARD_QGI_DOT_H_INCLUDED
