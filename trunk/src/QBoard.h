#ifndef QBOARD_H
#define QBOARD_H
//
#include <QPixmap>
#include <QObject>
#include <QRectF>
#include <QSize>
#include "Serializable.h"

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
	virtual bool load( QString const & );
	virtual bool save( QString const & ) const;
    /**
       Clears the state of the board.
    */
    void clear();
Q_SIGNALS:
	void loaded();
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
