#ifndef S11NQT_QVariant_H_INCLUDED
#define S11NQT_QVariant_H_INCLUDED 1
#include <QVariant>
#include <QSharedDataPointer>
namespace s11n { namespace qt {
    namespace Detail {
	/**
	   SharedS11nNode is a QSharedData type implementing
	   a reference-counted S11nNode. It is intended
	   to be used with QSharedDataPointer<SharedS11nNode>.
	*/
	struct SharedS11nNode : public QSharedData
	{
	public:
	    S11nNode node;
	    SharedS11nNode() : QSharedData(), node()
	    {}
	    SharedS11nNode( const SharedS11nNode & rhs )
		: QSharedData(rhs), node( rhs.node )
	    {
	    }
	    SharedS11nNode & operator=( SharedS11nNode const & rhs )
	    {
		if( this != &rhs )
		{
		    this->node = rhs.node;
		}
		return *this;
	    }
	};
    } // namespace


    /**
       VariantS11n is type to allow QVariant to hold a *copy* of any
       Serializable.
    */
    class VariantS11n
    {
    public:
	/**
	   Returns the preferred type name for use with QVariant.
	*/
	static char const * variantTypeName();
	/**
	   Returns the id for VariantS11n which is registered via
	   qRegisterMetaType<VariantS11n>().
	*/
	static int variantType();

	VariantS11n();
	/**
	   Makes a deep copy of n.
	*/
	VariantS11n( S11nNode const & n);
	/**
	   Makes a ref-counted copy of src.
	*/
	VariantS11n & operator=( VariantS11n const & src );
	/**
	   Makes a ref-counted copy of src.
	*/
	VariantS11n( VariantS11n const & src );
	/**
	   Calls this->serialize(src). If serialization fails an exception
	   is thrown.
	*/
	template <typename SerT>
	VariantS11n( SerT const & src )
	    : proxy(new Detail::SharedS11nNode)
	{
	    if( ! this->serialize( src ) )
	    {
		QString msg = QString("VariantS11n(<Serializable[%1]> const &): serialization failed").
		    arg(s11n::s11n_traits<SerT>::class_name( &src ).c_str());
		throw std::runtime_error( msg.toAscii().constData() );
	    }
	}
	~VariantS11n();

	/**
	   Returns true if this object contains no serialized data.
	*/
	bool empty() const;

	/**
	   Equivalent to s11n::serialize(this->node(), src).
	*/
	template <typename SerT>
	bool serialize( SerT const & src )
	{
	    S11nNodeTraits::clear(proxy->node);
	    return s11n::serialize(proxy->node, src );
	}

	/**
	   Equivalent to s11n::deserialize(this->node(), dest).
	*/
	template <typename SerT>
	bool deserialize( SerT & dest ) const
	{
	    return s11n::deserialize( proxy->node, dest );
	}

	/**
	   Equivalent to s11n::deserialize<SerT>(this->node()). The
	   caller owns the returned pointer, which may be 0 on error.
	*/
	template <typename SerT>
	SerT * deserialize() const
	{
	    return s11n::deserialize<SerT>( proxy->node );
	}

	/**
	   Non-const accesor for this object's S11nNode.  Calling this
	   will force a deep copy if other VariantS11n objects are sharing
	   the underlying node.
	*/
	S11nNode & node();
	/**
	   Const accesor for this object's S11nNode.
	*/
	S11nNode const & node() const;

	/**
	   Clears the memory used by serialized data, making
	   this->node() usable again as a target for serialization.
	*/
	void clear();
    private:
	/**
	   Static initialization of this var will register
	   VariantS11n with the Qt metatype system.
	*/
	static const int registration;
	QSharedDataPointer<Detail::SharedS11nNode> proxy;
    };


    /**

    s11n proxy for QVariant.
    It supports the following QVariant types (as defined in the
    QVariant::Type enum):

    - String, StringList
    - Color (only ARGB format)
    - Point, PointF
    - Size, SizeF
    - Rect, RectF
    - Int, UInt
    - Line, LineF
    - List, Map
    - LongLong, ULongLong
    - Time, Date, DateTime
    - ByteArray
    - Pixmap
    - Brush, Pen
    - Any Serializable Type (as defined by libs11n), via the 
    VariantS11n proxy type.

    If an attempt is made to serialize a different type, serialization
    will fail. If, upon deserialization, the integer values defined by
    QVariant::Types have changed since the serialization, results are
    undefined, but will most likely result in an invalid QVariant value.
    */
    struct QVariant_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QVariant const & src ) const;
	/** Deserializes dest from src. If this routine returns false,
	    dest is guaranteed to have not been modified. That is, if
	    this routine fails, dest's state is the same as it was
	    before this function was called. */
	bool operator()( S11nNode const & src, QVariant & dest ) const;
	/**
	   Returns true only if this type can de/serialize QVariants
	   of the given type.
	*/
	static bool canHandle( int t );
    };
}} // namespace

/** register s11n proxy for QVariant. */
#define S11N_TYPE QVariant
#define S11N_TYPE_NAME "QVariant"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QVariant_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

// Make VariantS11n known to QVariant:
using s11n::qt::VariantS11n; // kludge: we don't want the namespace part stored by QMetaType
Q_DECLARE_METATYPE(VariantS11n);

#endif // S11NQT_QVariant_H_INCLUDED
