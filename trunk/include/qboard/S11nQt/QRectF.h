#ifndef S11NQT_QRectF_H_INCLUDED
#define S11NQT_QRectF_H_INCLUDED 1
#include <QRectF>
namespace s11n { namespace qt {
/* s11n proxy for QRectF.*/
struct QRectF_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QRectF const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QRectF & dest ) const;
};
}} // namespace
/** register s11n proxy for QRectF. */
#define S11N_TYPE QRectF
#define S11N_TYPE_NAME "QRectF"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QRectF_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QRectF_H_INCLUDED
