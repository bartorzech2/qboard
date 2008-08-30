#ifndef S11NQT_QPen_H_INCLUDED
#define S11NQT_QPen_H_INCLUDED 1
#include <QPen>
namespace s11n { namespace qt {
    /*
      s11n proxy for QPen.

      De/serializes the following properties of a QPen:

      - style
      - capStyle
      - joinStyle
      - miterLimit
      - width
      - brush
      - color
    */
    struct QPen_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QPen const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, QPen & dest ) const;
    };
}} // namespace
/** s11n proxy for QPen. */
#define S11N_TYPE QPen
#define S11N_TYPE_NAME "QPen"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QPen_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QPen_H_INCLUDED
