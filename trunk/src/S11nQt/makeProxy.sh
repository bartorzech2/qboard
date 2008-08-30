#!/bin/bash
# Quick hack to create stub code for S11nQt proxies.
# Usage: $1 QtClassName
# Creates QtClassName.h and QtClassName.cpp
if [[ "$1x" = "x" ]]; then
    echo "Usage: $0 CLASSNAME"
    exit 1
fi

cl=$1

hpp=$cl.h
cpp=$cl.cpp
for i in $hpp $cpp; do
    if [[ -e $i ]]; then
	echo "Error: file already exists: $i"
	exit 2
    fi
done

guard="S11NQT_${cl}_H_INCLUDED"
lic=license.txt
test -f $lic || {
    cat <<EOF > $lic
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
EOF

}

ftor="${cl}_s11n"
cat <<EOF > $hpp
#ifndef ${guard}
#define ${guard} 1
#include <${cl}>
namespace s11n { namespace qt {
/* s11n proxy for ${cl}.*/
struct ${ftor}
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, ${cl} const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, ${cl} & dest ) const;
};
}} // namespace
/** register s11n proxy for ${cl}. */
#define S11N_TYPE ${cl}
#define S11N_TYPE_NAME "${cl}"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::${ftor}
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // ${guard}
EOF

cat <<EOF > $cpp
$(cat ${lic})
#include "S11nQt.h"
#include "${cl}.h"
EOF


sed -ne "/^bool ${ftor}/,/^}/p" < ../S11nQt.cpp >> $cpp

rm -f $lic 2>/dev/null
echo "#include \"S11nQt/${hpp}\""
echo "Output is in ${cpp} and ${hpp}"
