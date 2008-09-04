#ifndef QBOARD_WikiLiteParser_H_INCLUDED
#define QBOARD_WikiLiteParser_H_INCLUDED 1
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


#include <QString>
#include <QIODevice>
#include "parsepp.hpp"

namespace qboard {
    using namespace Ps;

    /**
       This class implements a basic Wiki-like parser, the goal
       being to emulate Google Code's wiki fairly closely.

       Currently working wiki features:

       *bold*

       _italics_

       =Header1=
       ...
       =====Header5=====

       `fixed width font`

       ^Superscript^

       ,,Subscript,,

       ~~Strikethrough~~

       ---- (horizontal rule)

       Numered and unnumbered lists, with consecutive lines, each
       starting with at least two spaces followed by a single '*'
       (unnumbered) or '#' (numbered).  Nested lists are not supported
       because figuring out when we need to close a level is a pain in
       the ass.

       {{{
       verbatim (code) blocks (PRE tags)
       }}}


       || Tables || like || this ||

       (but a difference from Google: the first row's elements are always
       TH items, not TD.)

       [WikiLinks with optional label text], but note that non-URLs
       linked this way are always relative to the current URL.

       [url://... with optional label text]

       [url://link url://link_to_image.{gif,jpg,png}] for inlined images.


       Most HTML tags creates by this class have a CSS class of 'WLP'
       (as in Wiki-Lite-Parser), so one may customize their appearance
       by configuring that class via CSS.


       NOTABLE TODOs:

       - Allow client to set a base URL which will be added to
       generated links.

       - Allow client to set per-element type style settings.

       - There are no plans to support non-bracketed WikiWords. Nor
       are there plans to automatically link URL-looking strings.

       KNOWN ANNOYANCES:

       - H# tags should only be triggered at the beginning of a line.


       OTHER NOTES:

       Keep in mind that wiki parsing is fragile by nature, so don't
       expect this to gracefully handle wildly invalid input.

       The line spacing could be handled differently. Right now we use
       BR tags, but we should wrap the paragraphs in P tags. Knowing
       exactly when to open and close them is problematic here,
       though.

    */
    class WikiLiteParser
    {
    public:
	WikiLiteParser();
	~WikiLiteParser();

	/**
	   Parses the wiki-formated input text and sends the output
	   (HTML-format) to the given output device.

	   If the parsing engine throws an exception, this routine will
	   stop parsing and will append the error string to the output,
	   then return false.
	*/
	bool parse( QString const & wiki,
		    QIODevice * out );
	/**
	   Parses the wiki-formated input text and returns the result
	   (HTML format) as a string.
	*/
	QString parse( QString const & code );

	/**
	   Parses the given file as a wiki file and returns the
	   expanded result. On error an empty string will be returned,
	   but an empty string is also returned if the input file
	   is empty.
	*/
	QString parseFile( QString const & fileName );

	/**
	   Metatags are a feature of google code wiki
	   which look like this:

	   #key some arbitrary value

	   which appear at the beginning of a wiki document.

	   Such keys are filtered out of parsed output but can be
	   fetched using this function. The value applies to the most
	   recently parse()'d document.

	   On error, or if no such metatag exists.
	*/
	QString metaTag( QString const & key ) const;

// 	QString setMetaInfo( QString const & key, QString const & value );
    private:
	struct Impl;
	Impl * impl;
    };

}

#endif // QBOARD_WikiLiteParser_H_INCLUDED
