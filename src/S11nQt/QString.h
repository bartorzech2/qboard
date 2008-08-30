#include <QString>
namespace s11n { namespace qt {
/**
	s11n proxy for QStrings. See the tryAscii member for important
	notes.

	As a special convenience, it can deserialize QByteArray data,
	in the assumption that it's a string. This can be used to take
	advantage of QByteArray_s11n's compression when you know that
	a given property will often contain large amounts of text.
*/
struct QString_s11n
{
	/** Serializes src to dest. See the tryAscii member for details. */
	bool operator()( S11nNode & dest, QString const & src ) const;
	/** Deserializes dest from src. It can read either format of
		serialized string (see tryAscii). */
	bool operator()( S11nNode const & src, QString & dest ) const;
	
	/**
		If tryAscii is true (the default) then serialized QStrings will be checked
		to see if they are made up of only ASCII values [0..127]. If so, then a human-readable
		representation is serialized, otherwise an unreadable (but unicode-friendly) format
		is used.
		
		It is intended that an application set this only once, and not twiddle it back and forth.
	*/
	static bool tryAscii;
};
}} // namespaces
/** register s11n proxy for QString. */
#define S11N_TYPE QString
#define S11N_TYPE_NAME "QString"
#define S11N_SERIALIZE_FUNCTOR s11n::qt::QString_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>
