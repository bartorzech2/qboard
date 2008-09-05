#ifndef S11NQT_QTransform_H_INCLUDED
#define S11NQT_QTransform_H_INCLUDED 1
#include <QTransform>
namespace s11n { namespace qt {
/* s11n proxy for QTransform.*/
struct QTransform_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QTransform const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QTransform & dest ) const;
};
}} // namespace
/** register s11n proxy for QTransform. */
#define S11N_TYPE QTransform
#define S11N_TYPE_NAME "QTransform"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QTransform_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QTransform_H_INCLUDED
