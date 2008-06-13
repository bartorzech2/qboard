#include "S11nClipboard.h"
S11nClipboard::S11nClipboard() : m_node(0)
{
}
S11nClipboard::~S11nClipboard()
{
	delete this->m_node;	
}
S11nClipboard & S11nClipboard::instance()
{
	static S11nClipboard bob;
	return bob;
}
S11nClipboard::S11nNode * S11nClipboard::contents()
{
	return this->m_node;
}

void S11nClipboard::slotCut( S11nNode * take )
{
	if( (! take) || (take == this->m_node) ) return;
	delete this->m_node;
	this->m_node = take;
	emit signalCut();
}
void S11nClipboard::slotCopy( S11nNode const * cp )
{
	if( (! cp) || (cp==this->m_node) ) return;
	delete this->m_node;
	this->m_node = new S11nNode( *cp );
	emit signalCopy();
}
void S11nClipboard::slotClear()
{
  delete this->m_node;
  this->m_node = 0;
  emit signalClear();
}
