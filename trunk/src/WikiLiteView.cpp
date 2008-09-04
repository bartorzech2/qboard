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
#include <QLabel>
#include <QFileInfo>

namespace qboard {

    struct WikiLiteView::Impl
    {
	WikiLiteParser * p;
	QTextEdit * edit;
	QLabel * label;
	Impl() : p(new WikiLiteParser),
		 edit(0)
	{
	}
	~Impl()
	{
	    delete p;
	}
    };

    WikiLiteView::WikiLiteView( QWidget * parent ) : QFrame(parent),
						     impl(new Impl)
    {
	this->setFrameStyle( QFrame::Panel | QFrame::Sunken );


	impl->label = new QLabel("Wiki viewer");
	impl->label->setWordWrap(true);

	impl->edit = new QTextEdit;
	impl->edit->setReadOnly(true);

	QGridLayout * lay = new QGridLayout( this );
	lay->addWidget( impl->label );
	lay->addWidget( impl->edit );

	this->setWindowTitle(QString("Wiki Viewer"));

    }

    WikiLiteView::~WikiLiteView()
    {
	delete impl;
    }

    void WikiLiteView::parseFile( QString const & fn )
    {
	QString code( impl->p->parseFile(fn) );
	impl->edit->setHtml( code );
	QString lbl( impl->p->metaTag( "summary" ) );
	if( lbl.isEmpty() ) lbl = QFileInfo(fn).fileName();
	impl->label->setText(lbl);
	this->setWindowTitle(QString("Wiki Viewer: %1").
			     arg(QFileInfo(fn).fileName()));
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
