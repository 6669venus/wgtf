#ifndef I_SERIALIZER_HPP
#define I_SERIALIZER_HPP
#include "variant/variant.hpp"
#include <vector>
class IDataStream;
class ISerializationManager;
/**
 * Serializer interface
 */

class ISerializer 
{
public:
	virtual ~ISerializer(){}
private:
	friend class ISerializationManager;
	virtual bool write( 
		IDataStream * dataStream, const Variant & variant ) = 0;
	virtual bool read( 
		IDataStream * dataStream, Variant & variant ) = 0;

};
#endif // I_SERIALIZER_HPP
