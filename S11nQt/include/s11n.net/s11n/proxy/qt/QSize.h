#ifndef S11NQT_QSize_H_INCLUDED
#define S11NQT_QSize_H_INCLUDED 1
#include <QSize>
namespace s11n { namespace qt {
/* s11n proxy for QSize.*/
struct QSize_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QSize const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QSize & dest ) const;
};
}} // namespace
/** register s11n proxy for QSize. */
#define S11N_TYPE QSize
#define S11N_TYPE_NAME "QSize"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QSize_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QSize_H_INCLUDED
