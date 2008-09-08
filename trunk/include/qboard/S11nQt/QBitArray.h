#ifndef S11NQT_QBitArray_H_INCLUDED
#define S11NQT_QBitArray_H_INCLUDED 1
#include <QBitArray>
namespace s11n { namespace qt {
    /*
      s11n proxy for QBitArray.

      Stores QBitArrays using a relatively space-efficient algorithm
      (efficient when compared to most other s11n proxies, anyway).
    */
    struct QBitArray_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QBitArray const & src ) const;
	/**
	   Deserializes dest from src.

	   Dest is only modified if this routine succeeds.
	*/
	bool operator()( S11nNode const & src, QBitArray & dest ) const;
    };

}} // namespace

/** register s11n proxy for QBitArray. */
#define S11N_TYPE QBitArray
#define S11N_TYPE_NAME "QBitArray"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QBitArray_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QBitArray_H_INCLUDED
