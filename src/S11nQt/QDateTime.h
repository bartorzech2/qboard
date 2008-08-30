#ifndef S11NQT_QDateTime_H_INCLUDED
#define S11NQT_QDateTime_H_INCLUDED 1
#include <QDateTime>
namespace s11n { namespace qt {
/* s11n proxy template.*/
struct QDateTime_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QDateTime const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QDateTime & dest ) const;
};
}} // namespace
/** s11n proxy for QDateTime. */
#define S11N_TYPE QDateTime
#define S11N_TYPE_NAME "QDateTime"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QDateTime_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QDateTime_H_INCLUDED
