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
S11nClipboard::S11nNode * S11nClipboard::take()
{
    S11nNode * x = m_node;
    m_node = 0;
    if( x )
    {
	emit signalUpdated();
    }
    return x;
}

void S11nClipboard::slotCut( S11nNode * take )
{
	if( (! take) || (take == this->m_node) ) return;
	delete this->m_node;
	this->m_node = take;
	emit signalUpdated();
}
void S11nClipboard::slotCopy( S11nNode const * cp )
{
	if( (! cp) || (cp==this->m_node) ) return;
	delete this->m_node;
	this->m_node = new S11nNode( *cp );
	emit signalUpdated();
}
void S11nClipboard::slotClear()
{
  delete this->m_node;
  this->m_node = 0;
  emit signalUpdated();
}
