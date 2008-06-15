#ifndef QBOARD_MVC_MODEL_H_INCLUDED
#define QBOARD_MVC_MODEL_H_INCLUDED 1

#include <QObject>
#include <QSet>

class QVariant;
#include "Serializable.h"

class MVCModel : public QObject, public Serializable
{
    Q_OBJECT
public:
    MVCModel();
    virtual ~MVCModel();

    /**
       This is like QObject::setProperty(), but with two
       differences:

       - If the given QVariant is not a type supported by our
       serialization code (see the QVariant_s11n class) then this function
       returns false and refuses to set the property. Note, however,
       that QObject::setProperty() also returns false if it sets
       a "dynamic property", so false does not necessarily mean
       failure.

       - A propertySet() signal is fired on success.

       When a MVCModel is serialized, all properties set via
       this routine or setProperty() will be serialized. An invalid
       property type (i.e. one which isn't serializable) would cause
       serialization to fail, which is why this function doesn't
       accept them. Note, however, that setProperty() accepts any
       QVariant type.

       Property names MUST be usable as serialization keys, which
       essentialy means that they must be only alphanumeric/underscore,
       starting with non-digit. Dashes, colons, and such may not be
       supported by any given libs11n file format, so their use is
       highly discouraged.
    */
    bool setModelProperty( char const *, QVariant const & );

    /**
       Subclasses MUST override this to supply a unique name to this
       class of model.
    */
    //     virtual char const * modelName() const;

    /** Serializes this object to dest. */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );

    /** Returns true if this object has the given dynamic property, else false. */
    bool hasProperty( char const * );

    /**
       Reimplemented to fire propertySet() signals.
    */
    virtual bool event( QEvent * e );

    /**
       To be used by Views when they connect to this piece.  It
       increments the "view count" by 1. See removeViewRef().
    */
    void addViewRef();
    /**
       To be used by View (or similar) when they disconnect from this piece.
       It decrements the "view count" by 1. If the view count goes to 0, this object
       scedules itself for destruction using deleteLate().
    */
    void removeViewRef();

Q_SIGNALS:
    void propertySet( char const *, MVCModel * );
    /**
       Signal emited when this object destructs.
    */
    void destructing( MVCModel * gp );


private:
    struct Impl;
    Impl * impl;
};

/**
   MVCManagedList implements a list of MVCModels.
   All MVCModel items in this type are owned by it.
*/
class MVCManagedList : public QObject, public Serializable
{
    Q_OBJECT
private:
    MVCManagedList( MVCManagedList const & ); // not implemented
    MVCManagedList & operator=(MVCManagedList const &); // not implemented
public:
    MVCManagedList();
    virtual ~MVCManagedList();
    typedef QSet<MVCModel*> ListType;
    typedef ListType::iterator iterator;
    typedef ListType::const_iterator const_iterator;
    iterator begin() { return this->m_list.begin(); }
    const_iterator begin() const { return this->m_list.begin(); }
    iterator end() { return this->m_list.end(); }
    const_iterator end() const { return this->m_list.end(); }
    /** Serializes this object to dest. */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );
    bool empty() const {return m_list.empty();}
    size_t size() const {return m_list.size();}
    /* Moves all objects from other into this list. */
    void takeObjects( MVCManagedList &other);
    Q_SIGNALS:
    void objectAdded( MVCModel * );
    void objectRemoved( MVCModel * );
public Q_SLOTS:
    /* Transfers ownership of gp to this object and emits objectAdded(gp) */
    void addModel( MVCModel * gp );

    /* Removes gp from our internal list and emits objectRemoved(gp).

    Transfers ownership of gp to the caller.

    If this object does not contain gp then no signal is fired
    and false is returned.
    */
    bool removeModel( MVCModel * gp );
    /** Deletes all objects in the list. */
    void clearModels();
    /**
       Clears the list without deleting the contained items. Ownership
       is effectively undefined - make sure you get the items you need
       before you do this.
    */
    void clearNoDelete();
private Q_SLOTS:
    /** Connects gp to this	object's internal handlers. */ 
    void connect( MVCModel *gp);
    void disconnect( MVCModel *gp);
private:
    ListType m_list;
};

#define S11N_TYPE MVCModel
#define S11N_TYPE_NAME "MVCModel"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE MVCModel
#define S11N_TYPE_NAME "MVCModel"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>


// Register MVCManagedList with s11n:
#define S11N_TYPE MVCManagedList
#define S11N_TYPE_NAME "MVCManagedList"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE MVCManagedList
#define S11N_TYPE_NAME "MVCManagedList"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // QBOARD_MVC_MODEL_H_INCLUDED

