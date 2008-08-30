#include <QRegExp>
namespace s11n { namespace qt {
/* s11n proxy for QRegExp.*/
struct QRegExp_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QRegExp const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QRegExp & dest ) const;
};
}} // namespace
/** register s11n proxy for QRegExp. */
#define S11N_TYPE QRegExp
#define S11N_TYPE_NAME "QRegExp"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QRegExp_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
