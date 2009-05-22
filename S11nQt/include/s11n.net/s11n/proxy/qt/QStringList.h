#ifndef S11NQT_QStringList_H_INCLUDED
#define S11NQT_QStringList_H_INCLUDED 1
#include <QStringList>
#include "QList.h"
namespace s11n { namespace qt {
/* s11n proxy for QStringList.*/
struct QStringList_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QStringList const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QStringList & dest ) const;
};
}} // namespace
/** register s11n proxy for QStringList. */
#define S11N_TYPE QStringList
#define S11N_TYPE_NAME "QStringList"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QStringList_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QStringList_H_INCLUDED
