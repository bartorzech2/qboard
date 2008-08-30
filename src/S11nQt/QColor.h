#ifndef S11NQT_QColor_H_INCLUDED
#define S11NQT_QColor_H_INCLUDED 1
#include <QColor>
namespace s11n { namespace qt {
/* s11n proxy template.*/
struct QColor_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QColor const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QColor & dest ) const;
};
}} // namespace
/** s11n proxy for QColor. */
#define S11N_TYPE QColor
#define S11N_TYPE_NAME "QColor"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QColor_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QColor_H_INCLUDED
