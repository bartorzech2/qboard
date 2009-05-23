#ifndef S11NQT_QPolygon_H_INCLUDED
#define S11NQT_QPolygon_H_INCLUDED 1
#include <s11n.net/s11n/proxy/qt/QList.h>
#include <QPoint>
#include <QPolygon>

/** register s11n proxy for QPolygon. */
#define S11N_TYPE QPolygon
#define S11N_TYPE_NAME "QPolygon"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::serialize_qlist_f
#define S11N_DESERIALIZE_FUNCTOR s11n::qt::deserialize_qlist_f
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#endif // S11NQT_QPolygon_H_INCLUDED
