#ifndef __QBOARDVIEW_H__
#define __QBOARDVIEW_H__
#include "QBoard.h"

#include <QGraphicsScene>
#include <QGraphicsView>
class QWheelEvent;
class QMouseEvent;
class QPainter;
class QDragMoveEvent;
class QContextMenuEvent;
#include "QBoard.h"

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
