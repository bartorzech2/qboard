#ifndef S11NQT_QBrush_H_INCLUDED
#define S11NQT_QBrush_H_INCLUDED 1
#include <QBrush>
namespace s11n { namespace qt {
/* s11n proxy template.*/
struct QBrush_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QBrush const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QBrush & dest ) const;
};
}} // namespace
/** s11n proxy for QBrush. */
#define S11N_TYPE QBrush
#define S11N_TYPE_NAME "QBrush"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QBrush_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QBrush_H_INCLUDED
