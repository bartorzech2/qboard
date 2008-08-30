#ifndef S11NQT_QByteArray_H_INCLUDED
#define S11NQT_QByteArray_H_INCLUDED 1
#include <QByteArray>
namespace s11n { namespace qt {
    /**
       An s11n proxy for QByteArrays. See the compressionThreshold member
       for important notes.
       
       Note that since you can read/write directly from/to QByteArrays
       using the QIODevice interface, it is in principal possible to save
       arbitrary binary data using libs11n this way. Not necessarily an
       efficient way to save binary data, but maybe useful for some
       cases (e.g. saving QPixmaps as inlined data).
    */
    struct QByteArray_s11n
    {
	/**
	   Serializes src to dest, saving the data in a bin64-encoded
	   string. If src.size() is greater than
	   QByteArray_s11n::compressionThreshold then the serialized
	   copy is compressed using qCompress() before bin64 encoding
	   is applied.
	   
	   See the compressionThreshold for more info.
	*/
	bool operator()( S11nNode & dest, QByteArray const & src ) const;
	
	/** Deserializes dest from src. */
	bool operator()( S11nNode const & src, QByteArray & dest ) const;
	/**
	   The value of compressionThreshold influences how the serialize
	   operator works.
	   
	   It is intended that an application set this only once, and not
	   twiddle it back and forth.

	   If set to 0 then compression is disabled. The default value is
	   quite small - technically undefined, but "probably" around 100.
	*/
	static unsigned short compressionThreshold;
    };
}} // namespaces
/** s11n proxy for QByteArray. */
#define S11N_TYPE QByteArray
#define S11N_TYPE_NAME "QByteArray"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QByteArray_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#endif // S11NQT_QByteArray_H_INCLUDED
