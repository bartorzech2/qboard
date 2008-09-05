#ifndef S11NQT_QPoint_H_INCLUDED
#define S11NQT_QPoint_H_INCLUDED 1
#include <QPoint>
namespace s11n { namespace qt {
    /*
      s11n proxy for QPoint.

      De/serializes the x/y coordinates of a point.
    */
    struct QPoint_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QPoint const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, QPoint & dest ) const;
    };
}} // namespace
/** s11n proxy for QPoint. */
#define S11N_TYPE QPoint
#define S11N_TYPE_NAME "QPoint"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QPoint_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QPoint_H_INCLUDED
