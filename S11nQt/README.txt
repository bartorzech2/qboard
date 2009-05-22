This is the README for libS11nQt.

This library contains the core libs11n library (http://s11n.net) plus
GPL-licensed s11n extensions for the Qt toolkit
(http://www.qtsoftware.com). It can in theory be used as a drop-in
replacement for vanilla libs11n builds, but is intended for direct
inclusion in s11n-using Qt-based projects.

It is maintained as part of the QBoard project:

http://code.google.com/p/qboard

Author: Stephan Beal (http://wanderinghorse.net/home/stephan)


========================================================================
BUILDING AND INSTALLING

Building requires the Qt qmake tool (only tested with Qt 4.4+, but
might work with earlier 4.x releases):

 ~> qmake
 ~> make

The library will be built to ./build/$QT_VERSION/... To install it, simply
copy it to somewhere in your libs path. If you need the headers, simply copy
./include/s11n.net to some directory in your includes path. If you already have
libs11n instead, then you only need to copy ./include/s11n.net/s11n/qt into your
existing <s11n.net/s11n> directory and copy ./include/s11n.net/s11n/proxy/qt
to <s11n.net/s11n/proxy>.
