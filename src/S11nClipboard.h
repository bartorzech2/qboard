#ifndef __S11NCLIPBOARD_H__
#define __S11NCLIPBOARD_H__
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

#include <QObject>
#include <s11n.net/s11n/s11nlite.hpp>
/**
   S11nClipboard provides clipboard features for any
   Serializable type.

   Bugs:

   - Each instance updates the QApplication::clipboard()
   independently, but does not look for "incoming" changes
   made by other instances.
*/
class S11nClipboard : public QObject
{
    Q_OBJECT
private:
    S11nClipboard();
    virtual ~S11nClipboard();
public:
    typedef s11nlite::node_type S11nNode;
    typedef s11nlite::node_traits S11nNodeTraits;
    static S11nClipboard & instance();

    /**
       Returns the clipboard contents, which may be 0.
       The returned object is owned by this object.
    */
    S11nNode * contents();

    /** 
	Transfers ownership of the clipboard contents
	(which may be 0) to the caller. Before the transfer,
	signalUpdated() is fired if the content is not 0.
    */
    S11nNode * take();


    /**
       Tries to deserialize the contents as the given type.  The
       caller owns the returned object.
    */
    template <typename SerializableType>
    SerializableType * deserialize()
    {
	return m_node
	    ? s11nlite::deserialize<SerializableType>( *m_node )
	    : 0;
    }
    /**
       Similar to slotCopy(), but takes a Serializable instead of
       an S11nNode. If serialization of ser fails then the clipboard
       is not updated.
    */
    template <typename SerializableType>
    bool serialize( SerializableType & ser )
    {
	S11nNode tmp;
	bool ret = s11nlite::serialize( tmp, ser );
	if( ret )
	{
	    if( ! m_node ) m_node = S11nNodeTraits::create("clipboard");
	    S11nNodeTraits::swap( *m_node, tmp );
	    this->slotUpdateQClipboard();
	}
	return ret;
    }
public Q_SLOTS:
   /* transfers ownership of 'take' to this object. */
   void slotCut( S11nNode * take );
    /* makes a copy of the given node. */
    void slotCopy( S11nNode const * copy );
    /* Clears the clipboard contents. */
    void slotClear();
    Q_SIGNALS:
    void signalUpdated();
private Q_SLOTS:
    void slotUpdateQClipboard();
private:
    S11nNode * m_node;
};

#endif // __S11NCLIPBOARD_H__
