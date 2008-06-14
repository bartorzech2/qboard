#ifndef __QBOARDVIEW_H__
#define __QBOARDVIEW_H__
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


#include <QGraphicsScene>
#include <QGraphicsView>
class QBoard;
class QWheelEvent;
class QMouseEvent;
class QPainter;
class QDragMoveEvent;
class QContextMenuEvent;

class QBoardView : public QGraphicsView
{
Q_OBJECT
public:
	QBoardView( QBoard & b, QGraphicsScene * parent = 0 );
	virtual ~QBoardView();
	virtual void wheelEvent(QWheelEvent *event);
	virtual QSize sizeHint () const;
    /**
       This object's board. Ownership is not changed.
    */
    QBoard & board();
public Q_SLOTS:
	void updateBoardPixmap();
	void zoomOut();
	void zoomIn();
	void zoom( qreal );
	void zoomReset();
	/* If handMode is true then grabbing-hand-style drag is used,
	otherwise rubber-band-style is used. */
	void setHandDragMode(bool handMode);
    void selectAll();
protected:
	virtual void drawBackground( QPainter *, const QRectF & );
	virtual void mousePressEvent ( QMouseEvent * event );
    virtual void dragMoveEvent( QDragMoveEvent * event );
    virtual void contextMenuEvent( QContextMenuEvent * event );

private:
	QBoard & m_b;
	qreal m_scale;
};
#endif // __QBOARDVIEW_H__
