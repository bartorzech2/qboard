#ifndef __S11NCLIPBOARD_H__
#define __S11NCLIPBOARD_H__
#include <QObject>
#include <s11n.net/s11n/s11nlite.hpp>
class S11nClipboard : public QObject
{
Q_OBJECT
public:
	typedef s11nlite::node_type S11nNode;
		typedef s11nlite::node_traits S11nNodeTraits;
	S11nClipboard();
	virtual ~S11nClipboard();
	static S11nClipboard & instance();
	S11nNode * contents();
	template <typename SerializableType>
	SerializableType * deserialize()
	{
		return m_node
			? s11nlite::deserialize<SerializableType>( *m_node )
			: 0;
	}
	template <typename SerializableType>
	bool serialize( SerializableType & ser )
	{
		if( m_node ) S11nNodeTraits::clear(*m_node);
		else m_node = S11nNodeTraits::create("clipboard");
		return s11nlite::serialize( *m_node, ser );
	}
public Q_SLOTS:
	/* transfers ownership of 'take' to this object. */
	void slotCut( S11nNode * take );
	/* makes a copy of the given node. */
	void slotCopy( S11nNode const * copy );
	/* Clears the clipboard contents. */
	void slotClear();
Q_SIGNALS:
	void signalCopy();
	void signalCut();
	void signalClear();
private:
	S11nNode * m_node;
};

#endif // __S11NCLIPBOARD_H__
