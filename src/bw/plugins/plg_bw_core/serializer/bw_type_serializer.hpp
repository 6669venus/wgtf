#ifndef BW_TYPE_SERIALIZER_HPP
#define BW_TYPE_SERIALIZER_HPP

#include "serialization/serializer/i_serializer.hpp"
#include "variant/type_id.hpp"
#include <vector>
#include <unordered_map>


class BWTypeSerializer : public ISerializer
{
public:
	BWTypeSerializer();
	~BWTypeSerializer();

	const std::vector<TypeId> getSupportedType();
private:
	// ISerializer
	bool write( 
		IDataStream * dataStream, const Variant & variant ) override;
	bool read( 
		IDataStream * dataStream, Variant & variant ) override;

	bool writeVector2( IDataStream * dataStream, const Variant & variant );
	bool writeVector3( IDataStream * dataStream, const Variant & variant );
	bool writeVector4( IDataStream * dataStream, const Variant & variant );

	bool readVector2( IDataStream * dataStream, Variant & variant );
	bool readVector3( IDataStream * dataStream, Variant & variant );
	bool readVector4( IDataStream * dataStream, Variant & variant );

	std::unordered_map< const TypeId, std::function< bool (IDataStream *, const Variant &)> > writeFuncMap_;
	std::unordered_map< const TypeId, std::function< bool (IDataStream *, Variant &)> > readFuncMap_;

};

#endif // BW_TYPE_SERIALIZER_HPP
