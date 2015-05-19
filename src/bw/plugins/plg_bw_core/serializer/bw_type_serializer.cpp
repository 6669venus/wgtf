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
		findIt->second( dataStream, variant );
	}
	else
	{
		assert( false );
		return false;
	}

	return true;
}

void BWTypeSerializer::writeVector2( IDataStream * dataStream, const Variant & variant )
{
	const BW::Vector2 & vec2 = variant.castRef<BW::Vector2>();
	dataStream->write( vec2.x );
	dataStream->write( vec2.y );
}

void BWTypeSerializer::writeVector3( IDataStream * dataStream, const Variant & variant )
{
	const BW::Vector3 & vec3 = variant.castRef<BW::Vector3>();
	dataStream->write( vec3.x );
	dataStream->write( vec3.y );
	dataStream->write( vec3.z );
}

void BWTypeSerializer::writeVector4( IDataStream * dataStream, const Variant & variant )
{
	const BW::Vector4 & vec4 = variant.castRef<BW::Vector4>();
	dataStream->write( vec4.x );
	dataStream->write( vec4.y );
	dataStream->write( vec4.z );
	dataStream->write( vec4.w );
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
		findIt->second( dataStream, variant );
	}
	else
	{
		assert( false );
		return false;
	}

	return true;
}

void BWTypeSerializer::readVector2( IDataStream * dataStream, Variant & variant )
{
	BW::Vector2 vec2;
	dataStream->read( vec2.x );
	dataStream->read( vec2.y );
	variant = vec2;
}

void BWTypeSerializer::readVector3( IDataStream * dataStream, Variant & variant )
{
	BW::Vector3 vec3;
	dataStream->read( vec3.x );
	dataStream->read( vec3.y );
	dataStream->read( vec3.z );
	variant = vec3;
}

void BWTypeSerializer::readVector4( IDataStream * dataStream, Variant & variant )
{
	BW::Vector4 vec4;
	dataStream->read( vec4.x );
	dataStream->read( vec4.y );
	dataStream->read( vec4.z );
	dataStream->read( vec4.w );
	variant = vec4;
}
