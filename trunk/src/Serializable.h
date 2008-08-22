#ifndef __QBOARD_SERIALIZABLE_H_INCLUDED__
#define __QBOARD_SERIALIZABLE_H_INCLUDED__
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


#include "S11n.h"
#include <QString>

#include <string>
#include <iostream>
/**
   Base type for polymorphic QBoard serializable types.
   We use the libs11n serialization framework to do
   the leg work, so see those docs for the significance
   of types like S11nNode.
*/
class Serializable
{

public:
    virtual ~Serializable();
    /**
       This default implementation sets the polymorphic type's class
       name in dest. Subclasses should always call this implementation
       (or, more properly, that of their direct Serializable ancestor)
       as their first serialization operation, and return false or
       throw if this function fails.
    */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object.

    This default implementation only verifies that the class name
    stored in dest is the same as this type's polymorphic class name.
    If it is not, this function throws.
	
    Subclasses should always call this implementation as their
    first deserialization operation, and return false or throw
    if this function fails.
    */
    virtual bool deserialize( S11nNode const & src );
    /**
       Returns the class name of this type as used by
       the de/serialization process. See the protected
       setter for more information.
    */
    char const * s11nClass() const;
    /**
       Tries to deserialize the contents of the given
       file into this object. If the file does not contain
       data for the same polymorphic type as this object,
       deserialization will fail.

       Returns true on success, false on error.

       Subclasses may reimplement to, e.g., support other
       file types.
    */
    virtual bool load( QString const & );
    /**
       Loads this object from the given stream.
     */
    virtual bool load( std::istream & );
    /**
       Serializes this object's state to the given file.  If a
       s11nFileExtension() has been set and autoAddFileExtension is
       true, the extension is automatically appended to the name if
       the name doesn't have that extension already.

       Returns true on success, false on error.

       The autoAddFileExtension parameter SHOULD default to true,
       but i don't like the idea of virtuals having default values.
    */
    virtual bool save( QString const &, bool autoAddFileExtension ) const;
    /**
       Saves this object to the given stream using serialize().
    */
    virtual bool save( std::ostream & ) const;
    /**
       Returns the file extension associated with this type.
    */
    char const * s11nFileExtension();
	
    /**
       Returns true if the given file name has a trailing
       suffix equal to s11nFileExtension().
       Note that it does a case-insensitive check.
		
       The intention is that client applications can ask an object
       if it knows how to handle a specific file before attempting
       to deserialize it. It's just a half-ass check, as a file
       extension doesn't have to map to a specific file type, but
       it's useful in some cases.
    */
    bool fileNameMatches( QString const & ) const;
	
    /**
       Creates a polymorphic clone of this object via serialization.
    */
    Serializable * clone() const;


protected:
    /**
       Uses de/serialize() to create a polymorphic copy of rhs.
       On error (e.g. an attempt to copy between two different
       polymorphic types) it throws.
    */
    Serializable & copy( Serializable const & rhs );
    /**
       Does nothing.
    */
    Serializable & operator=( Serializable const & rhs );
    /**
       Does nothing.
    */
    Serializable( Serializable const & );

    /** This sets s11nClass(cn) and sets s11nFileExtension()
	to cn prefixed by a '.'. e.g. if cn=="MyType" then
	s11nFileExtension(".MyType") is set. See s11nClass()
	for more info.
    */
    explicit Serializable(char const * cn);
    /**
       Subclasses must set this to ensure that
       polymorphic deserialization works properly.
		
       Note that the string is NOT copied, so it must
       point to static memory which must not change
       for the lifetime of any objects of this (sub)type.
    */
    void s11nClass( char const * );
    /**
       Sets the file extension for this type. When
       save() is called, that file extension is
       appended to the filename automatically
       if needed. The extension may be 0.
		
       Note that the "dot" part of the extension
       must be explicit. e.g. use 
       s11nFileExtension(".foo") instead of s11nFileExtension("foo").
    */
    void s11nFileExtension( char const * );

private:
    struct Impl;
    Impl * impl;
};
/**
   An s11n proxy for to forward de/serialization calls to
   the Serializable type's interface.
*/
struct Serializable_s11n
{
    /** Returns src.serialize(dest). */
    bool operator()( S11nNode & dest, Serializable const & src ) const;
    /** Returns dest.deserialize(src). */
    bool operator()( S11nNode const & src, Serializable & dest ) const;
};

////////////////////////////////////////////////////////////////////////
// Register Serializable with s11n...
// Set up a custom s11n_traits<> specialization...
namespace s11n {
    template <typename SubT>
    struct s11n_traits < SubT, Serializable >
    {
	typedef SubT serializable_type;
	typedef Serializable serializable_interface_type;
	typedef Serializable_s11n serialize_functor;
	typedef Serializable_s11n deserialize_functor;
	typedef ::s11n::cl::object_factory< serializable_interface_type > factory_type;
	typedef ::s11n::default_cleanup_functor< serializable_interface_type > cleanup_functor;
	static const std::string class_name( const serializable_interface_type * instance_hint )
	{
	    return instance_hint ? instance_hint->s11nClass() : "Serializable";
	}
    };
} // namespace
#define S11N_FACREG_TYPE Serializable
#define S11N_FACREG_TYPE_NAME "Serializable"
#define S11N_FACREG_INTERFACE_TYPE Serializable
#define S11N_FACREG_TYPE_IS_ABSTRACT 1
#include <s11n.net/s11n/factory_reg.hpp>


#endif // __QBOARD_SERIALIZABLE_H_INCLUDED__
