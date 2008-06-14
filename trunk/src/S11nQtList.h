#ifndef __S11NQTLIST_H__
#define __S11NQTLIST_H__
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

/**
	S11n routines specific to the QList/QVector types. Unfortunately we have to copy a large
	chunk of the s11n::list source code here just because the QList types don't have a
	swap() member, which s11n::list code relies on for more efficient deserialization. 
*/
#include <list>
#include <vector>
#include <set>
#include <memory>
#include <iterator> // insert_iterator
#include <algorithm> // for_each()
#include "S11nQt.h"
#include <s11n.net/s11n/variant.hpp> // lexical casting
#include <s11n.net/s11n/traits.hpp> // node_traits<>
#include <s11n.net/s11n/type_traits.hpp> // type_traits<>
#include <s11n.net/s11n/serialize.hpp> // core serialize funcs
#include <s11n.net/s11n/abstract_creator.hpp> // abstract_creator class
#include <s11n.net/s11n/exception.hpp> // s11n_exception class
#include <s11n.net/s11n/s11n_debuggering_macros.hpp> // tracing macros
#include <s11n.net/s11n/proxy/listish.hpp>

namespace s11n { namespace qt {
	using namespace s11n;
	template <typename NodeType, typename SerType>
	bool deserialize_qlist( const NodeType & src, SerType & dest )
	{ // this code is 100% identical to the copy in the s11n core
		// except that the Qt list types are missing a swap() function, which
		// that code requires.
		typedef typename SerType::value_type VT;
		typedef s11n::Private::abstract_creator<VT> ACVT;
		typedef typename NodeType::child_list_type::const_iterator CHIT;
		typedef node_traits<NodeType> TR;
		const NodeType * nch = 0;
		CHIT it = TR::children(src).begin();
		CHIT et = TR::children(src).end();
		if( et == it ) return true;
		VT ser; // reminder: might be a pointer type
		typedef typename ::s11n::type_traits<VT>::type VT_base;
		std::string implclass;
		using namespace ::s11n::debug;
		SerType buffer;
#define ERRMSG S11N_TRACE(TRACE_WARNING) << "deserialize_qlist(node,list) srcnode="<<std::dec<<&src << ": "
		for( ; et != it; ++it )
		{
			nch = *it;
			if( ! nch /* this never happens */ )
			{
				ERRMSG << "problemus internus: got a null child entry. "
				       << "Throwing and leaving list in its current state.\n";
				throw ::s11n::s11n_exception("%s:%d: Internal error: node children list contains a null pointer.", __FILE__, __LINE__ );
				return false;
			}
			implclass = TR::class_name(*nch);
			// instantiate a new child object to deserialize to...
			try
			{
				if( ! ACVT::create( ser, implclass ) ) // might throw
				{
					ERRMSG << "Internal error: abstract_creator<> "
					       << "could not create a new object of type '"
					       << implclass<<"'!\n";
					::s11n::cleanup_serializable<SerType>( buffer );
					return false;
				}
			}
			catch(...)
			{
				ERRMSG << "abstract_creator::create(...,"<<implclass<<") threw! Cleaning up and passing on the exception.\n";
				::s11n::cleanup_serializable<SerType>( buffer );
				throw;
			}
			// populate the child...
			try
			{
				// CERR << "This really works on pointers, right?\n";
				if( ! ::s11n::deserialize<NodeType,VT_base>( *nch, ser ) ) // might throw
				{
					ERRMSG << "deserialize_qlist(): deser of a child failed!\n"
					       << "name="<< TR::name(*nch)<< ". implclass="<< implclass
					       <<" @ " << std::hex<<nch <<"\n";
					::s11n::cleanup_serializable<VT_base>( ser );
					::s11n::cleanup_serializable<SerType>( buffer );
					return false;
				}
			}
			catch(...)
			{
				ERRMSG << "deserialize_qlist() got exception. Using cleanup_functor on failed child, then on container.\n";
				::s11n::cleanup_serializable<VT_base>( ser );
				::s11n::cleanup_serializable<SerType>( buffer );
				throw;
			}
			// add it to our list
			buffer.insert( buffer.end(), ser );
		} // for()
#undef ERRMSG
		dest = buffer;
		return true;
	}
	
	template <typename NodeType, typename SerType>
	bool deserialize_qlist( const NodeType & src,
					 	const std::string & subnodename,
					 	SerType & dest )
	{
		const NodeType * ch = s11n::find_child_by_name( src, subnodename );
		if( ! ch ) return false;
		return deserialize_qlist<NodeType,SerType>( *ch, dest );
	}

	struct serialize_qlist_f : ::s11n::list::serialize_list_f {};

	struct deserialize_qlist_f : ::s11n::deserialize_binary_f_tag
	{
		template <typename NodeType, typename SerType>
		inline bool operator()( const NodeType & src, SerType & dest ) const
		{
			return deserialize_qlist<NodeType,SerType>( src, dest );
		}
		template <typename NodeType, typename SerType>
		inline bool operator()( const NodeType & src, const std::string & subnodename, SerType & dest ) const
		{
			return deserialize_qlist<NodeType,SerType>( src, dest );
		}
	};
}} // namespaces


#define QTLIST_SERIALIZE_FUNCTOR s11n::qt::serialize_qlist_f
#define QTLIST_DESERIALIZE_FUNCTOR s11n::qt::deserialize_qlist_f

/* s11n proxy for QVector<SerializableT>. */
#define S11N_LIST_TYPE QVector
#define S11N_LIST_TYPE_NAME "QVector"
#define S11N_LIST_TYPE_PROXY QTLIST_SERIALIZE_FUNCTOR
#define S11N_LIST_TYPE_DESER_PROXY QTLIST_DESERIALIZE_FUNCTOR
#include "S11nQtListReg.h"

/* s11n proxy for QList<SerializableT>. */
#define S11N_LIST_TYPE QList
#define S11N_LIST_TYPE_NAME "QList"
#define S11N_LIST_TYPE_PROXY QTLIST_SERIALIZE_FUNCTOR
#define S11N_LIST_TYPE_DESER_PROXY QTLIST_DESERIALIZE_FUNCTOR
#include "S11nQtListReg.h"

#include <QPolygon>
#include <QPoint>
#define S11N_TYPE QPolygon
#define S11N_TYPE_NAME "QPolygon"
#if 0
   // this SHOULD work:
#  define S11N_BASE_TYPE QVector<QPoint>
#else
   // but it appears not to work, so we'll do a more verbose workaround:
#  define S11N_SERIALIZE_FUNCTOR QTLIST_SERIALIZE_FUNCTOR
#  define S11N_DESERIALIZE_FUNCTOR QTLIST_SERIALIZE_FUNCTOR
#endif
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#undef QTLIST_SERIALIZE_FUNCTOR
#undef QTLIST_DESERIALIZE_FUNCTOR

#endif // __S11NQTLIST_H__
