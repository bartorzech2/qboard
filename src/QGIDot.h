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
public Q_SLOTS:
   /**
      See QGIHider::hideItems(). Returns object is this item's new
      parent (which may technically be 0 but is "likely" to be a
      QGIHider).
   */
    QGraphicsItem * hideItems();

protected:
	//virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	//virtual void focusOutEvent( QFocusEvent * event );
	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private Q_SLOTS:
    /** Handles property changes. */
    void propertySet( char const * key, QVariant const & val );

private:
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
