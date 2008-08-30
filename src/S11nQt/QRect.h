#ifndef S11NQT_QRect_H_INCLUDED
#define S11NQT_QRect_H_INCLUDED 1
#include <QRect>
namespace s11n { namespace qt {
/* s11n proxy for QRect.*/
struct QRect_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QRect const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QRect & dest ) const;
};
}} // namespace
/** register s11n proxy for QRect. */
#define S11N_TYPE QRect
#define S11N_TYPE_NAME "QRect"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QRect_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QRect_H_INCLUDED
