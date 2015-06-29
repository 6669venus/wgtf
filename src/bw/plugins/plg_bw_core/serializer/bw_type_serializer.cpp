#include "bw_type_serializer.hpp"
#include "serialization/i_datastream.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"


BWTypeSerializer::BWTypeSerializer()
{
	TypeId vec2Type = TypeId::getType<BW::Vector2>();
	TypeId vec3Type = TypeId::getType<BW::Vector3>();
	TypeId vec4Type = TypeId::getType<BW::Vector4>();

	writeFuncMap_[vec2Type] = 
		std::bind( &BWTypeSerializer::writeVector2, this, std::placeholders::_1, std::placeholders::_2 );
	writeFuncMap_[vec3Type] = 
		std::bind( &BWTypeSerializer::writeVector3, this, std::placeholders::_1, std::placeholders::_2 );
	writeFuncMap_[vec4Type] = 
		std::bind( &BWTypeSerializer::writeVector4, this, std::placeholders::_1, std::placeholders::_2 );

	readFuncMap_[vec2Type] = 
		std::bind( &BWTypeSerializer::readVector2, this, std::placeholders::_1, std::placeholders::_2 );
	readFuncMap_[vec3Type] = 
		std::bind( &BWTypeSerializer::readVector3, this, std::placeholders::_1, std::placeholders::_2 );
	readFuncMap_[vec4Type] = 
		std::bind( &BWTypeSerializer::readVector4, this, std::placeholders::_1, std::placeholders::_2 );
}

BWTypeSerializer::~BWTypeSerializer()
{
	writeFuncMap_.clear();
	readFuncMap_.clear();
}

const std::vector<TypeId>  BWTypeSerializer::getSupportedType()
{
	std::vector<TypeId> typeList;
	for(auto it : writeFuncMap_)
	{
		typeList.push_back( it.first );
	}
	return typeList;
}

bool BWTypeSerializer::write( IDataStream * dataStream, const Variant & variant )
{
	if(!dataStream)
	{
		return false;
	}
	TypeId type( variant.type()->name() );
	auto findIt = writeFuncMap_.find( type );
	if(findIt != writeFuncMap_.end())
	{
		return findIt->second( dataStream, variant );
	}
	else
	{
		assert( false );
		return false;
	}

}

bool BWTypeSerializer::writeVector2( IDataStream * dataStream, const Variant & variant )
{
	return dataStream->write( variant );
}

bool BWTypeSerializer::writeVector3( IDataStream * dataStream, const Variant & variant )
{
	return dataStream->write( variant );
}

bool BWTypeSerializer::writeVector4( IDataStream * dataStream, const Variant & variant )
{
	return dataStream->write( variant );
}

bool BWTypeSerializer::read( IDataStream * dataStream, Variant & variant )
{
	if(!dataStream)
	{
		return false;
	}
	TypeId type( variant.type()->name() );
	auto findIt = readFuncMap_.find( type );
	if(findIt != readFuncMap_.end())
	{
		return findIt->second( dataStream, variant );
	}
	else
	{
		assert( false );
		return false;
	}

}

bool BWTypeSerializer::readVector2( IDataStream * dataStream, Variant & variant )
{
	return dataStream->read( variant );
}

bool BWTypeSerializer::readVector3( IDataStream * dataStream, Variant & variant )
{
	return dataStream->read( variant );
}

bool BWTypeSerializer::readVector4( IDataStream * dataStream, Variant & variant )
{
	return dataStream->read( variant );
}
