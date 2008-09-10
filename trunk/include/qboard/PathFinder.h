#ifndef QBOARD_PathFinder_H_INCLUDED
#define QBOARD_PathFinder_H_INCLUDED 1
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
#include <QObject>
#include <qboard/Serializable.h>
#include <QStringList>

namespace qboard {

    /**
       PathFinder is a utility for searching for files based on a set of
       prefixes (normally directories) and suffixes (normally file
       extensions).
    */
    class PathFinder :
	public QObject,
	public Serializable
    {
    Q_OBJECT
    public:
	explicit PathFinder( QObject * parent = 0 );
	virtual ~PathFinder();

	/**
	   Serializes this object to dest.
	*/
	virtual bool serialize( S11nNode & dest ) const;

	/**
	   Deserializes this object from src.
	*/
	virtual bool deserialize( S11nNode const & src );

	/**
	   Returns all search prefixes added via addPrefix().
	*/
	Q_INVOKABLE QStringList prefixes() const;
	/**
	   Returns all search suffixes added via addSuffix().
	*/
	Q_INVOKABLE QStringList suffixes() const;

	/**
	   Adds a search prefix. If autoAppendSlash is true (the default
	   then a '/' is added to the prefix if it doesn't contain one
	   already.

	   See find() for more info.

	   Returns the added prefix, including the added slash (if
	   applicable).
	*/
	Q_INVOKABLE QString addPrefix( QString const &, bool autoAppendSlash = true );

	/**
	   Adds a search suffix, as described in the find() docs.
	*/
	Q_INVOKABLE void addSuffix( QString const & );

	/**
	   Removes the last prefix entry which matches p. Note that
	   if it was added using addPrefix(p,true) then this routine
	   will only match if p contains that trailing slash.

	   If all is true then all matching entries are removed.
	*/
	Q_INVOKABLE void removePrefix( QString const & p, bool all = false );

	/**
	   Removes the last prefix entry which matches s.

	   If all is true then all matching entries are removed.
	*/
	Q_INVOKABLE void removeSuffix( QString const & s, bool all = false );

	/**
	   Return true if this object has no suffixes and no prefixes.
	*/
	Q_INVOKABLE bool empty() const;

	/**
	   Clears all search prefixes and suffixes.
	*/
	Q_INVOKABLE void clear();

	enum SearchFlags {
	InternalUseOnly = 0,
	Files = 1,
	Dirs = 2,
	All = Files | Dirs
	};

	/**
	   Searches for the given file using the following algorithm:

	   For each search prefix (added via addPrefix()) and suffix
	   (addSuffix()), the following patterns are searched:

	   - prefix + baseName + suffix

	   The prefix and suffix lists are used in the order they were
	   added with addPrefix() or addSuffix(). When searching, each
	   list implicitly starts with empty entry, such that, in effect,
	   the following patterns are checked:

	   - baseName
	   - baseName + suffix
	   - prefix + baseName
	   - prefix + baseName + suffix

	   If a file or dir is found, and flags is set to allow that type
	   of entry, the match is returned.

	   When no match is found, an empty string is returned.

	   The flags parameter may be one of Files, Dirs, or All. If only
	   Files is set (the default), then directory names (and symlinks
	   pointing to dirs) will never be returned as matches.

	   Note that a search prefix need not be a directory. It could be,
	   e.g. "/usr/lib/lib". Combined with a addSuffix(".so"),
	   find("foo") could find /usr/lib/libfoo.so.
	*/
	Q_INVOKABLE QString find( QString const & baseName, SearchFlags flags = Files ) const;

    private:
	friend class PathFinder_s11n;
	struct Impl;
	Impl * impl;
    };

} // namespace

#endif
