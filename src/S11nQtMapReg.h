////////////////////////////////////////////////////////////////////////
// A supermacro to generate some partial template specializations for
// s11n-proxying QMap-type classes.  See the various proxy files, like
// std/list.hpp, for how to use it.
////////////////////////////////////////////////////////////////////////
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

#ifndef S11N_MAP_TYPE
#error "You must define S11N_MAP_TYPE before including this file. e.g., to std::list or std::vector."
#endif

#ifndef S11N_MAP_TYPE_NAME
#error "You must define S11N_MAP_TYPE_NAME before including this file. e.g., to \"QList\" or \"QVector\"."
#endif

#ifndef S11N_MAP_TYPE_PROXY
#define S11N_MAP_TYPE_PROXY ::s11n::qt::serialize_qmap_f
#endif

#ifndef S11N_MAP_TYPE_DESER_PROXY
#  define S11N_MAP_TYPE_DESER_PROXY ::s11n::qt::deserialize_qmap_f
#endif


#include <s11n.net/s11n/s11n_debuggering_macros.hpp> // tracing commands

namespace s11n {

	/**
	   Specialization for std::map-compliant types.

	*/
	template <typename T1, typename T2>
	struct S11N_EXPORT_API default_cleanup_functor< S11N_MAP_TYPE<T1,T2> >
	{
		typedef S11N_MAP_TYPE<T1,T2> serializable_type;

		/**
		   ACHTUNG: it can only deallocate the .second member
		   of each mapped pair, because the first is const.
		   Since it would be very unusual to serialize a map
		   keyed on unmanaged pointers, this is not seen as a
		   major problem.

		   After this call, p is empty.
		*/
		void operator()( serializable_type & p ) const throw()
		{
			using namespace ::s11n::debug;
			S11N_TRACE(TRACE_CLEANUP) << "default_cleanup_functor<> specialization cleaning up map<>...\n";
			// reminder: pair type has a const .first member. If it weren't
			// for that pesky const, we could do this all in a simple
			// for_each() call.
			typedef typename serializable_type::mapped_type MT;
			typedef typename ::s11n::type_traits<MT>::type mapped_type; // strip pointer part
			typedef typename serializable_type::iterator MIT;
			MIT b = p.begin();
			MIT e = p.end();
			for( ; e != b; ++b )
			{
			    ::s11n::cleanup_serializable<mapped_type>( b.value() );
			}
			p.clear();
		}
	};
}

#define S11N_TEMPLATE_TYPE S11N_MAP_TYPE
#define S11N_TEMPLATE_TYPE_NAME S11N_MAP_TYPE_NAME
#define S11N_TEMPLATE_TYPE_PROXY S11N_MAP_TYPE_PROXY
#define S11N_TEMPLATE_TYPE_DESER_PROXY S11N_MAP_TYPE_DESER_PROXY
#include <s11n.net/s11n/proxy/reg_s11n_traits_template2.hpp>
#undef S11N_MAP_TYPE_DESER_PROXY
#undef S11N_MAP_TYPE_PROXY
#undef S11N_MAP_TYPE_NAME
#undef S11N_MAP_TYPE
