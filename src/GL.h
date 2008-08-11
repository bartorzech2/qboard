#ifndef QBOARD_GL_H_INCLUDED
#define QBOARD_GL_H_INCLUDED

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


#include <Qt/qconfig.h>

/**
   Using GL mode for QBoardView makes many paint operations much
   faster, especially when zoomed/rotated.  However, the screen is not
   always updated properly in GL mode. :(
*/
#if ! defined(QBOARD_USE_OPENGL)
#  ifndef QT_NO_OPENGL
#    define QBOARD_USE_OPENGL 1
#  else
#    define QBOARD_USE_OPENGL 0
#  endif
#endif

#endif // QBOARD_GL_H_INCLUDED
