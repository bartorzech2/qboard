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

    class WikiLiteParser
    {
    public:
	WikiLiteParser()
	{}
	~WikiLiteParser()
	{}

	bool parse( QString const & code,
		    QIODevice * out );
    };

}

#endif // QBOARD_WikiLiteParser_H_INCLUDED
