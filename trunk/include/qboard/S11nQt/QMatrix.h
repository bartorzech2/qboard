#ifndef S11NQT_QMatrix_H_INCLUDED
#define S11NQT_QMatrix_H_INCLUDED 1
#include <QMatrix>
namespace s11n { namespace qt {
/* s11n proxy for QMatrix.*/
struct QMatrix_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QMatrix const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QMatrix & dest ) const;
};
}} // namespace
/** register s11n proxy for QMatrix. */
#define S11N_TYPE QMatrix
#define S11N_TYPE_NAME "QMatrix"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QMatrix_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QMatrix_H_INCLUDED
