#ifndef S11NQT_QPair_H_INCLUDED
#define S11NQT_QPair_H_INCLUDED 1
#include <QPair>
#include <s11n.net/s11n/proxy/mapish.hpp>
namespace s11n {
  template <typename T1, typename T2>
    struct default_cleanup_functor< QPair<T1,T2> >
    {
      typedef T1 first_type;
      typedef T2 second_type;
      typedef QPair<T1,T2> serializable_type;
      void operator()( serializable_type & p ) const
      {
	using namespace s11n::debug;
	S11N_TRACE(TRACE_CLEANUP) << "default_cleanup_functor<> specialization cleaning up QPair<>...\n";
	typedef typename type_traits<first_type>::type TR1;
	typedef typename type_traits<second_type>::type TR2;
	cleanup_serializable<TR1>( p.first );
	cleanup_serializable<TR2>( p.second );
      }
    };
}

#define S11N_TEMPLATE_TYPE QPair
#define S11N_TEMPLATE_TYPE_NAME "QPair"
#define S11N_TEMPLATE_TYPE_PROXY ::s11n::map::serialize_pair_f
#define S11N_TEMPLATE_TYPE_DESER_PROXY ::s11n::map::deserialize_pair_f
#include <s11n.net/s11n/proxy/reg_s11n_traits_template2.hpp>
#endif // S11NQT_QPair_H_INCLUDED
