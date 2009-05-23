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
#include <QDebug>

namespace s11n { namespace qt {
/**
   S11nClipboard provides clipboard features for any Serializable
   type. It is intended to be used as a Singleton - fetch its instance
   via the static instance() function.
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
    /**
       Returns the shared instance of S11nClipboard.
    */
    static S11nClipboard & instance();

    /**
       Returns the clipboard contents, which may be 0.
       The returned object is owned by this object.

       Any change to the clipboard may invalidate this pointer, so do
       not keep a reference to it.
    */
    S11nNode * contents();

    /** 
	Transfers ownership of the clipboard contents (which may be 0)
	to the caller. If the content is non-zero then the Qt
	clipboard is also cleared and signalUpdated() is fired.
    */
    S11nNode * take();

    /**
       Tries to deserialize the current clipboard contents as the
       given type. The caller owns the returned object, which may be
       0.
    */
    template <typename SerializableType>
    SerializableType * deserialize()
    {
	try
	{
	    return m_node
		? s11nlite::deserialize<SerializableType>( *m_node )
		: 0;
	}
	catch(...)
	{
	    return 0;
	}
    }
    /**
       Similar to slotCopy(), but takes a Serializable instead of
       an S11nNode. If serialization of ser fails then the clipboard
       is not updated.

       If this call succeeds, the current content of the clipboard
       will be a serialized copy of ser.
    */
    template <typename SerializableType>
    bool serialize( SerializableType & ser )
    {
	bool ret = false;
	try
	{
	    S11nNode tmp;
	    S11nNodeTraits::name( tmp, "S11nClipboardData" );
	    ret = s11nlite::serialize( tmp, ser );
	    if( ret )
	    {
		if( ! m_node ) m_node = S11nNodeTraits::create("clipboard");
		S11nNodeTraits::swap( *m_node, tmp );
		this->syncToQt();
	    }
	    else
	    {
		qDebug() << "S11nClipboard::serialize() failed! Serializable class name is"
			 << s11n::s11n_traits<SerializableType>::class_name(&ser).c_str();
	    }
	}
	catch(...)
	{
	    ret = false;
	}
	return ret;
    }

    /**
       If the content is set, it returns a string form of it's s11n
       info, suitable for use as a quasi-informative label for the
       contents.
    */
    QString contentLabel();

public Q_SLOTS:
   /* Transfers ownership of 'take' to this object and sets it as the
      current clipboard content. */
   void slotCut( S11nNode * take );
    /* Sets the current clipboard content to a copy of the given
       node. If passed 0 then the clipboard is cleared.
    */
    void slotCopy( S11nNode const * copy );

    /* Clears the clipboard contents. */
    void slotClear();
Q_SIGNALS:
    /** Tells listeners that this object's clipboard state has
	(likely) been updated. Use content() to get the clipboard data.
    */
    void signalUpdated();
private Q_SLOTS:
    /** Copies the local s11n clipboard data to the Qt clipboard as text and emits signalUpdated(). */
    void syncToQt();
    /** Reads the text content from the Qt clipboard and tries to
    deserialize it, replacing the current data. If the source cannot
    be deserialized then this clipboard is cleared. */
    void syncFromQt();
private:
    S11nNode * m_node;
};

}} // namespaces

#endif // __S11NCLIPBOARD_H__
