#ifndef __S11NQTMAP_H__
#define __S11NQTMAP_H__
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
	S11n routines specific to the QMap types. Unfortunately we
	have to copy a large chunk of the s11n::map source code here
	due to differences in the QMap-vs-std::map APIs.
*/
#include <list>
#include <vector>
#include <set>
#include <memory>
#include <iterator> // insert_iterator
#include <algorithm> // for_each()
#include <s11n.net/s11n/variant.hpp> // lexical casting
#include <s11n.net/s11n/traits.hpp> // node_traits<>
#include <s11n.net/s11n/type_traits.hpp> // type_traits<>
#include <s11n.net/s11n/serialize.hpp> // core serialize funcs
#include <s11n.net/s11n/abstract_creator.hpp> // abstract_creator class
#include <s11n.net/s11n/exception.hpp> // s11n_exception class
#include <s11n.net/s11n/s11n_debuggering_macros.hpp> // tracing macros
#include <s11n.net/s11n/proxy/mapish.hpp>
#include "S11nQt.h"

namespace s11n { namespace qt {
    using namespace s11n;

    template <typename NodeType, typename MapType>
    bool serialize_qmap( NodeType & dest, const MapType & src )
    {
	typedef typename MapType::const_iterator CIT;
	typedef typename MapType::mapped_type mapped_type;
	typedef typename MapType::key_type key_type;
	typedef QPair<key_type,mapped_type> PairType;
	typedef node_traits<NodeType> TR;
	typedef ::s11n::s11n_traits<MapType> STR;
	if( ! TR::empty( dest ) )
	{
	    throw ::s11n::s11n_exception( "%s:%d: serialize_map(tgt_node=[%s]) requires that the target node be empty.",
					  __FILE__,
					  __LINE__,
					  TR::name(dest).c_str() );
	}
	TR::class_name( dest, STR::class_name(&src) );
	CIT b = src.begin();
	CIT e = src.end();
	for( ; e != b; ++b )
	{
	    std::auto_ptr<NodeType> ch( TR::create("pair") );
	    PairType bogo( b.key(), b.value() );
	    if( ! ::s11n::map::serialize_pair<NodeType,PairType>( *ch, bogo ) )
	    {
		using namespace ::s11n::debug;
		S11N_TRACE(TRACE_ERROR) << "serialize_map: child pair failed serialize.\n";
		return false;
	    }
	    dest.children().push_back( ch.release() ); // transfer ownership
	}
	return true;
    }

    template <typename NodeType, typename MapType>
    bool serialize_qmap( NodeType & dest,
				   const std::string & subnodename,
				   const MapType & src )
    {
	typedef node_traits<NodeType> TR;
	std::auto_ptr<NodeType> ch( TR::create(subnodename) );
	if( ! serialize_qmap<NodeType,MapType>( *ch, src ) )
	{
	    return false;
	}
	TR::children(dest).push_back( ch.release() );
	return true;
    }


    template <typename NodeType, typename MapType>
    bool deserialize_qmap( const NodeType & src, MapType & dest )
    {
	typedef node_traits<NodeType> TR;
	typedef typename NodeType::child_list_type::const_iterator CIT;
	//typedef typename SerializableType::value_type VT;
	// ^^^ no, because VT::first_type is const!
	// Thus we hand-create a compatible pair type:
	typedef typename MapType::key_type KType;
	typedef typename MapType::mapped_type VType;
	typedef std::pair< KType, VType > PairType;
	PairType pair;
	CIT b = TR::children(src).begin();
	CIT e = TR::children(src).end();
	if( e == b ) return true;
	const NodeType *ch = 0;
	using namespace ::s11n::debug;
#define ERRMSG S11N_TRACE(TRACE_ERROR) << "deserialize_qmap(node,map) srcnode="<<std::dec<<&src << ": "
	MapType buffer;
	try
	{
	    for( ; e != b ; ++b )
	    {
		ch = *b;
		if( ! ::s11n::map::deserialize_pair<NodeType,PairType>( *ch, pair ) )
		{ // ^^^ is this fails, pair is now guaranteed to be unmodified (1.1.3)
		    ERRMSG << "map child failed deser: node name='"<<TR::name(*ch)<<"'. Cleaning up map.\n";
		    ::s11n::cleanup_serializable<MapType>( buffer );
		    return false;
		}
		buffer.insert( pair.first, pair.second );
	    }
	}
	catch(...)
	{
	    ERRMSG << "Passing on exception. Map cleaned using s11n_traits::cleanup_functor.\n";
	    ::s11n::cleanup_serializable<MapType>( buffer );
	    throw;
	}
#undef ERRMSG
	dest = buffer;
	return true;
    }

    template <typename NodeType, typename MapType>
    bool deserialize_map( const NodeType & src,
			  const std::string & subnodename,
			  MapType & dest )
    {
	const NodeType * ch = ::s11n::find_child_by_name( src, subnodename );
	if( ! ch ) return false;
	return deserialize_qmap<NodeType,MapType>( *ch, dest );
    }

    struct serialize_qmap_f : ::s11n::serialize_binary_f_tag
    {
	template <typename NodeType, typename SerType>
	inline bool operator()( NodeType & dest, SerType const & src ) const
	{
	    return serialize_qmap<NodeType,SerType>( dest, src );
	}
    };

    struct deserialize_qmap_f : ::s11n::deserialize_binary_f_tag
    {
	template <typename NodeType, typename SerType>
	inline bool operator()( const NodeType & src, SerType & dest ) const
	{
	    return deserialize_qmap<NodeType,SerType>( src, dest );
	}
    };
}} // namespaces


#define QTMAP_SERIALIZE_FUNCTOR s11n::qt::serialize_qmap_f
#define QTMAP_DESERIALIZE_FUNCTOR s11n::qt::deserialize_qmap_f

/* s11n proxy for QMap<SerializableT,SerializableT>. */
#define S11N_MAP_TYPE QMap
#define S11N_MAP_TYPE_NAME "QMap"
#define S11N_MAP_TYPE_PROXY QTMAP_SERIALIZE_FUNCTOR
#define S11N_MAP_TYPE_DESER_PROXY QTMAP_DESERIALIZE_FUNCTOR
#include "S11nQtMapReg.h"


#undef QTMAP_SERIALIZE_FUNCTOR
#undef QTMAP_DESERIALIZE_FUNCTOR

#endif // __S11NQTMAP_H__
