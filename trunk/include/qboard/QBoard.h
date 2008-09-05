#ifndef QBOARD_H
#define QBOARD_H
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

#include <QPixmap>
#include <QObject>
#include <QRectF>
#include <QSize>
#include <qboard/Serializable.h>

/**
	QBoard is the basic game board type for the QBoard app.
	It is used in conjunction with QBoardView to provide
	a visible playing area.
*/
class QBoard : public QObject, public Serializable
{
Q_OBJECT
public:
	QBoard();
	virtual ~QBoard();
	/** Serializes this object to. */
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
	/* Clients should not modify this pixmap object directly. This function
	is provided so that printing and QPainter routines can use it. */
	QPixmap & pixmap();
	QPixmap const & pixmap() const;
	QSize size() const;
	virtual bool event( QEvent * e );

public Q_SLOTS:
    /**
       Reimplemented to accept image files as an argument.
    */
    virtual bool s11nLoad( QString const & );
    virtual bool s11nSave( QString const & ) const;

    /**
       Tries to load the given pixmap, returning true on success,
       false on error.
    */
    bool loadPixmap( QString const & );
    /**
       Clears the state of the board.
    */
    void clear();
Q_SIGNALS:
	void loadedBoard();
private:
	QString m_file;
	QPixmap m_px;
};
// Register QBoard with s11n:
#define S11N_TYPE QBoard
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "QBoard"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
// The s11nmeisters out there will know why we add a second registration here:
#define S11N_TYPE QBoard
#define S11N_TYPE_NAME "QBoard"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // QBOARD_H
