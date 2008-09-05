#ifndef S11NQT_QFont_H_INCLUDED
#define S11NQT_QFont_H_INCLUDED 1
#include <QFont>
namespace s11n { namespace qt {
/* s11n proxy for QFont.

Handles only the following properties of QFont:

- weight
- fontFamily
- pointSizeF
- bold
- italic
*/
struct QFont_s11n
{
  /** Serializes src to dest. */
  bool operator()( S11nNode & dest, QFont const & src ) const;
  /** Deserializes dest from src. */
  bool operator()( S11nNode const & src, QFont & dest ) const;
};
}} // namespace
/** s11n proxy for QFont. */
#define S11N_TYPE QFont
#define S11N_TYPE_NAME "QFont"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QFont_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QFont_H_INCLUDED
