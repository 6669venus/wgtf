#include <cstdio>
#include "reflection_serializer.hpp"
#include "serialization/i_datastream.hpp"
#include "reflection/i_object_manager.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/property_iterator.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "variant/interfaces/i_meta_type_manager.hpp"
#include "command_system/i_command_manager.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"

ReflectionSerializer::ReflectionSerializer( ISerializationManager & serializationManager , 
										    IMetaTypeManager & metaTypeManager, 
										    IObjectManager & objManager )
	: serializationManager_( serializationManager )
	, metaTypeManager_( metaTypeManager )
	, objManager_( objManager )
	, curDataStream_( nullptr )
{
	typeList.push_back( TypeId::getType<ObjectHandle>() );
}

ReflectionSerializer::~ReflectionSerializer()
{
	curDataStream_ = nullptr;
	typeList.clear();
}

std::vector<TypeId> ReflectionSerializer::getSupportedType()
{
	return typeList;
}

bool ReflectionSerializer::write( IDataStream * dataStream, const Variant & variant )
{
	TypeId type( variant.type()->name() );
	curDataStream_ = dataStream;
	if(!curDataStream_)
	{
		return false;
	}

	if(variant.canCast<ObjectHandle>())
	{
		ObjectHandle provider;
		bool isOk = variant.tryCast( provider );
		assert( isOk );
		if(provider.isValid())
		{
			const auto classDef = provider.getDefinition();
			assert( classDef != nullptr );
			curDataStream_->write( classDef->getName() );
			std::string stringId = "";
			RefObjectId id;
			isOk = provider.getId( id );
			if (isOk)
			{
				stringId = id.toString();
			}
			curDataStream_->write( stringId );
			writeProperties( provider );
			return true;
		}
		else
		{
			// write empty string for classdefinition name to handle nullptr
			curDataStream_->write( "" );
			return true;
		}
	}
	else
	{
		assert( false );
	}
	return false;
}

void ReflectionSerializer::writeProperties( const ObjectHandle & provider )
{
	const auto classDef = provider.getDefinition();
	assert( classDef );
	const PropertyIteratorRange& props = classDef->allProperties();
	std::vector< PropertyAccessor > pas;
	for (PropertyIterator pi = props.begin(), end = props.end(); 
		pi != end; ++pi)
	{
		const PropertyAccessor& pa = classDef->bindProperty( 
			pi->getName(), provider );
		assert( pa.isValid() );
		auto metaData = findFirstMetaData<MetaNoSerializationObj>( pa );
		if(metaData != nullptr)
		{
			continue;
		}
		pas.push_back( pa );
	}
	size_t count = pas.size();
	curDataStream_->write( count );
	for(auto pa : pas)
	{
		writeProperty( pa );
	}
}

void ReflectionSerializer::writeProperty( const PropertyAccessor & property )
{
	assert( property.isValid() );
	const auto & propPath = property.getFullPath();
	curDataStream_->write( propPath );

	const auto & type = property.getType();
	curDataStream_->write( type.getName() );
	Variant value = property.getValue();
	curDataStream_->write( value.type()->name() );
	if(!value.isVoid())
	{
		writePropertyValue( type, value );
	}
	
}

void ReflectionSerializer::writeCollection( const Collection & collection )
{
	TypeId type = collection.valueType();
	curDataStream_->write( type.getName() );
	size_t count = collection.size();
	curDataStream_->write( count );
	for (auto it = collection.begin(), end = collection.end();
		it != end; ++it )
	{
		unsigned int index;
		it.key().tryCast( index );
		//! TODO! how to write the key if we don't know its type?
		char buffer[260];
		_snprintf( buffer, 260, "%d", index);
		std::string strIndex(buffer);
		curDataStream_->write( strIndex );

		Collection subCollection;
		auto value = it.value();
		curDataStream_->write( value.type()->name() );
		writePropertyValue( type, value );
	}
}

void ReflectionSerializer::writePropertyValue( const TypeId propType, const Variant & value )
{
	if( value.typeIs< Collection >() )
	{
		Collection collection;
		bool isCollection = value.tryCast( collection );
		assert( isCollection );
		writeCollection( collection );
	}
	else
	{
		bool hasId = value.typeIs< ObjectHandle >();
		if(hasId)
		{
			ObjectHandle provider;
			bool isOk = value.tryCast( provider );
			assert( isOk );
			if(provider.isValid())
			{
				RefObjectId id;
				hasId = provider.getId( id );
				if(hasId)
				{
					curDataStream_->write( id.toString() );
				}
				else
				{	
					hasId = false;
				}
			}
			else
			{
				hasId = false;
			}
		}
		if (!hasId )
		{
			curDataStream_->write( "" );
			serializationManager_.serialize( *curDataStream_, value );
		}
	}
}

bool ReflectionSerializer::read( IDataStream * dataStream, Variant & variant )
{
	curDataStream_ = dataStream;
	if(!curDataStream_)
	{
		return false;
	}
	
	ObjectHandle provider;
	if(!variant.tryCast( provider ))
	{
		assert( false );
		return false;
	}
	std::string classDefName;
	curDataStream_->read( classDefName );
	if(!classDefName.empty())
	{
		std::string id;
		curDataStream_->read( id );
		if(id.empty())
		{
			if(provider.isValid())
			{
				readProperties( provider );
			}
			else
			{
				auto polyStruct = objManager_.create( classDefName.c_str() );
				readProperties( polyStruct );
				variant = polyStruct;
			}
		}
		else
		{
			auto object = 
				objManager_.createObject( RefObjectId( id.c_str() ), classDefName.c_str() );
			readProperties( object );
			variant = object;
		}
	}
	return true;
}

void ReflectionSerializer::readProperties( const ObjectHandle & provider )
{
	assert( provider.isValid() );
	size_t count = 0;
	curDataStream_->read( count );
	for(size_t i = 0; i < count; i++)
	{
		readProperty( provider );
	}
}

void ReflectionSerializer::readProperty( const ObjectHandle & provider )
{
	assert( provider.isValid() );
	std::string propName;
	curDataStream_->read( propName );
	
	PropertyAccessor prop = provider.getDefinition()->bindProperty(
		propName.c_str(), provider );
	assert( prop.isValid() );

	std::string typeName;
	curDataStream_->read( typeName );
	const TypeId type( typeName.c_str() );
	TypeId propType = prop.getType();
	assert( type == propType );
	std::string valueType;
	curDataStream_->read( valueType );
	const MetaType * metaType = 
		metaTypeManager_.findType( valueType.c_str() );
	if(metaType == nullptr)
	{
		assert( false );
		return;
	}
	readPropertyValue( type, valueType.c_str(), prop );
}

void ReflectionSerializer::readCollection( const PropertyAccessor & prop )
{
	assert( prop.isValid() );
	ObjectHandle baseProvider = prop.getRootObject();
	std::string typeName;
	curDataStream_->read( typeName );
	const TypeId type( typeName.c_str() );
	size_t count = 0;
	curDataStream_->read( count );

	Collection collection;
	bool isCollection = prop.getValue().tryCast( collection );
	assert( isCollection );
	if (!collection.empty())
	{
		collection.erase(collection.begin(), collection.end());
	}
	
	std::string strIndex;
	std::string propName;
	std::string valueType;
	for (size_t i = 0; i < count; i++)
	{
		strIndex.clear();
		curDataStream_->read( strIndex );
		int index = atoi( strIndex.c_str() );
		auto it = collection.insert( index );
		
		propName = prop.getFullPath();
		propName += '[';
		propName += strIndex;
		propName += ']';

		//TODO: Allow iteration to next element in collection.
		PropertyAccessor pa = baseProvider.getDefinition()->bindProperty( 
			propName.c_str(), baseProvider);
		assert( pa.isValid() );
		valueType.clear();
		curDataStream_->read( valueType );
		const MetaType * metaType = 
			metaTypeManager_.findType( valueType.c_str() );
		if(metaType == nullptr)
		{
			assert( false );
			return;
		}
		readPropertyValue( type, valueType.c_str(), pa );
	}
}

void ReflectionSerializer::readPropertyValue( const TypeId propType, const char * valueType, PropertyAccessor & pa )
{
	const MetaType * metaType = 
		metaTypeManager_.findType( valueType );
	if(metaType == nullptr)
	{
		assert( false );
		return;
	}
	Variant value = pa.getValue();
	
	if (value.typeIs< Collection >())
	{
		readCollection( pa );
		return;
	}
	std::string id;
	curDataStream_->read( id );
	if (ReflectionUtilities::isStruct( pa ))
	{
		read( curDataStream_, value );
	}
	else
	{
		if (!id.empty())
		{
			auto obj = objManager_.getObject( id );
			if(obj == nullptr)
			{
				objManager_.addObjectLinks( id, pa );
			}
			else
			{
				pa.setValueWithoutNotification( obj );
			}
		}
		else
		{
			Variant variant( metaType );
			if(!variant.isVoid())
			{
				serializationManager_.deserialize( *curDataStream_, variant );
				pa.setValueWithoutNotification( variant );
			}
		}
	}
}
