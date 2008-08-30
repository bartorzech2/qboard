#ifndef S11NQT_QDate_H_INCLUDED
#define S11NQT_QDate_H_INCLUDED 1
#include <QDate>
namespace s11n { namespace qt {
/* s11n proxy template.*/
struct QDate_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QDate const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QDate & dest ) const;
};
}} // namespace
/** s11n proxy for QDate. */
#define S11N_TYPE QDate
#define S11N_TYPE_NAME "QDate"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QDate_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QDate_H_INCLUDED
