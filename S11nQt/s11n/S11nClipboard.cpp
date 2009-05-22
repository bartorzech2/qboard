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

#include <s11n.net/s11n/qt/S11nClipboard.h>
#include <sstream>
#include <QClipboard>
#include <QApplication>

#ifndef Q_EMIT
#  define Q_EMIT
#endif

S11nClipboard::S11nClipboard() : m_node(0)
{
    this->syncFromQt();
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(syncFromQt()));
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
    disconnect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(syncFromQt()));
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
    Q_EMIT signalUpdated();
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(syncFromQt()));
}

void S11nClipboard::syncFromQt()
{
    QString data( QApplication::clipboard()->text( QClipboard::Clipboard ) );
    delete m_node;
    m_node = 0;
    if( data.isEmpty() )
    {
	Q_EMIT signalUpdated();
	return;
    }
    S11nNode * node = 0;
    {
	char const * ascii = data.toAscii().data();
	std::istringstream buf(ascii ? ascii : "");
	node = s11nlite::load_node( buf );
    }
    m_node = node;
    Q_EMIT signalUpdated();
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


QString S11nClipboard::contentLabel()
{
    if( m_node )
    {
	return QString("%1[%2]")
	    .arg( S11nNodeTraits::name(*m_node).c_str() )
	    .arg( S11nNodeTraits::class_name(*m_node).c_str() );
    }
    else
    {
	return QString("[empty]");
    }
}
