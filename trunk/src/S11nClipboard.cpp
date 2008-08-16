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
#include <sstream>
#include <QClipboard>
#include <QApplication>
S11nClipboard::S11nClipboard() : m_node(0)
{
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),
	    this,SLOT(syncFromQt()));
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

void S11nClipboard::syncToQt()
{
    QClipboard * cb = QApplication::clipboard();
    // cb->clear(); doh! Ends up nuking m_node via signal to syncFromQt()!
    if( this->m_node )
    {
	std::ostringstream os;
	if( s11nlite::save( *m_node, os ) )
	{
	    cb->setText( os.str().c_str() );
	}
    }
    else
    {
	cb->clear();
    }
    emit signalUpdated();
}

void S11nClipboard::syncFromQt()
{
    QString data( QApplication::clipboard()->text( QClipboard::Clipboard ) );
    delete m_node;
    m_node = 0;
    if( data.isEmpty() )
    {
	emit signalUpdated();
	return;
    }
    S11nNode * node = 0;
    {
	char const * ascii = data.toAscii().data();
	std::istringstream buf(ascii ? ascii : "");
	node = s11nlite::load_node( buf );
    }
    m_node = node;
    emit signalUpdated();
}

S11nClipboard::S11nNode * S11nClipboard::take()
{
    S11nNode * x = m_node;
    m_node = 0;
    if( x )
    {
	this->syncToQt();
    }
    return x;
}

void S11nClipboard::slotCut( S11nNode * take )
{
	if( (take == this->m_node) ) return;
	delete this->m_node;
	this->m_node = take;
	this->syncToQt();
}
void S11nClipboard::slotCopy( S11nNode const * cp )
{
	if( (cp==this->m_node) ) return;
	delete this->m_node;
	this->m_node = cp ? new S11nNode( *cp ) : 0;
	this->syncToQt();
}
void S11nClipboard::slotClear()
{
    delete this->m_node;
    this->m_node = 0;
    this->syncToQt();
}
