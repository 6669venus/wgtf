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

	void writeVector2( IDataStream * dataStream, const Variant & variant );
	void writeVector3( IDataStream * dataStream, const Variant & variant );
	void writeVector4( IDataStream * dataStream, const Variant & variant );

	void readVector2( IDataStream * dataStream, Variant & variant );
	void readVector3( IDataStream * dataStream, Variant & variant );
	void readVector4( IDataStream * dataStream, Variant & variant );

	std::unordered_map< const TypeId, std::function< void (IDataStream *, const Variant &)> > writeFuncMap_;
	std::unordered_map< const TypeId, std::function< void (IDataStream *, Variant &)> > readFuncMap_;

};

#endif // BW_TYPE_SERIALIZER_HPP
