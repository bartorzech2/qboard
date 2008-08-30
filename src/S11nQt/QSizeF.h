#include <QSizeF>
namespace s11n { namespace qt {
/* s11n proxy for QSizeF.*/
struct QSizeF_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QSizeF const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QSizeF & dest ) const;
};
}} // namespace
/** register s11n proxy for QSizeF. */
#define S11N_TYPE QSizeF
#define S11N_TYPE_NAME "QSizeF"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QSizeF_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
