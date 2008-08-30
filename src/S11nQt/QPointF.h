#include <QPointF>
namespace s11n { namespace qt {
    /*
      s11n proxy for QPointF.

      De/serializes the x/y coordinates of a point.
    */
    struct QPointF_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QPointF const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, QPointF & dest ) const;
    };
}} // namespace
/** register s11n proxy for QPointF. */
#define S11N_TYPE QPointF
#define S11N_TYPE_NAME "QPointF"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QPointF_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
