#ifndef S11NQT_QPixmap_H_INCLUDED
#define S11NQT_QPixmap_H_INCLUDED 1
#include <QPixmap>
namespace s11n { namespace qt {
/* s11n proxy for QPixmap.*/
struct QPixmap_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QPixmap const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QPixmap & dest ) const;
};
}} // namespace
/** register s11n proxy for QPixmap. */
#define S11N_TYPE QPixmap
#define S11N_TYPE_NAME "QPixmap"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QPixmap_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QPixmap_H_INCLUDED
