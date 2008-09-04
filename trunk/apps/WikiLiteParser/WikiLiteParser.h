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


       All HTML tags creates by this class have a CSS class of 'WLP'
       (as in Wiki-Lite-Parser), so one may customize their appearance
       by configuring that class via CSS.


       NOTABLE TODOs:

       || Tables ||

       [WikiLinks with optional label text]

       [link_to_image.{jpg,png} optional label text]

       There are no plans to support non-bracketed WikiWords.


       Other notes:

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
	WikiLiteParser()
	{}
	~WikiLiteParser()
	{}

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

    };

}

#endif // QBOARD_WikiLiteParser_H_INCLUDED
