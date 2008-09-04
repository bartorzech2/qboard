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

#include "WikiLiteView.h"
#include "WikiLiteParser.h"
#include <QTextEdit>
#include <QTextDocument>
#include <QGridLayout>
#include <QScrollBar>

namespace qboard {

    struct WikiLiteView::Impl
    {
	WikiLiteParser * p;
	QTextEdit * edit;
	Impl() : p(new WikiLiteParser),
		 edit(0)
	{
	}
	~Impl()
	{
	    delete p;
	}
    };

    WikiLiteView::WikiLiteView( QWidget * parent ) : QWidget(parent),
						     impl(new Impl)
    {
	impl->edit = new QTextEdit;
	QGridLayout * lay = new QGridLayout( this );
	lay->addWidget( impl->edit );
	impl->edit->setReadOnly(true);
    }

    WikiLiteView::~WikiLiteView()
    {
	delete impl;
    }

    void WikiLiteView::parseFile( QString const & fn )
    {
	QString code( impl->p->parseFile(fn) );
	impl->edit->setHtml( code );
	this->updateScrollbars();
    }

    void WikiLiteView::updateScrollbars()
    {
	// there no fucking way to scroll to the top of the doc???
#if 0
	// does nothing?
	impl->edit->horizontalScrollBar()->setValue(0);
	impl->edit->verticalScrollBar()->setValue(0);
#else
	impl->edit->scroll(0,0);
#endif
	this->update();
    }
} // namespace
