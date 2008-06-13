// Define S11N_SERIALIZE_FUNCTOR and S11N_TYPE before including this file,
// as described in <s11n.net/s11n/reg_s11n_traits.hpp>.
/* s11n proxy template.*/
struct S11N_SERIALIZE_FUNCTOR
{
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, S11N_TYPE const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, S11N_TYPE & dest );
};
#include <s11n.net/s11n/reg_s11n_traits.hpp>

