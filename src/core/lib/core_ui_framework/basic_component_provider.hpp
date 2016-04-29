#ifndef BASIC_COMPONENT_PROVIDER_HPP
#define BASIC_COMPONENT_PROVIDER_HPP

#include "core_ui_framework/i_component_provider.hpp"
#include "core_data_model/i_item_role.hpp"

ITEMROLE( valueType )
ITEMROLE( metaData )

template< typename ValueType, typename MetaDataType >
class BasicComponentProvider : public IComponentProvider
{
public:
	BasicComponentProvider( const char * component )
		: component_( component )
	{
		definitionManager_ = Context::queryInterface< IDefinitionManager >();
	}

	const char * component( std::function< Variant ( size_t ) > & dataPredicate ) const override
	{
		if (checkValueType< ValueType >( dataPredicate ) && checkMetaType< MetaDataType >( dataPredicate ))
		{
			return component_.c_str();
		}

		return nullptr;
	}

	template< typename T >
	bool checkValueType( std::function< Variant ( size_t ) > & dataPredicate ) const
	{
		auto data = dataPredicate( ItemRole::valueTypeId );
		
		ObjectHandle handle;
		if (!data.tryCast( handle ))
		{
			return false;
		}

		auto typeId = handle.getBase< TypeId >();
		if (typeId == nullptr)
		{
			return false;
		}

		return TypeId::getType< T >() == *typeId;
	}

	template< typename T >
	bool checkMetaType( std::function< Variant ( size_t ) > & dataPredicate ) const
	{
		auto data = dataPredicate( ItemRole::metaDataId );

		ObjectHandle handle;
		if (!data.tryCast( handle ))
		{
			return false;
		}

		auto metaHandle = safeCast< MetaBase >( handle );
		return findFirstMetaData( TypeId::getType< T >(), *metaHandle, *definitionManager_ ) != nullptr;
	}

	template<>
	bool checkValueType< void >( std::function< Variant ( size_t ) > & dataPredicate ) const
	{
		return true;
	}

	template<>
	bool checkMetaType< void >( std::function< Variant ( size_t ) > & dataPredicate ) const
	{
		return true;
	}

private:
	std::string component_;
	IDefinitionManager * definitionManager_;
};

#endif // BASIC_COMPONENT_PROVIDER_HPP