#ifndef __QBOARDVIEW_H__
#define __QBOARDVIEW_H__
#include "QBoard.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include "QBoard.h"
class QBoardView : public QGraphicsView
{
Q_OBJECT
public:
	QBoardView( QBoard & b, QGraphicsScene * parent = 0 );
	virtual ~QBoardView();
	virtual void wheelEvent(QWheelEvent *event);
	void persistentZoom( qreal );
	virtual QSize sizeHint () const;
public Q_SLOTS:
	void updateBoardPixmap();
	void zoomOut();
	void zoomIn();
	void zoom( qreal );
	void zoomReset();
	/* If handMode is true then grabbing-hand-style drag is used,
	otherwise rubber-band-style is used. */
	void setHandDragMode(bool handMode);
protected:
	virtual void drawBackground( QPainter *, const QRectF & );
	virtual void mousePressEvent ( QMouseEvent * event );
private:
	QBoard & m_b;
	qreal m_scale;
	qreal m_pzoom;
};
#endif // __QBOARDVIEW_H__
