#ifndef GAMEPIECE_H
#define GAMEPIECE_H
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

#include <QRect>
#include <QObject>
#include <QVariant>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QString>

#include "Serializable.h"

/**
	This is the base type for QBoard game pieces. It represents
	an abstract playing piece. To provide for some flexibility,
	it does not hard-code a set of properties, but instead
	relies on QObject's property support. See setPieceProperty()
	for information on that. Its UI is provided by an external
	QGraphicsItem subclass, e.g QGIGamePiece.
	
	The framework reserves a set of commonly-used property names
	and defines their expected types as follows:
	
	- color (QColor or QString #RGB)
	- colorAlpha (int (0-255), applies to 'color')
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
	- geometry (QRect, read-only)

	Client code using their own properties are encouraged to
	use a common prefix (e.g. "myFoo" and "myBar").
*/
class GamePiece : public QObject, public Serializable
{
Q_OBJECT
public:
Q_PROPERTY(QRect geometry READ geom STORED false)
#define PIECE_PROP_DECL(T,PN,FN) Q_PROPERTY(T PN READ get ## FN WRITE set ## FN SCRIPTABLE true)
#define PIECE_PROP_SET(T,PN,FN) public Q_SLOTS: void set ## FN( T const v ) { setPieceProperty(# PN, QVariant(v));};
#define PIECE_PROP_GET(T,PN) public: T PN() const { return this->property(# PN).value<T>();};
#define PIECE_PROP(T,PN,FN) PIECE_PROP_DECL(T,PN,FN); PIECE_PROP_SET(T,PN,FN); PIECE_PROP_GET(T,PN);
public:
PIECE_PROP(QColor,color,Color)
PIECE_PROP(QString,pixmapFile,PixmapFile)
//PIECE_PROP(QStringList,pixmapFiles,PixmapFiles)
PIECE_PROP(QColor,borderColor,BorderColor)
PIECE_PROP(int,borderSize,BorderSize)
PIECE_PROP(int,borderStyle,BorderStyle)
PIECE_PROP(qreal,angle,Angle)
PIECE_PROP(QPoint,pos,Pos)
PIECE_PROP(QSize,size,Size)
PIECE_PROP(qreal,zLevel,ZLevel)
PIECE_PROP(QString,name,Name)
PIECE_PROP(QString,colorAlpha,ColorAlpha)

#undef PIECE_PROP_DECL
#undef PIECE_PROP_SET
#undef PIECE_PROP_GET
#undef PIECE_PROP
private:
	GamePiece( GamePiece const & ); // not yet implemented!
	GamePiece & operator=( GamePiece const & ); // not yet implemented.
public:
	/** Constructs an empty piece. */
	GamePiece();
	virtual ~GamePiece();
	virtual bool event( QEvent * e );
public Q_SLOTS:
	/**
		This is like QObject::setProperty(), but with two
		differences:

		- If the given QVariant is not a type supported by our
		serialization code (see the QVariant_s11n class) then this function
		returns false and refuses to set the property. Note, however,
		that QObject::setProperty() also returns false if it sets
		a "dynamic property", so false does not necessarily mean
		failure.

		- A piecePropertySet() signal is fired on success.

		When a GamePiece is serialized, all properties set via
		this routine or setProperty() will be serialized. An invalid
		property type (i.e. one which isn't serializable) would cause
		serialization to fail, which is why this function doesn't
		accept them. Note, however, that setProperty() accepts any
		QVariant type.

		Property names MUST be usable as serialization keys, which
		essentialy means that they must be only alphanumeric/underscore,
		starting with non-digit. Dashes, colons, and such may not be
		supported by any given libs11n file format, so their use is
		highly discouraged.
	*/
	bool setPieceProperty( char const * name, QVariant const & );

	/** Serializes this object to dest. */
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
	
	/**
		Returns this object's geometry, as determined by the
		"pos" and "size" properties.
	*/
	QRect geom() const;
	
	/** Returns true if this object has the given dynamic property, else false. */
	bool hasProperty( char const * );
	/**
		To be used by QGIPieceView (or similar) when they connect to this piece.
		It increments the "view count" by 1.
	*/
	void addViewRef();
	/**
		To be used by QGIPieceView (or similar) when they disconnect from this piece.
		It decrements the "view count" by 1. If the view count goes to 0, this object
		scedules itself for destruction using deleteLate().
		
		The reason behind this behaviour is: action-handling code was having to do too
		many QGIPieceView-specific workarounds due to the different deletion mechanisms
		between QGIPieceView and other QGraphicsItems types. So this class got the
		add/removeViewref() funcs so that piece viewers could play nicely with each other
		and with the default destruction mechanism (which is simply to call delete on
		an object).
	*/
	void removeViewRef();
Q_SIGNALS:
	/**
		Signal fired when properties are changed via setPieceProperty().
	*/
	void piecePropertySet( char const *, GamePiece * pc );
	/**
		Signal emited when this object destructs.
	*/
	void destructing( GamePiece * gp );
private:
	struct Impl;
	Impl * impl;
};

// Register GamePiece with s11n:
#define S11N_TYPE GamePiece
#define S11N_TYPE_NAME "GamePiece"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
// The s11nmeisters out there will know why we add a second registration here:
#define S11N_TYPE GamePiece
#define S11N_TYPE_NAME "GamePiece"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#include <QSet>
/**
GamePieceList implements a list of GamePieces.
All GamePiece items in this type are owned by it.
*/
class GamePieceList : public QObject, public Serializable
{
Q_OBJECT
private:
	GamePieceList( GamePieceList const & ); // not implemented
	GamePieceList & operator=(GamePieceList const &); // not implemented
public:
	GamePieceList();
	virtual ~GamePieceList();
	typedef QSet<GamePiece*> ListType;
	typedef ListType::iterator iterator;
	typedef ListType::const_iterator const_iterator;
	iterator begin() { return this->m_list.begin(); }
	const_iterator begin() const { return this->m_list.begin(); }
	iterator end() { return this->m_list.end(); }
	const_iterator end() const { return this->m_list.end(); }
	/** Serializes this object to dest. */
	virtual bool serialize( S11nNode & dest ) const;
	/** Deserializes src to this object. */
	virtual bool deserialize( S11nNode const & src );
	bool empty() const {return m_list.empty();}
	size_t size() const {return m_list.size();}
	/* Moves all pieces from other into this list. */
	void takePieces( GamePieceList &other);
Q_SIGNALS:
	void pieceAdded( GamePiece * );
	void pieceRemoved( GamePiece * );
public Q_SLOTS:
	/* Transfers ownership of gp to this object and emits pieceAdded(gp) */
	void addPiece( GamePiece * gp );

	/* Removes gp from our internal list and emits pieceRemoved(gp).

	Transfers ownership of gp to the caller.

	If this object does not contain gp then no signal is fired
	and false is returned.
	*/
	bool removePiece( GamePiece * gp );
	/** Deletes all pieces in the list. */
	void clearPieces();
	/**
		Clears the list without deleting the contained items. Ownership
		is effectively undefined - make sure you get the items you need
		before you do this.
	*/
	void clearNoDelete();
private Q_SLOTS:
	/** Connects gp to this	object's internal handlers. */ 
	void connect( GamePiece *gp);
	void disconnect( GamePiece *gp);
private:
	ListType m_list;
};
// Register GamePieceList with s11n:
#define S11N_TYPE GamePieceList
#define S11N_TYPE_NAME "GamePieceList"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE GamePieceList
#define S11N_TYPE_NAME "GamePieceList"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // GAMEPIECE_H
