#ifndef QBOARD_QGIPIECE_H_INCLUDED
#define QBOARD_QGIPIECE_H_INCLUDED
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

class GamePiece;
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVariant>
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneDragDropEvent;
#include "QGI.h"
#include "Serializable.h"
/**
   A QGIPiece represents a game piece as a pixmap- or colored square.

   The framework reserves a set of commonly-used property names
   and defines their expected types as follows:
	
   - color (QColor or QString #RGB)
   - colorAlpha (int (0-255), applies to 'color') (deprecated: set alpha of color instead)
   - pixmapFile (QString)
   - pixmapFiles (QStringList)
   - borderColor (QColor)
   - borderSize (int)
   - borderAlpha (int (0-255). applies to 'borderColor')
   - borderStyle (int from Qt::PenStyle or QString forms of those flag names)
   - angle (qreal)
   - pos (QPoint or QPointF)
   - size (QSize or QSizeF)
   - zLevel (qreal)
   - name (QString)
   
   Client code using their own properties are encouraged to
   use a common prefix (e.g. "myFoo" and "myBar").

*/
class QGIPiece : public QObject,
		 public QGraphicsPixmapItem,
		 public Serializable
{
Q_OBJECT
private:
    QGIPiece( QGIPiece const & ); // not implemented
    QGIPiece & operator=(QGIPiece const &); // not implemented
    void setup();
    /**
       Unfortunate kludge to ensure that scaling and rotation
       don't hose each other.
    */
    void refreshTransformation();
public:
    /**
       Creates an empty object.
     */
    QGIPiece();
    virtual ~QGIPiece();
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual QRectF boundingRect () const;
    virtual int type() const { return QGITypes::QGIPiece; }
    virtual bool event( QEvent * e );

    /**
       Serializes this object to dest.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

protected:
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    virtual void dragMoveEvent( QGraphicsSceneDragDropEvent * event );
private Q_SLOTS:
    /** Handles property changes. */
    void propertySet( char const * key, QVariant const & val );
private:	
    struct Impl;
    Impl * impl;
};

// Register QGIPiece with s11n:
#define S11N_TYPE QGIPiece
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QGIPiece"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIPiece
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#define S11N_TYPE_NAME "QGIPiece"
#include <s11n.net/s11n/reg_s11n_traits.hpp>

/**
   This is the default context menu handler for QGIPieces.
   It is only in the public interface to make moc happy.
*/
class QGIPieceMenuHandler : public QObject
{
Q_OBJECT
public:
	QGIPieceMenuHandler();
	virtual ~QGIPieceMenuHandler();

public Q_SLOTS:
        void showHelp();
	void doMenu( QGIPiece *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
    void copyList();
    void cutList();
    bool copyPiece();
    bool cutPiece();
    void addChild();
private:
    static void clipList( QGIPiece * src, bool copy );
    struct Impl;
    friend class Impl;
    Impl * impl;
};



#endif // QBOARD_QGIPIECE_H_INCLUDED
