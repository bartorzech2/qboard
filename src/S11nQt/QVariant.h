#ifndef S11NQT_QVariant_H_INCLUDED
#define S11NQT_QVariant_H_INCLUDED 1
#include <QVariant>
namespace s11n { namespace qt {
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
#endif // S11NQT_QVariant_H_INCLUDED
