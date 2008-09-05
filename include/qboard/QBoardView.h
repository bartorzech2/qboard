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


#include <QGraphicsView>
class QGraphicsScene;
class QBoard;
class QWheelEvent;
class QMouseEvent;
class QPainter;
class QDragMoveEvent;
class QContextMenuEvent;
class GameState;
class GamePiece;
class QBoardView : public QGraphicsView
{
Q_OBJECT
public:
	QBoardView( GameState & st );
	virtual ~QBoardView();
	virtual void wheelEvent(QWheelEvent *event);
	virtual QSize sizeHint () const;
    /**
       This object's board. Ownership is not changed.
    */
    QBoard & board();
    GameState & state();
    /**
       Returns true if this object is in OpenGL mode.
    */
    bool isGLMode() const;

public Q_SLOTS:
	void zoomOut();
	void zoomIn();
	void zoom( qreal );
	void zoomReset();
	/* If handMode is true then grabbing-hand-style drag is used,
	otherwise rubber-band-style is used. */
	void setHandDragMode(bool handMode);

    void clipCopySelected();
    void clipCutSelected();
    void clipPaste();

    void selectAll();
    /** Sets or unsets OpenGL mode, which is faster for many operations
	but often misses important screen updates.
	If QBOARD_USE_OPENGL is false then this function does
	nothing.
    */
    void setGLMode(bool);
private Q_SLOTS:
	void updateBoardPixmap();

protected:
	virtual void drawBackground( QPainter *, const QRectF & );
	virtual void mousePressEvent ( QMouseEvent * event );
	virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void dragMoveEvent( QDragMoveEvent * event );
    virtual void contextMenuEvent( QContextMenuEvent * event );
    virtual bool event( QEvent * e );
    virtual void mouseDoubleClickEvent( QMouseEvent * e );

private:
    /** Handles property changes. */
    void propertySet( char const * key, QVariant const & val );
    void refreshTransformation();
    struct Impl;
    Impl * impl;
};
#endif // __QBOARDVIEW_H__
