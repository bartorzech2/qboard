#ifndef QBOARD_WikiLiteView_H_INCLUDED
#define QBOARD_WikiLiteView_H_INCLUDED 1
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


#include <QObject>
#include <QWidget>
#include "Serializable.h"

namespace qboard {
class WikiLiteView : public QWidget
{
Q_OBJECT
public:
    WikiLiteView( QWidget * parent = 0 );
    virtual ~WikiLiteView();

public Q_SLOTS:
    void parseFile( QString const & fn );
private:
    void updateScrollbars();
    struct Impl;
    Impl * impl;
};

} // namespace
#endif // QBOARD_WikiLiteView_H_INCLUDED
