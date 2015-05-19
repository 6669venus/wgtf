#ifndef REFLECTION_SERIALIZER_HPP
#define REFLECTION_SERIALIZER_HPP

#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/property_accessor.hpp"
#include "serialization/serializer/i_serializer.hpp"


class ObjectHandle;

/**
 * reflected object Serializer
 */
class ReflectionSerializer : public ISerializer
{
public:
	ReflectionSerializer( ISerializationManager & serializationManager, 
						  IMetaTypeManager & metaTypeManager, 
						  IObjectManager & objManager );
	~ReflectionSerializer();

	std::vector<TypeId> getSupportedType();

private:
	bool write( 
		IDataStream * dataStream, const Variant & variant ) override;
	bool read( 
		IDataStream * dataStream, Variant & variant ) override;

	void writeProperties( const ObjectHandle & provider );
	void writeProperty( const PropertyAccessor & property );
	void writePropertyValue( const TypeId propType, const Variant & value );
	void writeCollection( const Collection & collection );

	void readProperties( const ObjectHandle & provider );
	void readProperty( const ObjectHandle & provider );
	void readPropertyValue( const TypeId propType, const char * valueType, PropertyAccessor & pa );
	void readCollection( const PropertyAccessor & prop );
	

	ISerializationManager & serializationManager_;
	IMetaTypeManager & metaTypeManager_;
	IObjectManager & objManager_;
	IDataStream * curDataStream_;
	typedef std::vector< std::pair< PropertyAccessor, RefObjectId > > ObjLinks;
	std::vector<TypeId> typeList;
	ObjLinks objLinks_;
};

#endif // REFLECTION_SERIALIZER_HPP
