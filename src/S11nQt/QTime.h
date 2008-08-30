#include <QTime>
namespace s11n { namespace qt {
/* s11n proxy for QTime.*/
struct QTime_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QTime const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QTime & dest ) const;
};
}} // namespace
/** register s11n proxy for QTime. */
#define S11N_TYPE QTime
#define S11N_TYPE_NAME "QTime"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QTime_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
