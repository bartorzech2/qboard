#ifndef S11NQT_QLineF_H_INCLUDED
#define S11NQT_QLineF_H_INCLUDED 1
#include <QLineF>
namespace s11n { namespace qt {
/* s11n proxy for QLineF.*/
struct QLineF_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QLineF const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QLineF & dest ) const;
};
}} // namespace
/** register s11n proxy for QLineF. */
#define S11N_TYPE QLineF
#define S11N_TYPE_NAME "QLineF"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QLineF_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QLineF_H_INCLUDED
