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

// Define S11N_SERIALIZE_FUNCTOR and S11N_TYPE before including this file,
// as described in <s11n.net/s11n/reg_s11n_traits.hpp>.
/* s11n proxy template.*/
struct S11N_SERIALIZE_FUNCTOR
{
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, S11N_TYPE const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, S11N_TYPE & dest ) const;
};
#include <s11n.net/s11n/reg_s11n_traits.hpp>

