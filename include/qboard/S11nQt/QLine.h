#ifndef S11NQT_QLine_H_INCLUDED
#define S11NQT_QLine_H_INCLUDED 1
#include <QLine>
namespace s11n { namespace qt {
/* s11n proxy for QLine.*/
struct QLine_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QLine const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QLine & dest ) const;
};
}} // namespace
/** register s11n proxy for QLine. */
#define S11N_TYPE QLine
#define S11N_TYPE_NAME "QLine"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QLine_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QLine_H_INCLUDED
