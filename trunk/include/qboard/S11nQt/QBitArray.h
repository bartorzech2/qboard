#ifndef S11NQT_QBitArray_H_INCLUDED
#define S11NQT_QBitArray_H_INCLUDED 1
#include <QBitArray>
namespace s11n { namespace qt {
    /*
      s11n proxy for QBitArray.

      It stores QBitArrays as a series of encoded bytes,
      squeezing them into groups of 8. Thus a QBitArray
      with 800 bits is stored as a series of 100 small
      numbers.
    */
    struct QBitArray_s11n
    {
	/** Serializes src to dest. */
	bool operator()( S11nNode & dest, QBitArray const & src ) const;
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, QBitArray & dest ) const;
    };

}} // namespace

/** register s11n proxy for QBitArray. */
#define S11N_TYPE QBitArray
#define S11N_TYPE_NAME "QBitArray"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QBitArray_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QBitArray_H_INCLUDED
